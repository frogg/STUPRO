#ifndef STUPRO_ARITHMETICFUNCTORS_HPP
#define STUPRO_ARITHMETICFUNCTORS_HPP

template <typename T, typename T2>
struct Add
{
    T operator()(T a, T2 b) const
    {
        return a + b;
    }
};
template <typename T, typename T2>
struct Sub
{
    T operator()(T a, T2 b) const
    {
        return a - b;
    }
};
template <typename T, typename T2>
struct Mul
{
    T operator()(T a, T2 b) const
    {
        return a * b;
    }
};
template <typename T, typename T2>
struct Div
{
    T operator()(T a, T2 b) const
    {
        return a / b;
    }
};

#endif