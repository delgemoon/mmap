//
// Created by tammd on 8/26/22.
//

#ifndef CONNECTOR_COMPARE_HPP
#define CONNECTOR_COMPARE_HPP

#include <compare>
#include <string>

namespace std
{
template < typename T, typename Traits, typename Alloc >
constexpr inline std::strong_ordering
operator<=>(const std::basic_string< T, Traits, Alloc > &lhs, const std::basic_string< T, Traits, Alloc > &rhs)
{
    if (const auto res = lhs.compare(rhs); res == 0)
        return std::strong_ordering::equal;
    else if (res < 0)
        return std::strong_ordering::less;
    else
        return std::strong_ordering::greater;
}

}   // namespace std

#endif   // CONNECTOR_COMPARE_HPP
