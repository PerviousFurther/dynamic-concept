# What is this library?
`dynamic concept` is an **single header library** consist by serval method 
implmenting fast polymorphism  
that ready to take the place of traditional virtual inheritance polymorphism.

# Why can it replace traditional polymorphism?
because the problem of counterintuitive +8/+4 byte and OOP, 
virtual function have been criticized for many years.

```cpp

struct interface_0 { virtual foo0(); };
struct interface_1 { virtual foo1(); };

struct impl : interface_0, interface_1 {};

static_assert(sizeof(impl) == 16) // in x64 never fire!

// AND
// the interface can only be called by pointer or rerference, like...
void plug(interface_0 *ptr);
// or 
void plug(interface_0 &ref);
// the ownership is not clear.

```
This library offer a better method to implement polymorphism,
that humanized interface composition and clear ownership, 
without inheritance. 
```cpp

struct programable 
{
  using dynamic = /* See `example.cpp` for more details. */;
  void program() { dynamic::invoke<0>(this); /* '0' is depended on `dynamic` */ }
};

struct joe // define the implmentation.
{ 
  void program() { ::std::cout << "Hello world!"; }
}
// box have same ownership like unique_ptr
void do_program(dyn::box<programmer> who) 
{
  who.program();
}

do_program({std::in_place_type<joe>}); // Output: Hello world!

```
Also we can implment this.
```cpp

struct programable 
{
  using dynamic = /* See `example.cpp` for more details. */;
};

struct sleepable
{
  using dynamic = /* See `example.cpp` for more details. */;
};

struct programmer
{
  using dynamic = dyn::require<programable, sleepable>; // like multiple inheritance.

  void program() {/* See `example.cpp` for more details. */}
  void sleep() {/* See `example.cpp` for more details. */}
};

struct mary 
{
  void pragram() { ::std::cout << "Hello world!"; }
  void sleep() { ::std::cout << "Bye bye world!"; }
};

void past_a_day(dyn::box<programmer> p)
{ 
  p.program(); 
  p.sleep(); 
}

past_a_day({::std::in_place_type<mary>}); // Output: Hello world! Bye bye world!
```

# Concept

- `dyn::require<...>`: 
define function signature and prototype that implmentation should impl.

- `dyn::view`, `dyn::box`, `dyn::generic_`:
These are implmentation ownership manager.

- *`Signature`*:
example: `void()`, `void(<Invoker>::*)()` or `struct` with `type` aliasing `void()` like type. 

- *`Invoker`*:
CPO, or other `struct` with `operator()(Impl &&impl, ...)`. 
For transfer invocation into *concrete named* function or other operation.

# Usage
First, copy [`dynamic_concept.hpp`](dynamic_concept.hpp) or git as you like.

Then, `#include "dynamic_concept.hpp"`.

And more you can do was shown in [`example.cpp`](example.cpp).

Online running is on [godbolt](https://godbolt.org/z/hKP79TrsE).   

## What is the difference with `Microsoft\proxy`?
We have different interface with it, but our library is more bottom part. That means we support more customization and flexible operation.
Anyway, you can define some macro and implment an proxy.

See the comparation at [**here**](https://godbolt.org/z/bxdPbv7or).

## Would it be slow?
No, sometime the compilers might devirtualized the virtual function. 
Then it looks faster than dynamic concept, 
but not all the virtual function can be so lucky for being devirtualized by compilers.

## How does it implmented?
Manually making vtable. 

in `.hpp` you would see a type named `virtual_bundle` which 
contain the key part of this library.
Other type just for making beautiful invocation or collecting more meta infomation.

Yep, key part didn't over 100 lines. 

## We welcome you to open an issue or contribute code.
There still some limitation that:
- We cannot directly inherit other's interface's functionality.
- Embed ownership manager doesn't contain all the method currently online.

We are forward to implment them in future.

