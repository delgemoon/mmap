//
// Created by tammd on 8/24/22.
//

#include <boost/asio/experimental/awaitable_operators.hpp>
#include <fmt/core.h>
#include <network/http_connection_impl.hpp>
#include <spdlog/spdlog.h>
#include <util/explainer.hpp>
#include <util/format.hpp>

namespace
{

using namespace boost::asio::experimental::awaitable_operators;
boost::asio::awaitable< std::tuple< error_code, boost::asio::ip::tcp::resolver::results_type > >
resolve(std::string const &hostname, std::string const &port, std::chrono::milliseconds timeout)
{
    error_code                           error;
    net::ip::tcp::resolver::results_type results;

    auto cancel = (co_await net::this_coro::cancellation_state).cancelled();
    if (cancel != net::cancellation_type::none)
    {
        error = net::error::operation_aborted;
    }
    else
    {
        auto exec = co_await net::this_coro::executor;

        auto resolver = boost::asio::ip::tcp::resolver(exec);
        auto timer    = boost::asio::steady_timer(exec);
        timer.expires_after(timeout);

        try
        {
            auto which = co_await (timer.async_wait(boost::asio::use_awaitable) ||
                                   resolver.async_resolve(hostname, port, net::use_awaitable));

            switch (which.index())
            {
            case 0:
                error = net::error::operation_aborted;
                break;
            case 1:
                results = std::move(std::get< 1 >(which));
                break;
            }
        }
        catch (...)
        {
            spdlog::default_logger_raw()->critical("Error in Resolve endpoint {}", std::current_exception());
            throw;
        }
    }
    co_return std::make_tuple(error, std::move(results));
}

boost::asio::awaitable< error_code >
connect_tcp(network::http::tcp_layer            &tcp,
            net::ip::tcp::resolver::results_type results,
            std::chrono::milliseconds            timeout)
{
    error_code ec;
    auto       cancel = (co_await net::this_coro::cancellation_state).cancelled();
    if (cancel != net::cancellation_type::none)
    {
        ec = net::error::operation_aborted;
    }
    else
    {
        tcp.expires_after(timeout);
        auto endpoint = co_await tcp.async_connect(results, net::redirect_error(net::use_awaitable, ec));
        boost::ignore_unused(endpoint);
    }
    co_return ec;
}

boost::asio::awaitable< error_code >
handshake(network::http::tls_layer &tls, std::string const &hostname, std::chrono::milliseconds timeout)
{
    error_code ec;
    auto       cancel = (co_await net::this_coro::cancellation_state).cancelled();
    if (cancel != net::cancellation_type::none)
    {
        ec = net::error::operation_aborted;
    }
    else
    {
        if (SSL_set_tlsext_host_name(tls.native_handle(), hostname.c_str()))
        {
            tls.next_layer().expires_after(timeout);
            co_await tls.async_handshake(boost::asio::ssl::stream_base::client,
                                         net::redirect_error(net::use_awaitable, ec));
        }
        else
            ec.assign(static_cast< int >(::ERR_get_error()), net::error::get_ssl_category());
    }
    co_return ec;
}
}   // namespace
namespace network::http
{
connection_impl::connection_impl(executor_type              exec,
                                 boost::asio::ssl::context &ssl_ctx,
                                 const connection_key      &key,
                                 connect_options            options)
: executor_(std::move(exec))
, ssl_ctx_(ssl_ctx)
, options_(options)
, stream_(std::make_shared< tls_layer >(exec, ssl_ctx))
, key_(key)
{
}

boost::asio::awaitable< std::tuple< error_code, response_type > >
connection_impl::get(std::string const &url, boost::beast::http::fields headers, request_options options)
{
    auto impl = weak_from_this().lock();
    spdlog::info("url {}", url);
    assert(co_await net::this_coro::executor == executor_);
    auto request = boost::beast::http::request< boost::beast::http::string_body >(
        boost::beast::http::verb::get, url, 11, std::string(), std::move(headers));
    request.set(boost::beast::http::field::host, key_.hostname_);
    request.prepare_payload();

    co_return co_await rest_call(request, options);
}
boost::asio::awaitable< std::tuple< error_code, response_type > >
connection_impl::post(std::string const         &url,
                      std::string                data,
                      boost::beast::http::fields headers,
                      request_options            options)
{
    auto impl = weak_from_this().lock();
    assert(co_await net::this_coro::executor == executor_);
    auto request = request_class(boost::beast::http::verb::post, url, 11, std::move(data), std::move(headers));
    request.set(boost::beast::http::field::host, key_.hostname_);
    request.prepare_payload();
    co_return co_await rest_call(request, options);
}
net::awaitable< error_code >
connection_impl::connect()
{
    auto impl = weak_from_this().lock();
    assert(co_await net::this_coro::executor == executor_);
    error_code ec;
    auto       cancel = (co_await boost::asio::this_coro::cancellation_state).cancelled();
    if (cancel != boost::asio::cancellation_type::none)
        co_return boost::asio::error::operation_aborted;
    if (stream_)
    {
        spdlog::info("hostname {}, service {}", key_.hostname_, key_.service_);
        auto [ec, results] = co_await resolve(key_.hostname_, key_.service_, options_.resolve_timeout);
        if (!ec)
        {
            ec = co_await connect_tcp(stream_->next_layer(), results, options_.connect_timeout);
            if (!ec)
                ec = co_await handshake(*stream_, key_.hostname_, options_.handshake_timeout);
        }
    }
    co_return ec;
}
boost::asio::awaitable< std::tuple< error_code, response_type > >
connection_impl::rest_call(request_class const &request, request_options const &options)
{
    auto impl     = weak_from_this().lock();
    auto response = std::make_unique< response_class >();
    auto ec       = error_code();

    ec = co_await connect();

    if (!ec)
        ec = co_await rest_call_impl(request, *response, options, impl);
    else if (!ec)
        ec = boost::asio::error::basic_errors::not_connected;

    co_return std::make_tuple(ec, std::move(response));
}

boost::asio::awaitable< error_code >
connection_impl::rest_call_impl(request_class const   &request,
                                response_class        &response,
                                request_options const &options,
                                std::shared_ptr< connection_impl >)
{
    auto ec = error_code();
    if ((co_await net::this_coro::cancellation_state).cancelled() != net::cancellation_type::none)
        ec = net::error::operation_aborted;

    auto &tcp = stream_->next_layer();

    if (!ec)
    {
        tcp.expires_after(options.write_timeout);
        co_await boost::beast::http::async_write(*stream_, request, net::redirect_error(net::use_awaitable, ec));
    }

    if (ec)
        spdlog::default_logger_raw()->error(
            "send failed: {} with request {} : {}", ec, request.target(), request.body());

    if (!ec && ((co_await net::this_coro::cancellation_state).cancelled() != net::cancellation_type::none))
        ec = net::error::operation_aborted;

    if (!ec)
    {
        tcp.expires_after(options.read_timeout);
        co_await boost::beast::http::async_read(
            *stream_, buffer_, response, net::redirect_error(net::use_awaitable, ec));
        if (ec)
            spdlog::default_logger_raw()->error(
                "send failed: {} with request {} : {}", ec, request.target(), request.body());
    }

    if (ec)
    {
        // less interested in how this works out than the previous error
        tcp.socket().shutdown(net::ip::tcp::socket::shutdown_both, ec);
        if (ec)
            spdlog::default_logger_raw()->warn(
                "shutdown failed: {} with request {} : {}", ec, request.target(), request.body());
        tcp.close();
    }
    else if (response.need_eof())
    {
        tcp.socket().shutdown(net::ip::tcp::socket::shutdown_both, ec);
        if (ec)
            spdlog::default_logger_raw()->warn(
                "shutdown failed: {} with request {} : {}", ec, request.target(), request.body());
        tcp.close();
    }
    stream_ = std::make_shared< tls_layer >(executor_, ssl_ctx_);
    co_return ec;
}

net::awaitable< std::tuple< error_code, response_type > >
connection_impl::call(boost::beast::http::verb   method,
                      std::string const         &target,
                      std::string                data,
                      boost::beast::http::fields headers,
                      request_options            options)
{
    assert(co_await net::this_coro::executor == executor_);
    auto impl    = weak_from_this().lock();
    auto request = boost::beast::http::request< boost::beast::http::string_body >(
        method, target, 11, std::move(data), std::move(headers));
    request.set(boost::beast::http::field::host, key_.hostname_);
    request.prepare_payload();
    co_return co_await rest_call(request, options);
}

}   // namespace network::http
