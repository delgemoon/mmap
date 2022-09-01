//
// Created by tammd on 8/31/22.
//

#include <bit>
#include <bitset>
#include <iostream>

int
main()
{
    std::int64_t      ll = 0xA2345678ABCD23EF;
    std::bitset< 64 > b(ll);
    std::cout << "i          = " << std::bitset<64>( (b ) << '\n';
    std::cout << "rotr(i,0)  = " << std::bitset<64>() << '\n';
    std::cout << "i          = " << std::bitset<64>(b) << '\n';
    std::cout << "rotr(i,1)  = " << std::bitset<64>(b >>= 8) << '\n';
    std::cout << "rotr(i,9)  = " << std::bitset<64>(b <<= 24) << '\n';
    std::cout << "rotr(i,-1) = " << std::bitset<64>(b <<= 32) << '\n';
}