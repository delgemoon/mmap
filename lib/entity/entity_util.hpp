//
// Created by tammd on 8/28/22.
//

#ifndef CONNECTOR_ENTITY_UTIL_HPP
#define CONNECTOR_ENTITY_UTIL_HPP

#include <boost/container/flat_map.hpp>
#include <boost/unordered_map.hpp>

#include <type_traits>

namespace entity
{
template < typename T >
struct identify_tag
{
};

template < typename T >
struct to_cache_tag
{
};

template < typename T >
struct from_cache_tag
{
};

template < typename T >
auto
tag_invoke(to_cache_tag< T >, T &t) -> std::weak_ptr< typename T::impl_type >
{
    return t.get_implementation();
}

template < typename K, typename Impl >
struct entity_cache
{
    using shared_impl = std::shared_ptr< Impl >;

    shared_impl
    add_or_skip();
};

}   // namespace entity
#endif   // CONNECTOR_ENTITY_UTIL_HPP
