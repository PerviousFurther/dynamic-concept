//----------------------------------------====\\
// Dynamic-concept Header File. Below MIT lincese.
// 
// Copyright (c) 2025 MaoQwe (PerviousFurther) /
//----------------------------------------====//

#if !defined(DYNAMIC_CONCEPT_HPP)
#define DYNAMIC_CONCEPT_HPP

// For module.
#if !defined(DYNAMIC_CONCEPT_NO_STD)
#	include <utility>
#	include <concepts>
#endif

// `#define DYNAMIC_CONCEPT_EXPORT_AS_MODULE export` 
// to embed dynamic concept into your module.
#if !defined(DYNAMIC_CONCEPT_EXPORT)
#	define DYNAMIC_CONCEPT_EXPORT_AS_MODULE
#endif

// maybe no use.
#if defined(_MSC_VER) || defined(__INTEL_COMPILER) || defined(__CUDACC__)
#	define DYNAMIC_CONCEPT_FORCEINLINE __forceinline
#elif defined(__CLANG__) || defined(__GNUC__)
#	define DYNAMIC_CONCEPT_FORCEINLINE __attribute__((always_inline))
#else
#	define DYNAMIC_CONCEPT_FORCEINLINE inline
#endif

DYNAMIC_CONCEPT_EXPORT_AS_MODULE
namespace dyn
{

struct self;

namespace impl
{
#pragma region META OPERATION, REALLY HAPPY TO MEET CPP BECAUSE OF THESE...THANK YOU SO MUCH STANDARD LIBRARY.

template<typename...>
struct type_list;

template<::std::size_t index, typename T__>
struct type_at {};
template<::std::size_t index, template<typename...>typename Tp, typename From, typename...Ts>
struct type_at<index, Tp<From, Ts...>> : type_at<index - 1, Tp<Ts...>> {};
template<template<typename...>typename Tp, typename From, typename...Ts>
struct type_at<0, Tp<From, Ts...>> : ::std::type_identity<From> {};
template<::std::size_t index, typename T>
using type_at_t = typename type_at<index, T>::type;

template<typename T, ::std::size_t index, typename From, typename...Ts>
inline consteval auto index_of() noexcept 
{
    if constexpr (::std::same_as<From, ::std::remove_cvref_t<T>>) return index;
    else if constexpr (sizeof...(Ts)) return impl::index_of<T, index + 1, Ts...>();
}

// THANK YOU SO MUCH CPP. 
template<typename T, typename L>
struct qual_like { using type = T; };
template<typename T, typename L>
struct qual_like<T, L&> { using type = T&; };
template<typename T, typename L>
struct qual_like<T, L&&> { using type = T&&; };
template<typename T, typename L>
struct qual_like<T, L const&> { using type = T const&; };
template<typename T, typename L>
struct qual_like<T, L const&&> { using type = T const&&; };
template<typename T, typename L>
struct qual_like<T, L volatile&&> { using type = T volatile&&; };
template<typename T, typename L>
struct qual_like<T, L volatile&> { using type = T volatile&; };
template<typename T, typename L>
struct qual_like<T, L const volatile&> { using type = T const volatile&; };
template<typename T, typename L>
struct qual_like<T, L const volatile&&> { using type = T const volatile&&; };
template<typename T, typename L>
using qual_like_t = typename qual_like<::std::remove_cvref_t<T>, L>::type;

template<typename T>
concept self_duck = ::std::same_as<::std::remove_cvref_t<T>, self>;

template<typename T>
struct patch {};

template<typename T>
struct proto_traits : ::std::false_type {};

template<typename...>
struct proto_transform;
template<typename F
    , template<typename...>typename Tp, typename...Ts, typename...Rs
    , typename Invoker, typename Rt>
struct proto_transform<Tp<Rs...>, Tp<F, Ts...>, Invoker, Rt> 
    : proto_transform<Tp<Rs..., F>, Tp<Ts...>, Invoker, Rt>
{};
template< self_duck Self
    , template<typename...>typename Tp
    , typename...Ts, typename...Rs
    , typename Invoker, typename Rt>
struct proto_transform<Tp<Rs...>, Tp<Self, Ts...>, Invoker, Rt> : ::std::true_type
{
    using return_type = Rt;
    using invoker = Invoker;

    using type = Rt(Rs..., Ts...);
    using ptype = Rt(*)(void*, Rs..., Ts...);

    template<typename Impl>
    static constexpr Rt transfer(void *ptr, Rs...tops, Ts...args)
    { if constexpr (::std::invocable<Invoker, Rs..., qual_like_t<Impl, Self>, Ts...>) 
        return Invoker()(static_cast<Rs>(tops)...,
            static_cast<qual_like_t<Impl, Self>>(*static_cast<Impl*>(ptr)), 
            static_cast<Ts>(args)...);
      else 
        return Invoker()(patch<Impl>(),
            static_cast<Rs>(tops)...,
            static_cast<qual_like_t<Impl, Self>>(*static_cast<Impl*>(ptr)), 
            static_cast<Ts>(args)...); }
};

template<typename T, typename Invoker, self_duck Self, typename...Ts>
struct proto_traits<T(Invoker::*)(Self, Ts...)> : proto_transform<type_list<>, type_list<Self, Ts...>, Invoker, T> {};

template<typename T, typename Invoker, typename...Ts>
    requires((self_duck<Ts>)||...)
struct proto_traits<T(Invoker::*)(Ts...)> : proto_transform<type_list<>, type_list<Ts...>, Invoker, T> {};

struct functor_convension // this is default convension.
{
    template<typename Impl, typename...Args>
    constexpr decltype(auto) operator()(Impl &&ref, Args&&...args)
        noexcept(::std::is_nothrow_invocable_v<Impl&, Args&&...>)
    { return static_cast<Impl&&>(ref)(static_cast<Args&&>(args)...); }
};
template<typename T, self_duck Self, typename...Ts>
struct proto_traits<T(Self, Ts...)> : proto_traits<T(functor_convension::*)(Self, Ts...)> {};

template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...)> : proto_traits<T(functor_convension::*)(self&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...)&&> : proto_traits<T(functor_convension::*)(self&&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...)&> : proto_traits<T(functor_convension::*)(self&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const &> : proto_traits<T(functor_convension::*)(self const&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) volatile &> : proto_traits<T(functor_convension::*)(self volatile &, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const volatile &> : proto_traits<T(functor_convension::*)(self const volatile &, Ts...)> {};

template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const &&> : proto_traits<T(functor_convension::*)(self const&&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) volatile &&> : proto_traits<T(functor_convension::*)(self volatile &&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const volatile &&> : proto_traits<T(functor_convension::*)(self const volatile &&, Ts...)> {};

template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const> : proto_traits<T(functor_convension::*)(self const&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) volatile> : proto_traits<T(functor_convension::*)(self volatile&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const volatile> : proto_traits<T(functor_convension::*)(self const volatile&, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
    requires((!self_duck<Ts>) && ...)
struct proto_traits<T(Invoker::*)(Ts...)> : proto_traits<T(Invoker::*)(self&, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...)&&> : proto_traits<T(Invoker::*)(self&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...)&> : proto_traits<T(Invoker::*)(self&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const &> : proto_traits<T(Invoker::*)(self const&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) volatile &> : proto_traits<T(Invoker::*)(self volatile &, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const volatile &> : proto_traits<T(Invoker::*)(self const volatile &, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const &&> : proto_traits<T(Invoker::*)(self const&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) volatile &&> : proto_traits<T(Invoker::*)(self volatile &&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const volatile &&> : proto_traits<T(Invoker::*)(self const volatile &&, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const> : proto_traits<T(Invoker::*)(self const&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) volatile> : proto_traits<T(Invoker::*)(self volatile&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const volatile> : proto_traits<T(Invoker::*)(self const volatile&, Ts...)> {};

// For noexcept!!!
// We ignore noexcept here, however.

template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) noexcept> : proto_traits<T(self&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...)&& noexcept> : proto_traits<T(functor_convension::*)(self&&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...)& noexcept> : proto_traits<T(functor_convension::*)(self&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const & noexcept> : proto_traits<T(functor_convension::*)(self const&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) volatile & noexcept> : proto_traits<T(functor_convension::*)(self volatile &, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const volatile & noexcept> : proto_traits<T(functor_convension::*)(self const volatile &, Ts...)> {};

template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const && noexcept> : proto_traits<T(functor_convension::*)(self const&&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) volatile && noexcept> : proto_traits<T(functor_convension::*)(self volatile &&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const volatile && noexcept> : proto_traits<T(functor_convension::*)(self const volatile &&, Ts...)> {};

template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const noexcept> : proto_traits<T(functor_convension::*)(self const&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) volatile noexcept> : proto_traits<T(functor_convension::*)(self volatile&, Ts...)> {};
template<typename T, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Ts...) const volatile noexcept> : proto_traits<T(functor_convension::*)(self const volatile&, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) noexcept> : proto_traits<T(Invoker::*)(self&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) volatile noexcept> : proto_traits<T(Invoker::*)(self&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const volatile noexcept> : proto_traits<T(Invoker::*)(self&&, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...)&& noexcept> : proto_traits<T(Invoker::*)(self&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...)& noexcept> : proto_traits<T(Invoker::*)(self&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const & noexcept> : proto_traits<T(Invoker::*)(self const&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) volatile & noexcept> : proto_traits<T(Invoker::*)(self volatile &, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const volatile & noexcept> : proto_traits<T(Invoker::*)(self const volatile &, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const && noexcept> : proto_traits<T(Invoker::*)(self const&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) volatile && noexcept> : proto_traits<T(Invoker::*)(self volatile &&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((!self_duck<Ts>)&&...) struct proto_traits<T(Invoker::*)(Ts...) const volatile && noexcept> : proto_traits<T(Invoker::*)(self const volatile &&, Ts...)> {};

// for non-member functions.

template<typename T, typename...Ts>
requires((self_duck<Ts>)||...) struct proto_traits<T(Ts...)> : proto_traits<T(functor_convension::*)(Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
requires((self_duck<Ts>)||...) struct proto_traits<T(Invoker::*)(Ts...) noexcept> : proto_traits<T(Invoker::*)(Ts...)> {};
template<typename T, typename...Ts>
requires((self_duck<Ts>)||...) struct proto_traits<T(Ts...) noexcept> : proto_traits<T(Ts...)> {};


template<typename T>
concept proto = proto_traits<T>::value;

template<typename T, typename Fn>
struct set_to_mem : set_to_mem<T, typename proto_traits<Fn>::type>{};
template<typename T, typename Rt, typename...Args>
struct set_to_mem<T, Rt(Args...)> { using type = Rt(T::*)(Args...); };

template<typename T>
    requires(::std::is_function_v<typename T::proto>)
struct proto_traits<T> 
    : proto_traits<typename set_to_mem<T, typename T::proto>::type> {};

template<typename T, typename...Ts> // might work on template specialization, but currently how to impl it remain unkown.
struct proto_traits<T(*)(Ts...)> : proto_traits<T(Ts...)>  
{ 
    static_assert(proto<T>?0:0, "Function pointer as proto is reserved.");
};

#pragma endregion

template<typename...>
struct covariant : ::std::false_type {};
template<typename From, typename To>
    requires(requires { typename From::dynamic; } && requires{ typename To::dynamic; })
struct covariant<From, To> 
    : covariant<typename From::dynamic::sign, typename To::dynamic::sign> 
{ using this_type = From;
  using super_type = To; };
template< template<typename...>typename TP
        , typename...Ts, typename...Rs>
struct covariant<TP<Ts...>, TP<Rs...>> : covariant<TP<Ts...>, TP<Rs...>, TP<>, TP<>> {};

// completely same.
template<template<typename...>typename TP, typename...Ts>
struct covariant<TP<Ts...>, TP<Ts...>> : ::std::true_type 
{ static constexpr void const *as(void const *ptr) noexcept { return ptr; } };

// recursion process...
template< template<typename...>typename TP
    , typename L, typename...TTs
    , typename To, typename...RRs
    , typename...Ss, typename...RSs>
struct covariant<TP<L, TTs...>, TP<To, RRs...>, TP<Ss...>, TP<RSs...>>
    : ::std::conditional_t<::std::same_as<L, To> 
        // if the top interface is same, then to next.
        , covariant<TP<TTs...>, TP<RRs...>, TP<Ss..., L>, TP<RSs..., To>>
        // if the top interface is not same, then needs to rematch.
        , covariant<TP<TTs...>, TP<RSs..., To, RRs...>, TP<Ss..., L>, TP<>>
        > {};

// Final situation 1: all matched.
template< template<typename...>typename TP
    , typename...Rs
    , typename...Ss, typename...RSs>
struct covariant<TP<Rs...>, TP<>, TP<Ss...>, TP<RSs...>>
    : ::std::true_type
{
    static constexpr DYNAMIC_CONCEPT_FORCEINLINE 
        void const *as(void const *ptr) noexcept
    { return reinterpret_cast<void(*const*)()>(ptr) + sizeof...(Rs); }
};
// Or in back.
template< template<typename...>typename TP
        , typename...Ss, typename...RSs>
struct covariant<TP<>, TP<>, TP<Ss...>, TP<RSs...>>
    : ::std::true_type
{
    static constexpr DYNAMIC_CONCEPT_FORCEINLINE 
        void const *as(void const *ptr) noexcept
    { return ptr; }
};

// Final situation 2: not all matched.
template< template<typename...>typename TP
    , typename...Rs
    , typename...Ss, typename...RSs>
struct covariant<TP<>, TP<Rs...>, TP<Ss...>, TP<RSs...>>
    : ::std::false_type
{};

template<typename From, typename To>
concept covariant_to = covariant<From, To>::value;
template<typename From, typename To>
concept covariant_from = covariant<To, From>::value;



// virtual table


template<typename Impl, typename...Ss>
struct make_table {};
template<typename Impl, proto T, proto...Rs>
struct make_table<Impl, T, Rs...> : make_table<Impl, Rs...> 
{
    using base = make_table<Impl, Rs...>;
    using trait = proto_traits<T>;
    using type = typename trait::ptype; // GCC doesnt know what is `<template_object><T> fn = <type>::template transfer<...>`.
    type stub_ = &trait::template transfer<Impl>;
};
template<typename Impl, template<typename>typename MG, proto...Ss>
struct make_meta : make_table<Impl, Ss...> 
{
    using type = MG<Impl>;
    using base = make_table<Impl, Ss...>;
    MG<Impl> stub_;
};

// @tparam MetaGenerator: Might use in static reflection or something.
// @tparam Base: User defined interface.
template<typename This, template<typename>typename MG, proto...Signs>
struct virtual_bundle : public This
{
    template<typename, template<typename>typename, proto...>
    friend struct virtual_bundle;
    template<template<typename>typename, proto...>
    friend struct concept_descriptor;

protected:
    using sign = type_list<Signs...>;

public:
    template<typename Impl>
    using typeinfos_type = make_meta<::std::remove_cvref_t<Impl>, MG, Signs...>;

    template<typename Impl>
    static constexpr auto typeinfos_ = make_meta<::std::remove_cvref_t<Impl>, MG, Signs...>{};

protected:
    constexpr virtual_bundle() = default;

    template<typename T>
    constexpr virtual_bundle(T *impl)
    { this->reset(impl); }

    template<typename T>
    constexpr virtual_bundle(::std::in_place_type_t<T> stub, void *impl)
    { this->reset(stub, impl); }

    template<typename Other, proto...Os>
    constexpr virtual_bundle(virtual_bundle<Other, MG, Os...> const &other)
        : ptr_{covariant<type_list<Os...>, sign>::as(other.ptr_)}
        , this_{other.this_}
    {}

    constexpr virtual_bundle(virtual_bundle const&) noexcept = default;
    constexpr virtual_bundle &operator=(virtual_bundle const&) noexcept = default;

    constexpr virtual_bundle(virtual_bundle &&other) noexcept 
        : ptr_{::std::exchange(other.ptr_, nullptr)} 
        , this_{::std::exchange(other.this_, nullptr)}
    {}
    constexpr virtual_bundle& operator=(virtual_bundle &&other) noexcept 
    { ::std::swap(ptr_, other.ptr_); ::std::swap(this_, other.this_); return *this; }

private:
    template<::std::size_t index, typename T>
    static constexpr DYNAMIC_CONCEPT_FORCEINLINE auto &get(void const *ptr) noexcept 
    { const auto offset = (sizeof...(Signs) - (index + 1));
      return (*(reinterpret_cast<::std::add_const_t<T>*>(ptr) + offset)); }

protected:
    template<::std::size_t index, typename...Args>
    constexpr DYNAMIC_CONCEPT_FORCEINLINE decltype(auto) invoke(Args&&...args) const 
    { auto func = virtual_bundle::template get<index, typename proto_traits<type_at_t<index, sign>>::ptype>(ptr_); 
      return func(this_, static_cast<Args&&>(args)...); }

    template<typename T>
    constexpr void reset(T *impl) noexcept
    { this->reset(::std::in_place_type<T>, impl); }

    template<typename T>
    constexpr DYNAMIC_CONCEPT_FORCEINLINE void reset(::std::in_place_type_t<T>, void *impl) noexcept
    { ptr_ = &virtual_bundle::template typeinfos_<::std::remove_cvref_t<T>>;
      this_ = impl; }

    constexpr void reset() noexcept { ptr_ = nullptr; this_ = nullptr; }

    // reset `this_` only.
    constexpr void tweak(void *self = nullptr) noexcept 
    { this_ = self; }

    constexpr void *get_this() const noexcept { return this_; }

public:
    constexpr auto &info() const noexcept 
    { return *reinterpret_cast<MG<void> const*>
        (reinterpret_cast<void(*const*)()>(ptr_) + sizeof...(Signs)); }
   
private:
    void const *ptr_{nullptr};
    void *this_{nullptr};
};





template<typename From, typename To>
struct append_ { static_assert(proto<From>, "Cannot append non-prototype in dynamic concept."); };

// Avoid redundant.

template<template<typename...>typename TP, proto T, typename...Rs>
    requires((!::std::same_as<T, Rs>) && ...)
struct append_<T, TP<Rs...>> : ::std::type_identity<TP<Rs..., T>> {};
template<template<typename...>typename TP, proto T, typename...Rs>
    requires((::std::same_as<T, Rs>) || ...)
struct append_<T, TP<Rs...>> : ::std::type_identity<TP<Rs...>> {};

template<template<typename...>typename TP, typename...Rs>
struct append_<TP<>, TP<Rs...>> : ::std::type_identity<TP<Rs...>> {};
template<template<typename...>typename TP, proto T, typename...Fs, typename...Rs>
struct append_<TP<T, Fs...>, TP<Rs...>> : append_<TP<Fs...>, typename append_<T, TP<Rs...>>::type> {};

template< template<template<typename>typename, typename...>typename FT, template<typename...>typename RT
        , template<typename>typename MG, typename...T, typename...To>
struct append_<FT<MG, T...>, RT<To...>> : append_<RT<T...>, RT<To...>> {};
template<typename T, typename To>
    requires(requires{ typename T::dynamic; })
struct append_<T, To> : append_<typename T::dynamic, To> {}; // for interface.

template< template<template<typename>typename, typename...>typename CD
    , template<typename>typename MG
    , proto T, typename...Rs>
    requires((!::std::same_as<T, Rs>) && ...)
struct append_<T, CD<MG, Rs...>> : ::std::type_identity<CD<MG, Rs..., T>> {};
template< template<template<typename>typename, typename...>typename CD
    , template<typename>typename MG
    , proto T, typename...Rs>
    requires((::std::same_as<T, Rs>) || ...)
struct append_<T, CD<MG, Rs...>> : ::std::type_identity<CD<MG, Rs...>> {};


// DO NOT SPECIALIZATION THIS TEMPLATE.
template<template<typename>typename MG, proto...Ss>
struct concept_descriptor
{
    using sign = type_list<Ss...>;
    // FOR INNER USAGE, DO NOT USE IT.
    template<typename Interface>
    using attach = impl::virtual_bundle<Interface, MG, Ss...>;

    // @tparam MetaGenerator: MetaGenerator<void> should be layout compatible with MetaGenerator<<Any Concrete Type>>.
    template<template<typename>typename MG>
    using switch_meta = concept_descriptor<MG, Ss...>;

    template<typename T>
    using append = typename append_<T, concept_descriptor<MG, Ss...>>::type;

    // @tparam index: the functionality index in `dynamic`.
    // @param self: just pass `this` in it.
    template<::std::size_t index, typename Interface, typename...Args>
    static constexpr DYNAMIC_CONCEPT_FORCEINLINE decltype(auto) invoke(Interface *self, Args&&...args) 
    { return static_cast<attach<Interface>*>(self)->
        template invoke<index>(static_cast<Args&&>(args)...); }

    // @tparam Proto: type of functionality in `dynamic`.
    // @param self: just pass `this` in it.
    template<typename Proto, typename Interface, typename...Args>
    static constexpr DYNAMIC_CONCEPT_FORCEINLINE decltype(auto) invoke(Interface *self, Args&&...args)
        requires((::std::same_as<Proto, Ss>) || ...)
    { return static_cast<attach<Interface>*>(self)->
        template invoke<impl::index_of<Proto, 0, Ss...>()>(static_cast<Args&&>(args)...); }
};

// if you occur error with this part, mostly because some type doesn't satisfy `proto` or *`Interface`*.
template<typename T, typename C>
struct make_concept 
{ static_assert(proto<T>, "Some or one of the protos expressed wasn't satisfy concept of proto."); }; 
template<typename...Rs>
struct make_concept<type_list<>, type_list<Rs...>> 
    : ::std::type_identity<concept_descriptor<::std::type_identity, Rs...>> 
{};
template<typename From, typename...Fs, typename...Rs>
struct make_concept<type_list<From, Fs...>, type_list<Rs...>>
    : make_concept<type_list<Fs...>, typename append_<From, type_list<Rs...>>::type>
{};


#pragma region embedded ownership manager.


template<typename This>
class view : public This::dynamic::template attach<This>
{
    template<typename>
    friend class view;
    template<typename>
    friend class box;
    using base = typename This::dynamic::template attach<This>;
public:
    constexpr view() = default;
    constexpr view(view const&) = default;
    constexpr view(view &&) noexcept = default;
    constexpr view& operator=(view const&) = default;
    constexpr view& operator=(view &&) noexcept = default;

    template<typename T>
    constexpr view(T &impl) noexcept
        : base{&impl}
    {}

    template<covariant_to<This> O>
    constexpr view(view<O> &other) noexcept
        : base{static_cast<view<O>::base const&>(other)}
    {}
    template<covariant_to<This> O>
    constexpr view &operator=(view<O> &other) noexcept
    { static_cast<base&>(*this) = static_cast<view<O>::base const&>(other); }

    template<typename O>
    constexpr view(view<O>&) noexcept = delete;

    template<typename T>
    constexpr void emplace(T &impl) noexcept
    { base::reset(&impl); }

    constexpr void reset() noexcept 
    { base::reset(); }

    // constexpr auto info() const noexcept { return base::get_meta(); }

    operator bool() const noexcept { return base::get_this(); }

    template<covariant_from<This> To>
    constexpr view<typename covariant<This, To>::super_type> as() noexcept { return {*this}; }

private:
    template<typename T>
    constexpr view(::std::nullptr_t, T const &base_)
        : base{base_}
    {}
};

template<typename This>
class box : public This::dynamic::template attach<This>
{
    template<typename>
    friend class box;

    using base = typename This::dynamic::template attach<This>;
    template<typename T>
    static constexpr void do_delete(void *ptr) noexcept { delete static_cast<T*>(ptr); }
public:
    constexpr box() = default;

    constexpr box(box const&) = delete;
    constexpr box(box &&other) noexcept 
        : base{static_cast<box&&>(other)}
        , deleter_{::std::exchange(other.deleter_, nullptr)}
    {}

    constexpr box& operator=(box const&) = delete;
    constexpr box& operator=(box &&other) noexcept 
    { if(this != &other)
    { ::std::swap(static_cast<base&>(*this), static_cast<base&&>(other));
      ::std::swap(deleter_, other.deleter_); } };
    
    template<typename T, typename...Args>
    constexpr box(::std::in_place_type_t<T>, Args&&...args)
        requires(::std::constructible_from<T, Args&&...>)
    { this->template emplace<T>(static_cast<Args&&>(args)...); }

    template<covariant_to<This> O>
    constexpr box(box<O> &&other)
        : base{static_cast<typename box<O>::base&&>(other)}
        , deleter_{::std::exchange(other.deleter_, nullptr)}
    {}
    template<typename O>
    constexpr box(box<O>&&) = delete;

    template<covariant_to<This> O>
    constexpr box& operator=(box<O> &&other)
    { ::std::swap(static_cast<base&>(*this), static_cast<typename box<O>::base&&>(other));
      ::std::swap(deleter_, other.deleter_); }

    template<typename O>
    constexpr box& operator=(box<O>&&) = delete;

    ~box() { this->reset(); }

    template<typename T, typename...Args>
    constexpr void emplace(Args&&...args) noexcept
    { if(deleter_) deleter_(base::get_this());
      base::reset(new T{static_cast<Args&&>(args)...});
      deleter_ = &do_delete<T>; }

    constexpr void reset() noexcept 
    { if(deleter_) { ::std::exchange(deleter_, nullptr)(base::get_this()); base::reset(); } }

    // constexpr const auto &info() const noexcept { return base::get_meta(); }

    operator bool() const noexcept { return deleter_; }

    template<covariant_from<This> To>
    constexpr box<typename covariant<This, To>::super_type> as() && noexcept { return {static_cast<box&&>(*this)}; }
    
    template<covariant_from<This> To>
    constexpr view<typename covariant<This, To>::super_type> as() & noexcept { return {nullptr, static_cast<base&>(*this)}; }

private:
    void(*deleter_)(void*){nullptr};
};

template<typename T>
struct owner_traits { };
template<typename T>
    requires(requires{ typename T::reference_type; })
struct owner_traits<T> 
    : ::std::type_identity<::std::remove_cvref_t<typename T::reference_type>> {};
template<typename T>
    requires(requires{ typename T::pointer_type; })
struct owner_traits<T>
    : ::std::type_identity<::std::remove_cv_t<::std::remove_pointer_t<typename T::pointer_type>>> {};
template<typename T>
    requires(requires{ typename T::element_type; })
struct owner_traits<T>
    : ::std::type_identity<::std::remove_cv_t<::std::remove_pointer_t<typename T::element_type>>> {};

template<typename This, typename Ownership>
class generic : public This::dynamic::template attach<This>
{
    template<typename, typename>
    friend class generic;

    using base = typename This::dynamic::template attach<This>;
    using owner = Ownership;
public:
    constexpr generic() = default;

    template<covariant_to<This> Other, typename OtherOwnership>
    constexpr generic(generic<Other, OtherOwnership> const &other) 
        noexcept(::std::is_nothrow_constructible_v<owner, OtherOwnership const&>)
        requires(::std::constructible_from<owner, OtherOwnership const&>)
        : base{static_cast<typename generic<Other, Ownership>::base const&>(other)}
        , data_{other.data_}
    { base::tweak(data_.get()); }
    template<typename Other, typename OtherOwnership>
    constexpr generic(generic<Other, OtherOwnership> const&) = delete;

    template<covariant_to<This> Other, typename OtherOwnership>
    constexpr generic(generic<Other, OtherOwnership> &&other) 
        noexcept(::std::is_nothrow_move_constructible_v<owner>)
        requires(::std::constructible_from<owner, OtherOwnership&&>)
        : base{static_cast<typename generic<Other, Ownership>::base&&>(other)}
        , data_{static_cast<OtherOwnership&&>(other.data_)}
    { base::tweak(data_.get()); }
    template<typename Other, typename OtherOwnership>
    constexpr generic(generic<Other, OtherOwnership>&&) = delete;

    template<covariant_to<This> Other, typename OtherOwnership>
    constexpr generic& operator=(generic<Other, Ownership> const &other)
        noexcept(::std::is_nothrow_swappable_with_v<Ownership, owner>)
        requires(::std::swappable_with<OtherOwnership, owner>)
    { if (this != &other) 
    { data_ = other.data_;
      static_cast<base&>(*this) = static_cast<typename generic<Other, Ownership>::base const&>(other); 
      base::tweak(data_.get()); }
      return *this; }
    template<typename Other, typename OtherOwnership>
    constexpr generic& operator=(generic<Other, OtherOwnership> const&) = delete;

    template<covariant_to<This> Other, typename OtherOwnership>
    constexpr generic& operator=(generic<Other, OtherOwnership> &&other)
        noexcept(::std::is_nothrow_move_assignable_v<owner>)
        requires(::std::swappable_with<owner, OtherOwnership&&>)
    { if (this != &other) 
    { ::std::swap(data_, other.data_);
      ::std::swap(static_cast<base&>(*this), static_cast<typename generic<Other, Ownership>::base&&>(other)); 
      base::tweak(data_.get()); }
      return *this; }
    template<typename Other, typename OtherOwnership>
    constexpr generic& operator=(generic<Other, OtherOwnership>&&) = delete;

    constexpr generic(generic const &other)
        noexcept(::std::is_nothrow_copy_constructible_v<owner>)
        requires(::std::copy_constructible<owner>)
        : base{other}
        , data_{other.data_}
    { base::tweak(data_.get()); }

    constexpr generic(generic &&other) 
        noexcept(::std::is_nothrow_move_constructible_v<owner>)
        requires(::std::move_constructible<owner>)
        : base{static_cast<base&&>(other)}
        , data_{::std::exchange(other.data_, {})}
    { base::tweak(data_.get()); };

    constexpr generic& operator=(generic const &other)
        noexcept(::std::is_nothrow_copy_assignable_v<owner>)
        requires(::std::is_copy_assignable_v<owner>)
    { if (this != &other) 
    { data_ = other.data_;
      static_cast<base&>(*this) = static_cast<base const&>(other);
      base::tweak(data_.get()); }
      return *this; }

    constexpr generic& operator=(generic &&other) 
        noexcept(::std::is_nothrow_move_assignable_v<owner>)
        requires(::std::is_move_assignable_v<owner>)
    { if (this != &other) 
    { ::std::swap(data_, other.data_);
      ::std::swap(static_cast<base&>(*this), static_cast<base&&>(other)); 
      base::tweak(data_.get()); }
      return *this; }

    template< typename Factory, typename...Args
            , typename FOT = decltype(Factory::make(::std::declval<Args&&>()...))
            , typename Impl = typename owner_traits<FOT>::type>
    constexpr generic(Factory &&factory, Args&&...args)
        noexcept(noexcept(Factory::make(::std::declval<Args&&>()...)) && ::std::constructible_from<owner, FOT>)
        requires(::std::convertible_to<FOT, owner> ) 
        : base{::std::in_place_type<Impl>, nullptr}
        , data_{Factory::make(static_cast<Args&&>(args)...)}
    { base::tweak(data_.get()); }

    template<typename T>
    constexpr generic(T &&something_canbe_narrowized)
        requires(::std::convertible_to<T&&, owner>)
        : base{::std::in_place_type<typename owner_traits<::std::remove_cvref_t<T>>::type>, nullptr}
        , data_{static_cast<T&&>(something_canbe_narrowized)}
    { base::tweak(data_.get()); }

    template<typename T>
    constexpr generic& operator=(T &&something_canbe_narrowized)
        requires(::std::convertible_to<T&&, owner>)
    { data_ = static_cast<T&&>(something_canbe_narrowized);
      static_cast<base&>(*this)
        = base{::std::in_place_type<typename owner_traits<::std::remove_cvref_t<T>>::type>
              , data_.get()}; }

    ~generic() = default;

    template< typename Factory, typename...Args
            , typename FOT = decltype(Factory::make(::std::declval<Args&&>()...))
            , typename Impl = typename owner_traits<FOT>::type>
    constexpr void emplace(Args&&...args) 
        noexcept(noexcept(Factory::make(::std::declval<Args&&>()...)) && ::std::constructible_from<owner, FOT>)
        requires(::std::convertible_to<FOT, owner> )
    { auto &ptr = data_ = Factory::make(static_cast<Args&&>(args)...);
      static_cast<base&>(*this) = base{::std::in_place_type<typename owner_traits<decltype(Factory::make(::std::declval<Args&&>()...))>::type>, ptr.get()}; }

    constexpr auto detach() noexcept 
    { base::reset(); return::std::exchange(data_, {}); }

    constexpr auto &get() noexcept { return data_; }
    constexpr auto &get() const noexcept { return data_; }

    // constexpr const auto &info() const noexcept { return base::get_meta(); }

    constexpr operator bool() const noexcept { return static_cast<bool>(data_.get()); }

    template< covariant_from<This> To
            , ::std::constructible_from<owner const&> OtherOwner=Ownership>
    constexpr generic<typename covariant<This, To>::super_type, OtherOwner> as() const&
        noexcept(::std::is_nothrow_constructible_v<OtherOwner, owner const&>)
        requires(::std::constructible_from<OtherOwner, owner const&>)
    { return {*this}; }

    template< covariant_from<This> To
            , ::std::constructible_from<owner const&> OtherOwner=Ownership>
    constexpr generic<typename covariant<This, To>::super_type, OtherOwner> as() && 
        noexcept(::std::is_nothrow_constructible_v<OtherOwner, owner&&>) 
    { return {static_cast<generic&&>(*this)}; }
    
    template< covariant_from<This> To=This>
    constexpr view<To> as() & noexcept 
    { return {nullptr, static_cast<base&>(*this)}; }

private:
    owner data_;
};


#pragma endregion

// @tparam T: avoid 'require<>', requirement same with `ProtoOrInterfaces`.
// @tparam ProtoOrInterfaces: type satisfied `proto` or concrete *`Interfaces`*.	q+	QW0
template<typename...ProtoOrInterfaces>
using require = typename make_concept<type_list<ProtoOrInterfaces...>, type_list<>>::type;

}

#if !defined(DYNAMIC_CONCEPT_INTERFACE)
using impl::box;
using impl::view;
using impl::generic;
#endif

using impl::proto;
using impl::require;

}

#if defined(DYNAMIC_CONCEPT_INTERFACE)

namespace dyn::impl
{

template<typename T>
struct friend_self_
{
    using type = T&;
    using self = impl::qual_like_t<dyn::self, T&>;
};
template<typename T>
struct friend_self_<T&&>
{
    using type = T&&;
    using self = dyn::self&&;
};

template<typename Rt, typename T, typename V=impl::type_list<>>
struct friend_proto;
template<typename Rt, typename T, typename...Ts, typename...Rs>
struct friend_proto<Rt, impl::type_list<T, Ts...>, impl::type_list<Rs...>> 
    : friend_proto<Rt, impl::type_list<Ts...>, impl::type_list<T, Rs...>>
{};
template<typename Rt, typename...Ts, typename...Rs>
struct friend_proto<Rt, impl::type_list<void, Ts...>, impl::type_list<Rs...>> 
    : friend_proto<Rt, impl::type_list<>, impl::type_list<Rs..., Ts...>>
{};
template<typename Rt, typename...Rs>
struct friend_proto<Rt, impl::type_list<>, impl::type_list<Rs...>> 
    : std::type_identity<Rt(Rs...)>
{};

}

namespace dyn
{
namespace impl
{

template<::std::size_t index, typename T, typename...Ss>
struct make_require_impl : ::std::type_identity<impl::require<Ss...>>
{};

template<::std::size_t index, typename T, typename...Ss>
    requires(::std::default_initializable<typename T::template dcc_conv<index, void>>)
struct make_require_impl<index, T, Ss...> 
    : make_require_impl<index+1, T, Ss..., typename T::template dcc_conv<index, void>>
{};

template<template<typename...>typename...>
struct template_list;


template<typename T, typename...Bases>
struct make_require : make_require_impl<0, T, Bases...> {};

template<typename T, typename=void>
struct dyn_ : dyn_<T, typename T::dcc_base> {};
template<typename T, template<typename...>typename TP, typename...This>
struct dyn_<T, TP<This...>> : public T
{
    using dynamic = typename make_require<T, dyn_<This>...>::type;
};

template<typename Conv, typename T, typename...Args>
inline constexpr auto DYNAMIC_CONCEPT_FORCEINLINE dcc_invoke(T *self, Args&&...args)
{ return dyn_<T>::dynamic::template invoke<Conv>(self, static_cast<Args&&>(args)...); }

template<typename F, typename T>
    requires(requires{ typename F::have_dcc_conv; typename T::have_dcc_conv; })
struct covariant<F, T> : covariant<dyn_<F>, dyn_<T>> {};
template<typename F, typename T>
struct covariant<dyn_<F>, dyn_<T>> : covariant<typename dyn_<F>::dynamic::sign, typename dyn_<T>::dynamic::sign> 
{
    using this_type = dyn_<F>;
    using super_type = dyn_<T>;
};

}

template<typename T>
using view = impl::view<impl::dyn_<T>>;
template<typename T>
using box = impl::box<impl::dyn_<T>>;
template<typename T, typename Ownership>
using generic = impl::generic<impl::dyn_<T>, Ownership>;
}

#define DYN_CONCEPT_CONCAT_IMPL_(_0, _1) _0##_1
#define DYN_CONCEPT_CONCAT_(_0, _1) DYN_CONCEPT_CONCAT_IMPL_(_0, _1)

#define DYN_CONCEPT_COUNTER_IMPL_(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,...) _15
#define DYN_CONCEPT_COUNTER_(...) \
    DYN_CONCEPT_COUNTER_IMPL_(__VA_OPT__(__VA_ARGS__,) 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)

#define DYN_CONCEPT_COUNTER_W_IMPL_(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,...) _15
#define DYN_CONCEPT_COUNTER_W_(...) \
    DYN_CONCEPT_COUNTER_W_IMPL_(__VA_OPT__(__VA_ARGS__,) 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)

// if you ask me why we expand it manually?
// because only by this way, the intellsense wouldn't died.
// oops? you asking the why making table manually?
// because C like let people make table manually.

#define DYN_CONCEPT_OP_TABLE_0(MACRO)
#define DYN_CONCEPT_OP_TABLE_1(MACRO, _1) \
    MACRO(_1)
#define DYN_CONCEPT_OP_TABLE_2(MACRO, _1, _2) \
    MACRO(_1), MACRO(_2)
#define DYN_CONCEPT_OP_TABLE_3(MACRO, _1, _2, _3) \
    MACRO(_1), MACRO(_2), MACRO(_3)
#define DYN_CONCEPT_OP_TABLE_4(MACRO, _1, _2, _3, _4) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4)
#define DYN_CONCEPT_OP_TABLE_5(MACRO, _1, _2, _3, _4, _5) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5)
#define DYN_CONCEPT_OP_TABLE_6(MACRO, _1, _2, _3, _4, _5, _6) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6)
#define DYN_CONCEPT_OP_TABLE_7(MACRO, _1, _2, _3, _4, _5, _6, _7) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6), MACRO(_7)
#define DYN_CONCEPT_OP_TABLE_8(MACRO, _1, _2, _3, _4, _5, _6, _7, _8) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6), MACRO(_7), MACRO(_8)
#define DYN_CONCEPT_OP_TABLE_9(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6), MACRO(_7), MACRO(_8), MACRO(_9)
#define DYN_CONCEPT_OP_TABLE_10(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6), MACRO(_7), \
    MACRO(_8), MACRO(_9), MACRO(_10)
#define DYN_CONCEPT_OP_TABLE_11(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6), MACRO(_7), \
    MACRO(_8), MACRO(_9), MACRO(_10), MACRO(_11)
#define DYN_CONCEPT_OP_TABLE_12(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6), MACRO(_7), \
    MACRO(_8), MACRO(_9), MACRO(_10), MACRO(_11), MACRO(_12)
#define DYN_CONCEPT_OP_TABLE_13(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6), MACRO(_7), \
    MACRO(_8), MACRO(_9), MACRO(_10), MACRO(_11), MACRO(_12), MA90CRO(_13)
#define DYN_CONCEPT_OP_TABLE_14(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6), MACRO(_7), \
    MACRO(_8), MACRO(_9), MACRO(_10), MACRO(_11), MACRO(_12), MACRO(_13), MACRO(_14)
#define DYN_CONCEPT_OP_TABLE_15(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) \
    MACRO(_1), MACRO(_2), MACRO(_3), MACRO(_4), MACRO(_5), MACRO(_6), MACRO(_7), \
    MACRO(_8), MACRO(_9), MACRO(_10), MACRO(_11), MACRO(_12), MACRO(_13), MACRO(_14), MACRO(_15)
#define DYN_CONCEPT_OP_TABLE_(MACRO, ...) \
    DYN_CONCEPT_CONCAT_(DYN_CONCEPT_OP_TABLE_, DYN_CONCEPT_COUNTER_(__VA_ARGS__))(MACRO __VA_OPT__(,__VA_ARGS__))

#define DYN_CONCEPT_OP_TABLE_W_0(MACRO) 
#define DYN_CONCEPT_OP_TABLE_W_1(MACRO, _1) \
    MACRO(_1)
#define DYN_CONCEPT_OP_TABLE_W_2(MACRO, _1, _2) \
    MACRO(_1); MACRO(_2)
#define DYN_CONCEPT_OP_TABLE_W_3(MACRO, _1, _2, _3) \
    MACRO(_1); MACRO(_2); MACRO(_3)
#define DYN_CONCEPT_OP_TABLE_W_4(MACRO, _1, _2, _3, _4) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4)
#define DYN_CONCEPT_OP_TABLE_W_5(MACRO, _1, _2, _3, _4, _5) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5)
#define DYN_CONCEPT_OP_TABLE_W_6(MACRO, _1, _2, _3, _4, _5, _6) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6)
#define DYN_CONCEPT_OP_TABLE_W_7(MACRO, _1, _2, _3, _4, _5, _6, _7) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6); MACRO(_7)
#define DYN_CONCEPT_OP_TABLE_W_8(MACRO, _1, _2, _3, _4, _5, _6, _7, _8) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6); MACRO(_7); MACRO(_8)
#define DYN_CONCEPT_OP_TABLE_W_9(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6); MACRO(_7); MACRO(_8); MACRO(_9)
#define DYN_CONCEPT_OP_TABLE_W_10(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6); MACRO(_7); \
    MACRO(_8); MACRO(_9); MACRO(_10)
#define DYN_CONCEPT_OP_TABLE_W_11(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6); MACRO(_7); \
    MACRO(_8); MACRO(_9); MACRO(_10); MACRO(_11)
#define DYN_CONCEPT_OP_TABLE_W_12(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6); MACRO(_7); \
    MACRO(_8); MACRO(_9); MACRO(_10); MACRO(_11); MACRO(_12)
#define DYN_CONCEPT_OP_TABLE_W_13(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6); MACRO(_7); \
    MACRO(_8); MACRO(_9); MACRO(_10); MACRO(_11); MACRO(_12); MACRO(_13)
#define DYN_CONCEPT_OP_TABLE_W_14(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6); MACRO(_7); \
    MACRO(_8); MACRO(_9); MACRO(_10); MACRO(_11); MACRO(_12); MACRO(_13); MACRO(_14)
#define DYN_CONCEPT_OP_TABLE_W_15(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) \
    MACRO(_1); MACRO(_2); MACRO(_3); MACRO(_4); MACRO(_5); MACRO(_6); MACRO(_7); \
    MACRO(_8); MACRO(_9); MACRO(_10); MACRO(_11); MACRO(_12); MACRO(_13); MACRO(_14); MACRO(_15)
#define DYN_CONCEPT_OP_TABLE_W_(MACRO, ...) \
DYN_CONCEPT_CONCAT_(DYN_CONCEPT_OP_TABLE_W_, DYN_CONCEPT_COUNTER_(__VA_ARGS__))(MACRO __VA_OPT__(,__VA_ARGS__))

#define DYN_CONCEPT_OP_TABLE_B_0(MACRO) 
#define DYN_CONCEPT_OP_TABLE_B_1(MACRO, _1) \
    MACRO(_1)
#define DYN_CONCEPT_OP_TABLE_B_2(MACRO, _1, _2) \
    MACRO(_1) MACRO(_2)
#define DYN_CONCEPT_OP_TABLE_B_3(MACRO, _1, _2, _3) \
    MACRO(_1) MACRO(_2) MACRO(_3)
#define DYN_CONCEPT_OP_TABLE_B_4(MACRO, _1, _2, _3, _4) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4)
#define DYN_CONCEPT_OP_TABLE_B_5(MACRO, _1, _2, _3, _4, _5) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5)
#define DYN_CONCEPT_OP_TABLE_B_6(MACRO, _1, _2, _3, _4, _5, _6) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6)
#define DYN_CONCEPT_OP_TABLE_B_7(MACRO, _1, _2, _3, _4, _5, _6, _7) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7)
#define DYN_CONCEPT_OP_TABLE_B_8(MACRO, _1, _2, _3, _4, _5, _6, _7, _8) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) MACRO(_8)
#define DYN_CONCEPT_OP_TABLE_B_9(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) MACRO(_8) MACRO(_9)
#define DYN_CONCEPT_OP_TABLE_B_10(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) \
    MACRO(_8) MACRO(_9) MACRO(_10)
#define DYN_CONCEPT_OP_TABLE_B_11(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) \
    MACRO(_8) MACRO(_9) MACRO(_10) MACRO(_11)
#define DYN_CONCEPT_OP_TABLE_B_12(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) \
    MACRO(_8) MACRO(_9) MACRO(_10) MACRO(_11) MACRO(_12)
#define DYN_CONCEPT_OP_TABLE_B_13(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) \
    MACRO(_8) MACRO(_9) MACRO(_10) MACRO(_11) MACRO(_12) MACRO(_13)
#define DYN_CONCEPT_OP_TABLE_B_14(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) \
    MACRO(_8) MACRO(_9) MACRO(_10) MACRO(_11) MACRO(_12) MACRO(_13) MACRO(_14)
#define DYN_CONCEPT_OP_TABLE_B_15(MACRO, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15) \
    MACRO(_1) MACRO(_2) MACRO(_3) MACRO(_4) MACRO(_5) MACRO(_6) MACRO(_7) \
    MACRO(_8) MACRO(_9) MACRO(_10) MACRO(_11) MACRO(_12) MACRO(_13) MACRO(_14) MACRO(_15)
#define DYN_CONCEPT_OP_TABLE_B_(MACRO, ...) \
DYN_CONCEPT_CONCAT_(DYN_CONCEPT_OP_TABLE_B_, DYN_CONCEPT_COUNTER_(__VA_ARGS__))(MACRO __VA_OPT__(,__VA_ARGS__))

#define DYN_CONCEPT_TO_STR_IMPL_(value) #value
#define DYN_CONCEPT_TO_STR_(value) DYN_CONCEPT_TO_STR_IMPL_(value)

#define DYN_CONCEPT_EMPTY_INHERITANCE_IMPL_(...) __VA_OPT__(,__VA_ARGS__) 
#define DYN_CONCEPT_EMPTY_INHERITANCE_(value) DYN_CONCEPT_EMPTY_INHERITANCE_IMPL_ value 

#define DYN_CONCEPT_CONVENSION_NAME_(name) convension<sizeof(#name), #name>
#define DYN_CONCEPT_FORWARD_(args) static_cast<decltype(args)&&>(args)

#define DYN_CONCEPT_FORWARD_W_IMPL_(name, type, ...) static_cast<type>(name)
#define DYN_CONCEPT_FORWARD_W_(value) DYN_CONCEPT_FORWARD_W_IMPL_ value 

#define DYN_CONCEPT_EXPAND_PARAMETER_IMPL_(name, type, ...) type name
#define DYN_CONCEPT_EXPAND_PARAMETER_(value) DYN_CONCEPT_EXPAND_PARAMETER_IMPL_ value

#define DYN_CONCEPT_EXPAND_PARAMETER_TYPE_IMPL_(name, type, ...) type
#define DYN_CONCEPT_EXPAND_PARAMETER_TYPE_(value) DYN_CONCEPT_EXPAND_PARAMETER_IMPL_ value

#define DYN_CONCEPT_DYN_FN_IMPL_(index, name, qualifier, return_type, ...)\
    public:\
        template<::std::size_t, typename> struct dcc_conv; \
        template<typename T__> struct dcc_conv<index, T__> { \
        using proto = \
            return_type(DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_EXPAND_PARAMETER_TYPE_,__VA_ARGS__));\
        template<typename Impl_, typename...Args> \
            constexpr decltype(auto) operator()(Impl_ &&impl, Args&&...args) \
                { return static_cast<Impl_&&>(impl).name(static_cast<Args&&>(args)...); } }; \
        constexpr return_type DYNAMIC_CONCEPT_FORCEINLINE name(DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_EXPAND_PARAMETER_,__VA_ARGS__)) qualifier \
        { return dyn::impl::dcc_invoke<dcc_conv<index, void>>(this \
                __VA_OPT__(,DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_FORWARD_W_,__VA_ARGS__))); }

#define DYN_CONCEPT_FORWARD_FF_IMPL_IMPL_2(name, type) ,static_cast<type>(name)
#define DYN_CONCEPT_FORWARD_FF_IMPL_IMPL_1(type)
#define DYN_CONCEPT_FORWARD_FF_IMPL_(...) DYN_CONCEPT_CONCAT_(DYN_CONCEPT_FORWARD_FF_IMPL_IMPL_, DYN_CONCEPT_COUNTER_(__VA_ARGS__))(__VA_ARGS__)
#define DYN_CONCEPT_FORWARD_FF_(value) DYN_CONCEPT_FORWARD_FF_IMPL_ value 

#define DYN_CONCEPT_PARAMETER_TYPE_FF_IMPL_IMPL_2(name, type) type
#define DYN_CONCEPT_PARAMETER_TYPE_FF_IMPL_IMPL_1(type) typename dyn::impl::friend_self_<type>::self
#define DYN_CONCEPT_PARAMETER_TYPE_FF_IMPL_(...) DYN_CONCEPT_CONCAT_(DYN_CONCEPT_PARAMETER_TYPE_FF_IMPL_IMPL_, DYN_CONCEPT_COUNTER_(__VA_ARGS__))(__VA_ARGS__)
#define DYN_CONCEPT_PARAMETER_TYPE_FF_(value) DYN_CONCEPT_PARAMETER_TYPE_FF_IMPL_ value 

#define DYN_CONCEPT_PARAMETER_SELF_TYPE_FF_IMPL_IMPL_2(name, type) 
#define DYN_CONCEPT_PARAMETER_SELF_TYPE_FF_IMPL_IMPL_1(type) type
#define DYN_CONCEPT_PARAMETER_SELF_TYPE_FF_IMPL_(...) DYN_CONCEPT_CONCAT_(DYN_CONCEPT_PARAMETER_SELF_TYPE_FF_IMPL_IMPL_, DYN_CONCEPT_COUNTER_(__VA_ARGS__))(__VA_ARGS__)
#define DYN_CONCEPT_PARAMETER_SELF_TYPE_FF_(value) DYN_CONCEPT_PARAMETER_SELF_TYPE_FF_IMPL_ value

#define DYN_CONCEPT_PARAMETER_FF_IMPL_IMPL_2(name, type) type name
#define DYN_CONCEPT_PARAMETER_FF_IMPL_IMPL_1(type_) Self__ &&self
#define DYN_CONCEPT_PARAMETER_FF_IMPL_(...) DYN_CONCEPT_CONCAT_(DYN_CONCEPT_PARAMETER_FF_IMPL_IMPL_, DYN_CONCEPT_COUNTER_(__VA_ARGS__))(__VA_ARGS__)
#define DYN_CONCEPT_PARAMETER_FF_(value) DYN_CONCEPT_PARAMETER_FF_IMPL_ value 

#define DYN_CONCEPT_DYN_FF_IMPL_(index, name, qualifier, return_type, ...)\
    public:\
        template<::std::size_t, typename> struct dcc_conv; \
        template<typename T__> struct dcc_conv<index, T__> { \
        using proto = \
            return_type(DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_PARAMETER_TYPE_FF_,__VA_ARGS__));\
        template<typename...Args> \
            constexpr decltype(auto) operator()(Args&&...args) \
                { return name(static_cast<Args&&>(args)...); } }; \
        template<typename Self__>\
        friend constexpr return_type DYNAMIC_CONCEPT_FORCEINLINE name(DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_PARAMETER_FF_,__VA_ARGS__)) qualifier \
            requires(::std::derived_from<::std::remove_reference_t<Self__>, \
                ::std::remove_reference_t<DYN_CONCEPT_OP_TABLE_B_(DYN_CONCEPT_PARAMETER_SELF_TYPE_FF_,__VA_ARGS__)>>)\
        { return dyn::impl::dcc_invoke<dcc_conv<index, void>>(\
            (::std::remove_reference_t<DYN_CONCEPT_OP_TABLE_B_(DYN_CONCEPT_PARAMETER_SELF_TYPE_FF_,__VA_ARGS__)>*)(&self) \
                DYN_CONCEPT_OP_TABLE_B_(DYN_CONCEPT_FORWARD_FF_,__VA_ARGS__)); }

               
#define DYN_CONCEPT_DYN_TRANSFER_CONV_IMPL_(base, index) typename base::template dcc_conv<index, void>
#define DYN_CONCEPT_DYN_TRANSFER_CONV_(value) DYN_CONCEPT_DYN_TRANSFER_CONV_IMPL_ value

#define DYN_CONCEPT_DYN_TO_IMPL_(base, name, qualifier)\
    public: \
        template<typename...Args> \
        constexpr decltype(auto) name(Args&&...args) qualifier \
        { return dyn::impl::dcc_invoke<DYN_CONCEPT_DYN_TRANSFER_CONV_(base)>(this, static_cast<Args&&>(args)...); }


#define DYN_CONCEPT_DYN_CC_IMPL_(...) \
    public:\
        using dcc_base = dyn::impl::type_list<__VA_ARGS__>;\
        struct have_dcc_conv;

// Define member functionality, macro can be replace by ','.
// Pattern should like:
//  DYN_FN(<func index> NAMED <func name> WITH [qualifier] RETURN <return type> TAKE [(<argument's name> NAMED <type>)...])
// N--------------------A
// <func name>: must samed with impl's.
#define DYN_FN(...) DYN_CONCEPT_DYN_FN_IMPL_(__VA_ARGS__)
// Define friend functionality, macro can be replace by ','.
// Remember that qualifier is not allow with 'const' or '&'.
// Pattern should like:
//  DYN_FF(<func index> NAMED <func name> WITH [qualifier] RETURN <return type> TAKE [(<argument's name> NAMED <type>)...])
// N--------------------A
// <func name>: must samed with impl's.
// <argument's name>: must and only can contain one interface type. expressed by (<interface>).
#define DYN_FF(...) DYN_CONCEPT_DYN_FF_IMPL_(__VA_ARGS__)
// Inherit functionality from base, macro can be replace by ','.
// Pattern should like:
//  DYN_TO((<base type>, <func index>) NAMED <func name> WITH [qualifier])
// N--------------------A
// <func name>: not required to be samed with impl's
#define DYN_TO(...) DYN_CONCEPT_DYN_TO_IMPL_(__VA_ARGS__)
// Define friend functionality, 'EXTENDS(...)' can be replace by ', (...)'.
// Pattern should like:
//  DYN_CC(<interface name> EXTENDS([base type...]))
#define DYN_CC(...) DYN_CONCEPT_DYN_CC_IMPL_(__VA_ARGS__)

#if !defined(DYN_CONCEPT_NO_NETURAL_LANG_PRIMITIVE)
#  if defined(MUST) || defined(UNADORNED) || defined(RETURN) || defined(TAKE) || defined(EXTENDS)
#    warning "Some MACRO have same naming with dynamic-concept's, the behavior will be undefined."
#  endif
// [FROM DYNAMIC-CONCEPT] Mark that next part are return type.
#  define RETURN ,
// [FROM DYNAMIC-CONCEPT] Mark that rest part are parameters.
#  define TAKE ,
// [FROM DYNAMIC-CONCEPT] Mark that the dynamic concept extend these kind of dynamic concept.
#  define EXTENDS(...) , (__VA_ARGS__)
// [FROM DYNAMIC-CONCEPT] Mark functionality next part is qualifier, qualfier can be empty. 
#  define WITH ,
// [FROM DYNAMIC-CONCEPT] Mark that next part is name.
#  define NAMED ,
#endif

#endif

#undef DYNAMIC_CONCEPT_EXPORT_AS_MODULE
#endif
