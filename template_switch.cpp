#include "template_switch.hpp"

#include<type_traits>
#include<utility>
#include<iostream>

using namespace std;

template<int N>
using Int = integral_constant<int,N>;

int main(int argc, char **argv)
{
    int m = argc == 1 ? 25 : 33;

    auto process = [](auto t) {
        using Type = typename decltype(t)::Type;
        cerr << "val = " << Type::value << endl;
    };

    int k = m/8;
    cerr << "k = " << k << " m = " << m << endl;

    auto res = gt::switchIndex<Int<1>, Int<2>, Int<3>, Int<4>, Int<5>>(m/8, process );
    cerr << "switchIndex = " << res << endl;

    res = gt::switchIndexLinear<Int<1>, Int<2>, Int<3>, Int<4>, Int<5>>(m/8, process );
    cerr << "switchIndexLinear = " << res << endl;

    res = gt::switchSequence<Int<1>, Int<2>, Int<3>, Int<4>, Int<5>>(m,
                                                                    std::integer_sequence<int, 11,22,25,33,55>{},
                                                                    process );
    cerr << "switchSequence = " << res << endl;

    res = gt::switchSequenceLinear<Int<1>, Int<2>, Int<3>, Int<4>, Int<5>>(m,
                                                                    std::integer_sequence<int, 11,22,25,33,55>{},
                                                                    process );
    cerr << "switchSequenceLinear = " << res << endl;
}
