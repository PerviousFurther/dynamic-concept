#include "dynamic_concept.hpp"


// traiditional interface.

struct iyell { virtual void yell() = 0; };
struct ibark { virtual void bark() = 0; };
#pragma region ...
struct ianimal : iyell, ibark {};
#include <iostream>

struct dog : ianimal
{
    void yell() { std::cout << "."; }
    void bark() { std::cout << "!"; }
};
#pragma endregion
struct cat : iyell, ibark
{
    void yell() { std::cout << "."; }
    void bark() { std::cout << "!"; }
}; // 64 bit platform.

// We still need some ways to decrease code to do with it.
// I hope these could be an grammar if it can be use as standard.

#define MAKE_VOID_INVOKE(name, call)\
struct name { using prototype = void(); void operator()(auto &impl) { impl.call(); } }
struct animal 
{
    MAKE_VOID_INVOKE(bark, bark);
    MAKE_VOID_INVOKE(yell, yell);

	using dynamic = dyn::require<bark, yell>;
	void bark() { dynamic::invoke<0>(this); }
	void yell() { dynamic::invoke<1>(this); }
};

#include <chrono>

using clk = std::chrono::high_resolution_clock;

void test_0(dyn::view<animal> dv, ianimal &pv)
{
    auto missing = clk::now() - clk::now();
    auto begin = clk::now();
    auto end = clk::now();

    // test proxy invocation.
    begin = clk::now();
    for (auto i{100}; i--;) 
        pv.bark(), pv.yell();
    end = clk::now();
    ::std::cout 
        << "\nvirtual function spend:"
        << (end - begin - missing)
        << ::std::endl;

    // test my invocation.
    begin = clk::now();
    for (auto i{100}; i--;)
        dv.bark(), dv.yell();
    end = clk::now();
    ::std::cout 
        << "\ndynamic concept SPEND:"
        << (end - begin - missing)
        << ::std::endl;
}

int main(void)
{
    dog w;
    ::std::cout << clk::now() - clk::now() << ::std::endl;
    for (auto i{10}; i--;)
        test_0(w, w); 
}
