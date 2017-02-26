#ifndef _GT_TEMPLATE_SWITCH_HPP_
#define _GT_TEMPLATE_SWITCH_HPP_

#include<utility>
#include<algorithm>

namespace gt
{

//-----------------------------
// UTILITY
//-----------------------------

template<class T>
struct Identity {
    using Type = T;
};

template<class...>
struct TypeHolder {};

template<class Pred,class T>
void launcher(Pred pred) { pred(Identity<T>{}); }

//-------------------------------------
// Switch index sequence 0...N in O(1)
//-------------------------------------

template<class...>
struct SwitchIndex;

// specialization for 1 element (trivial)
template<class T1>
struct SwitchIndex<T1>
{
    template<class Pred>
    void operator()(size_t, Pred pred) const { pred(Identity<T1>{}); }
};

// specialization for 2 elements to avoid jump table...
template<class T1,class T2>
struct SwitchIndex<T1,T2>
{
    template<class Pred>
    void operator()(size_t m, Pred pred) const {
        if (m == 0) pred(Identity<T1>{});
        else pred(Identity<T2>{});
    }
};

// switch in O(1) via jump table
template<class... Types>
struct SwitchIndex
{
    template<class Pred>
    void operator()(size_t m, Pred pred) const
    {
        static constexpr auto jmp = { launcher<Pred,Types>... };
        jmp.begin()[m](pred);
    }
};

//---------------------------------------
// Switch integer sequence in O(logN)
//---------------------------------------

template<class,class...>
struct SwitchSequence;

// Switch in O(logN) via binary search (maybe specialize for < 3 elements)
template<int... Is,class... Types>
struct SwitchSequence<std::integer_sequence<int,Is...>,Types...>
{
    template<class Pred>
    bool operator()(int m, Pred pred) const
    {
        // binary search in index array
        static constexpr auto idx = { Is... };
        auto it = std::lower_bound(idx.begin(), idx.end(), m);
        if (it == idx.end() || (*it != m)) return false;
        const auto i = std::distance(idx.begin(), it);
        SwitchIndex<Types...>()(i, pred);
        return true;
    }
};

template<int I>
constexpr bool isIncreasing() { return true; }

template<int I1, int I2, int... Is>
constexpr bool isIncreasing() { return I1 < I2 && isIncreasing<I2,Is...>(); }

//----------------------------------------------
// Switch integer sequence in O(N) recursively
//----------------------------------------------

template<class,class>
struct SwitchSequenceLinear;

template<class Z>
struct SwitchSequenceLinear<std::integer_sequence<Z>,TypeHolder<>>
{
    template<class Pred>
    bool operator()(Z, Pred) const { return false; }
};

// the "naive" recursive O(N) method (well, might be faster for very small N)
template<class Z,int I1,int... Is,class T1,class... Types>
struct SwitchSequenceLinear<std::integer_sequence<Z,I1,Is...>,TypeHolder<T1,Types...>>
{
public:
    template<class Pred>
    bool operator()(Z m, Pred pred) const
    {
        if (m == I1) {
            pred(Identity<T1>{});
            return true;
        }
        return SwitchSequenceLinear<std::integer_sequence<Z,Is...>,TypeHolder<Types...>>()(m, pred);
    }
};

//-----------------------------
// USER INTERFACE FUNCTIONS
//-----------------------------

// O(1)
template<class... Types,class Pred>
bool switchIndex(size_t m, Pred pred)
{
    if (m < sizeof...(Types)) {
        SwitchIndex<Types...>()(m, pred);
        return true;
    }
    return false;
}

// O(N)
template<class... Types,class Pred>
bool switchIndexLinear(size_t m, Pred pred)
{
    return SwitchSequenceLinear<std::index_sequence_for<Types...>,TypeHolder<Types...>>()(m, pred);
}

// O(logN)
template<class... Types, int... Is, class Pred>
bool switchSequence(int m, std::integer_sequence<int, Is...>, Pred pred)
{
    static_assert( sizeof...(Is) == sizeof...(Types), "length of integer sequence should match number of types" );
    static_assert( isIncreasing<Is...>(), "integer sequence should be monotonically increasing" );
    return SwitchSequence<std::integer_sequence<int, Is...>,Types...>()(m, pred);
}

// O(N)
template<class... Types, int... Is, class Pred>
bool switchSequenceLinear(int m, std::integer_sequence<int, Is...>, Pred pred)
{
    static_assert( sizeof...(Is) == sizeof...(Types), "length of integer sequence should match number of types" );
    static_assert( isIncreasing<Is...>(), "integer sequence should be monotonically increasing" );
    return SwitchSequenceLinear<std::integer_sequence<int, Is...>,TypeHolder<Types...>>()(m, pred);
}

} // gt

#endif
