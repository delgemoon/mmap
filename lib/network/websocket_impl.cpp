//
// Created by tammd on 8/28/22.
//

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <fmt/ostream.h>
#include <network/event.hpp>
#include <network/websocket_impl.hpp>
#include <network/websocket_key.hpp>
#include <network/ws_type.hpp>
#include <spdlog/spdlog.h>

#include <cassert>
#include <regex>
using namespace std::literals;
namespace
{

boost::asio::awaitable< boost::asio::ip::tcp::resolver::results_type >
resolve(std::string const &host, std::string const &port)
{
    boost::asio::ip::tcp::resolver resolver(co_await boost::asio::this_coro::executor);
    co_return co_await resolver.async_resolve(host, port, boost::asio::use_awaitable);
}

boost::asio::awaitable< void >
connect_tcp(boost::beast::tcp_stream &stream, boost::asio::ip::tcp::resolver::results_type results)
{
    spdlog::trace("connecting tcp");
    stream.expires_after(30s);
    auto ep = co_await stream.async_connect(results, boost::asio::use_awaitable);
    boost::ignore_unused(ep);
    spdlog::trace("connected on {}", ep);
}

boost::asio::awaitable< void >
connect_tls(boost::beast::ssl_stream< boost::beast::tcp_stream > &stream, std::string const &host)
{
    spdlog::trace("tls handshake [host {}]", host);
    network::websocket::set_sni(stream, host);
    stream.next_layer().expires_after(30s);
    co_await stream.async_handshake(boost::asio::ssl::stream_base::client, boost::asio::use_awaitable);
}

}   // namespace
namespace network::websocket
{

[[nodiscard]] auto
connection_impl::connect(boost::asio::ssl::context &sslctx,
                         std::string                url,
                         boost::beast::http::fields headers,
                         std::chrono::milliseconds  pingpong_timeout) -> boost::asio::awaitable< void >
{
    assert((co_await boost::asio::this_coro::cancellation_state).cancelled() == boost::asio::cancellation_type::none);
    static auto url_regex =
        std::regex(R"regex((ws|wss|http|https)://([^/ :]+):?([^/ ]*)(/?[^ #?]*)\x3f?([^ #]*)#?([^ ]*))regex",
                   std::regex_constants::icase);
    auto match = std::smatch();
    if (not std::regex_match(url, match, url_regex))
        throw boost::system::system_error(boost::asio::error::invalid_argument, "invalid url");

    auto &protocol = match[1];
    auto &host     = match[2];
    auto &port_ind = match[3];
    auto &path     = match[4];
    auto &query    = match[5];
    auto &fragment = match[6];

    auto transport = deduce_scheme(protocol, port_ind);
    auto port      = deduce_port(protocol, port_ind);

    auto to = boost::beast::websocket::stream_base::timeout {
        .handshake_timeout = std::chrono::seconds(30),
        .idle_timeout = pingpong_timeout.count() ? pingpong_timeout / 2 : boost::beast::websocket::stream_base::none(),
        .keep_alive_pings = pingpong_timeout.count() ? true : false
    };
    auto &tcp_layer = ws_.next_layer().next_layer();

    // connect tcp
    co_await connect_tcp(tcp_layer, co_await resolve(host.str(), port));

    // tls handshake
    co_await connect_tls(ws_.next_layer(), host.str());

    // websocket handshake

    spdlog::info("handshaking");
    ws_.set_option(boost::beast::websocket::stream_base::decorator(
        [headers](boost::beast::websocket::request_type &req)
        {
            for (auto &&field : headers)
                req.insert(field.name(), field.value());
        }));
    boost::beast::websocket::response_type response;
    try
    {
        co_await ws_.async_handshake(
            response, host.str(), build_target(path, query, fragment), boost::asio::use_awaitable);
    }
    catch (...)
    {
        if (response.at(boost::beast::http::field::content_type) == "text/html")
            spdlog::info("Exception during client_handshake {} for {} - sent HTML {}",
                         std::current_exception(),
                         url,
                         response.base());
        else
            spdlog::info(
                "Exception during client_handshake {} for {} - response {}", std::current_exception(), url, response);
        throw;
    }

    tcp_layer.expires_never();
}

connection_impl::connection_impl(executor_type exec)
: exec_(std::move(exec))
, last_error_()
, close_reason_()
{
}

boost::asio::awaitable< void >
connection_impl::start()
{
    auto ec = error_code();
    do
    {
        auto msg = message();
        co_await ws_.async_read(msg., boost::asio::redirect_error(boost::asio::use_awaitable, ec)));

        if (ec)
        {
            if (ec == boost::beast::websocket::error::closed)
                set_close_reason(ws_.reason());
            else
                set_last_error(ec);
            queue_event(event(last_error_));
        }
        else
        {
            msg.type(ws_.text() ? message::text : message::binary);
            queue_event(event(std::move(msg)));
        }
    } while (!ec);

    ws_.close();
}

[[nodiscard]] boost::asio::awaitable< void >
connection_impl::stop()
{
    assert(co_await boost::asio::this_coro::executor == get_executor());
    co_await close(boost::beast::websocket::close_code::going_away);
}

boost::asio::awaitable< std::size_t >
connection_impl::send(std::string text)
{
    assert(co_await boost::asio::this_coro::executor == get_executor());
    static unsigned long seq_ = 0;
    auto                 seq  = seq_++;

    BOOST_SCOPE_EXIT_ALL(&)
    {
        log::trace< "{} exit scope {}-{}" >(*this, __func__, seq);
        // This allows the next sender to progress
        may_send_ = true;
        may_send_condition_.notify_one();

        // This allows the start() coroutine to complete
        if (--senders_waiting_ == 0)
            senders_waiting_condition_.notify_one();
    };

    if (last_error_)
        throw system_error(last_error_);

    ws_.text();
    auto written = co_await ws_.async_write(boost::asio::buffer(text));
    co_return written;
}

boost::asio::awaitable< void >
connection_impl::close(boost::beast::websocket::close_reason reason)
{
    assert(co_await boost::asio::this_coro::executor == get_executor());

    if (set_close_reason(reason))
    {
        co_await ws_.async_close(reason, boost::asio::use_awaitable);
    }
}

auto
connection_impl::consume() -> boost::asio::awaitable< event >
{
}

auto
connection_impl::queue_event(event e) -> void
{
    event_queue_.push(std::move(e));
    event_available_.notify_all();
}

bool
connection_impl::set_last_error(const error_code &ec)
{
    if (last_error_)
        return false;

    last_error_ = ec;
    return true;
}

bool
connection_impl::set_close_reason(boost::beast::websocket::close_reason reason)
{
    auto ret = set_last_error(boost::beast::websocket::error::closed);
    if (ret)
        close_reason_ = std::move(reason);
    return ret;
}

boost::asio::awaitable< void >
connection_impl::wait_for_event()
{
}

std::ostream &
operator<<(std::ostream &os, connection_impl const &impl)
{
    return os;
}

}   // namespace network::websocket