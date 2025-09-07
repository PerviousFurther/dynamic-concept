# What is this library?
`dynamic-concept` is an **single header library** consist by serval method 
implmenting fast polymorphism  
that ready to take the place of traditional virtual inheritance polymorphism.

# What is the problem of traditional polymorphism?
because the problem of counterintuitive +8/+4 byte and OOP, 
virtual function have been criticized for many years.

```cpp

struct IMeow { virtual void Meow(int time) = 0; };
struct IWoof { virtual void Woof(int time) = 0; };

struct Doggy : IMeow, IWoof 
{ 
    void Meow() { /*...*/ }
    void Bark() { /*...*/ }
};

static_assert(sizeof(Doggy) == sizeof(void*)*2) // never fire!
```

also the pointer casting will change the pointer's value.
the following part will always resulting true!

```cpp
(uintptr_t)(IWoof*)(doggy) != (uintptr_t)(IMeow*)(doggy)
```
NOTE: *Assume 'Doggy doggy' has been initialized.*

AND
the interface can only be called by **pointer** or **reference**, like...
```cpp
void Pat(IWoof *ptr);
```
or 
```cpp
void Pat(IMeow &ref);
```
the ownership is not clear.

# Using Simple Version of `dynamic-concept`
To replace code mentioned by our library.

Use the code as followed.

```cpp

// for library.
#include "dyn_cc.hpp"

struct Meowable
{
    // noexcept is optional, these can notice who wanna implment the interface.
    // btw, '&' and 'const' is also allowed.
    // ----------------------\/ 
    DYN_FN(0 NAMED Meow WITH noexcept RETURN void TAKE (time NAMED int));
    DYN_CC(Meowable EXTENDS());
};
struct Woofable
{
    DYN_FN(0 NAMED Woof WITH RETURN void TAKE (time NAMED int));
    DYN_CC(Woofable EXTENDS());
};

struct Doggy
{
    void Woof() { /*...*/ }
    void Meow() { /*...*/ }
};
```

To define procedure.
select one of `dyn::view`, `dyn::box`, `dyn::generic` to mark ownership.

```cpp
void PatByView(dyn::view<Woofable> view) { view.Woof(); }
void PatByMonopoly(dyn::box<Meowable> meow) { meow.Meow(); }

Doggy doggy;
PatByView(doggy); // call view verison.

PatByMonopoly({std::in_place_type<Doggy>}); // call monopoly version.
```
Or, require `Meow` and `Woof` same time:

```cpp
// ... Meowable and Woofable ...

struct CatDogLike
{
    DYN_TO((Meowable,0) NAMED Meow WITH);
    DYN_TO((Woofable,0) NAMED Woof WITH);
    DYN_CC(CatDogLike, EXTENDS(Meowable, Woofable));
};

```
However, there is no inheitance problem, since no inheritance from 
`Meowable` or `Woofable` was made.

*NOTE: We are too lazy to make some conversion...
thus `dyn::view<CatDogLike>` cannot convert to 
`dyn::view<Meow>` currently.*

More example, see [`example_with_macro.cpp`](example_with_macro.cpp)

# Using basic part of `dynamic-concept`
If you doesn't like the MACRO version and want to define your own
pattern and use it. 
You will only need [`dynamic_concept.hpp`](dynamic_concept.hpp).

And more you can do was shown in [`example.cpp`](example.cpp).

Online running is on [godbolt](https://godbolt.org/z/nhdMEbo1q).

## For people who wanna know `dynamic-concept` Basic Concept

- `dyn::require<...>`: 
define function signature and prototype that implmentation should impl.

- `dyn::view`, `dyn::box`, `dyn::generic`:
These are implmentation ownership manager.

  *NOTE: `dyn::generic` needs some kind of smart pointer   
  or other 'on stack' object for non-intrusive lifetime management.*

- `prototype`:
example: `void()`, `void()&&`, `void(<Invoker>::*)()` or `struct` with 
`prototype` aliasing these function like type. 

- *`Invoker`*:
CPO, or other `struct` with `operator()(Impl &&impl, ...)`. 
For transfer invocation into *concrete named* function or other operation (eg.`operator<<`).

- *`MetaGenerator`*:
Template with one template parameter for receive concrete implmentation type.
Also, it **MUST** allow `MetaGenerator<void>` and *`layout_compatible`* with any 
other specialization.   

## What is the difference with `Microsoft\proxy`?
We have different interface with it, but our library is more bottom part. 
That means we support more customization and flexible operation.
Anyway, you can define some macro and implment *proxy-like interface*
by yourself.

See the comparation at [`compare_with_proxy.cpp`](compare_with_proxy.cpp), 
and run in [**godbolt**](https://godbolt.org/z/GjdeGbPso).

## Would it be slow?
No, it even faster than `proxy` since invocation won't need to  
dereference `ownership manager`.

Sometime the compilers might apply devirtualization. 
So it looks faster than dynamic concept.
See [online test](https://godbolt.org/z/Mcd9c8848)

*NOTE: since some function wasn't inlined without optimization, 
it may preform slower than virtual function.
However, dynamic concept always faster than 
virtual function in MSVC.*

## How does it implmented?
Manually making vtable. 

in `.hpp` you would see a type named `virtual_bundle` which 
contain the key part of this library.
Other type just for making beautiful invocation or
collecting more meta infomation.

Yep, key part didn't over 50 lines. 
But tempolate operation ballooned the code into 400 lines.

## We welcome you to open an issue or contribute code.
There still some limitation that:
- We cannot directly inherit other's interface's functionality.
- Embed ownership manager doesn't contain all the method currently online.

We are forward to implment them in future.

