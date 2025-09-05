#include <iostream>
#include <chrono>
#include "dynamic_concept.hpp"
#include "proxy/proxy.h"


// Proxy like.

PRO_DEF_MEM_DISPATCH(pro_mem_draw, draw); // MACRO here.
struct pro_drawable : pro::facade_builder
    ::add_convention<pro_mem_draw, void()>
    ::build {};


// My interface like.

struct dyn_mem_draw
{
    using prototype = void();
    template<typename T>
    void operator()(T &impl) { impl.draw(); }
};
struct dyn_drawable
{
    using dynamic = dyn::require<dyn_mem_draw>;
    void draw() { dynamic::invoke<0>(this); }
};


// Implmentation.

struct rectangle
{
    void draw() { std::cout << "-"; }
};


using clk = std::chrono::high_resolution_clock;

void test_0()
{
    auto missing = clk::now() - clk::now();
    rectangle r;
    dyn::view<dyn_drawable> dv(r);
    auto pv = pro::make_proxy_view<pro_drawable>(r);

    auto begin = clk::now();
    auto end = clk::now();

    // test proxy invocation.
    begin = clk::now();
    for (auto i{100}; i--;)
        pv->draw();
    end = clk::now();
    ::std::cout 
        << "\nmicrosoft proxy spend:"
        << (end - begin - missing)
        << ::std::endl;

    // test my invocation.
    begin = clk::now();
    for (auto i{100}; i--;)
        dv.draw();
    end = clk::now();
    ::std::cout 
        << "\ndynamic concept SPEND:"
        << (end - begin - missing)
        << ::std::endl;
}

int main(void)
{
    test_0();
    ::std::cout 
        << "[[Ignore upper one.]]\n"
        "Chrono might initialize something \n"
        "and without exclude it...\n";
    for (auto i{10}; i--;)
        test_0(); 
}
