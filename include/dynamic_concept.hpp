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

template<::std::size_t index, typename T>
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
    if constexpr (::std::same_as<From, T>) return index;
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
struct prototype_traits : ::std::false_type {};

template<typename T, typename Invoker, self_duck Self, typename...Ts>
struct prototype_traits<T(Invoker::*)(Self, Ts...)> : ::std::true_type
{
    using type = T(Ts...);
    // using type = T(Invoker::*)(Ts...);
    using ptype = T(*)(void*, Ts...);
    
    using invoker = Invoker;
    using return_type = T;

    template<typename Impl>
    static constexpr T transfer(void *ptr, Ts...args)
    { return Invoker()(static_cast<qual_like_t<Impl, Self>>(*static_cast<Impl*>(ptr)), static_cast<Ts>(args)...); }

    template<typename T>
    static constexpr auto invocable = ::std::invocable<Invoker, T, Ts...>;
};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...)> : prototype_traits<T(Invoker::*)(self&, Ts...)> {};

struct functor_convension // this is default convension.
{
    template<typename Impl, typename...Args>
    constexpr decltype(auto) operator()(Impl &&ref, Args &&...args)
        noexcept(::std::is_nothrow_invocable_v<Impl&, Args&&...>)
    { return static_cast<Impl&&>(ref)(static_cast<Args&&>(args)...); }
};
template<typename T, self_duck Self, typename...Ts>
struct prototype_traits<T(Self, Ts...)> : prototype_traits<T(functor_convension::*)(Self, Ts...)> {};

// function qualifier is most garbage thing that i ever seen.

template<typename T, typename...Ts>
struct prototype_traits<T(Ts...)> : prototype_traits<T(self&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...)&&> : prototype_traits<T(functor_convension::*)(self&&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...)&> : prototype_traits<T(functor_convension::*)(self&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const &> : prototype_traits<T(functor_convension::*)(self const&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) volatile &> : prototype_traits<T(functor_convension::*)(self volatile &, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const volatile &> : prototype_traits<T(functor_convension::*)(self const volatile &, Ts...)> {};

template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const &&> : prototype_traits<T(functor_convension::*)(self const&&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) volatile &&> : prototype_traits<T(functor_convension::*)(self volatile &&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const volatile &&> : prototype_traits<T(functor_convension::*)(self const volatile &&, Ts...)> {};

template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const> : prototype_traits<T(functor_convension::*)(self const&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) volatile> : prototype_traits<T(functor_convension::*)(self volatile&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const volatile> : prototype_traits<T(functor_convension::*)(self const volatile&, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...)&&> : prototype_traits<T(Invoker::*)(self&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...)&> : prototype_traits<T(Invoker::*)(self&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const &> : prototype_traits<T(Invoker::*)(self const&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) volatile &> : prototype_traits<T(Invoker::*)(self volatile &, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const volatile &> : prototype_traits<T(Invoker::*)(self const volatile &, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const &&> : prototype_traits<T(Invoker::*)(self const&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) volatile &&> : prototype_traits<T(Invoker::*)(self volatile &&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const volatile &&> : prototype_traits<T(Invoker::*)(self const volatile &&, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const> : prototype_traits<T(Invoker::*)(self const&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) volatile> : prototype_traits<T(Invoker::*)(self volatile&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const volatile> : prototype_traits<T(Invoker::*)(self const volatile&, Ts...)> {};

// For FUCKING noexcept!!!

template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) noexcept> : prototype_traits<T(self&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...)&& noexcept> : prototype_traits<T(functor_convension::*)(self&&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...)& noexcept> : prototype_traits<T(functor_convension::*)(self&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const & noexcept> : prototype_traits<T(functor_convension::*)(self const&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) volatile & noexcept> : prototype_traits<T(functor_convension::*)(self volatile &, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const volatile & noexcept> : prototype_traits<T(functor_convension::*)(self const volatile &, Ts...)> {};

template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const && noexcept> : prototype_traits<T(functor_convension::*)(self const&&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) volatile && noexcept> : prototype_traits<T(functor_convension::*)(self volatile &&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const volatile && noexcept> : prototype_traits<T(functor_convension::*)(self const volatile &&, Ts...)> {};

template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const noexcept> : prototype_traits<T(functor_convension::*)(self const&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) volatile noexcept> : prototype_traits<T(functor_convension::*)(self volatile&, Ts...)> {};
template<typename T, typename...Ts>
struct prototype_traits<T(Ts...) const volatile noexcept> : prototype_traits<T(functor_convension::*)(self const volatile&, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) noexcept> : prototype_traits<T(Invoker::*)(self&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) volatile noexcept> : prototype_traits<T(Invoker::*)(self&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const volatile noexcept> : prototype_traits<T(Invoker::*)(self&&, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...)&& noexcept> : prototype_traits<T(Invoker::*)(self&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...)& noexcept> : prototype_traits<T(Invoker::*)(self&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const & noexcept> : prototype_traits<T(Invoker::*)(self const&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) volatile & noexcept> : prototype_traits<T(Invoker::*)(self volatile &, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const volatile & noexcept> : prototype_traits<T(Invoker::*)(self const volatile &, Ts...)> {};

template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const && noexcept> : prototype_traits<T(Invoker::*)(self const&&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) volatile && noexcept> : prototype_traits<T(Invoker::*)(self volatile &&, Ts...)> {};
template<typename T, typename Invoker, typename...Ts>
struct prototype_traits<T(Invoker::*)(Ts...) const volatile && noexcept> : prototype_traits<T(Invoker::*)(self const volatile &&, Ts...)> {};

template<typename T>
concept prototype = prototype_traits<T>::value;

template<typename T, typename Fn>
struct set_to_mem : set_to_mem<T, typename prototype_traits<Fn>::type>{};
template<typename T, typename Rt, typename...Args>
struct set_to_mem<T, Rt(Args...)> { using type = Rt(T::*)(Args...); };

template<typename T>
    requires(prototype<typename T::prototype>)
struct prototype_traits<T> 
    : prototype_traits<typename set_to_mem<T, typename T::prototype>::type> {};

template<typename T, typename...Ts> // might work on template specialization, but currently how to impl it remain unkown.
struct prototype_traits<T(*)(Ts...)> : prototype_traits<T(Ts...)>  
{ 
    using type = T(Ts...);
    static_assert(::std::is_abstract_v<T>?0:0, "Function pointer as prototype is reserved.");
};

#pragma endregion

template<typename...>
struct covariant : ::std::false_type {};
template<typename From, typename To>
    requires(requires { typename From::dynamic; } && requires{ typename To::dynamic; })
struct covariant<From, To> 
    : covariant<typename From::dynamic::sign, typename To::dynamic::sign> {};
template< template<typename...>typename TP
        , typename...Ts, typename...Rs>
struct covariant<TP<Ts...>, TP<Rs...>> : covariant<TP<Ts...>, TP<Rs...>, TP<>, TP<>> { };

// completely same.
template< template<typename...>typename TP, typename...Ts>
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

// @tparam MetaGenerator: Might use in static reflection or something.
// @tparam Base: User defined interface.
template<typename Base, template<typename>typename MetaGenerator, prototype...Signs>
struct virtual_bundle : public Base
{
    template<typename Base, template<typename>typename MetaGenerator, prototype...Signs>
    friend struct virtual_bundle;
    template<template<typename>typename MG, prototype...Ss>
    friend struct concept_descriptor;

protected:
    using sign = type_list<Signs...>;

private:
    template<typename T>
    using ptype_t = typename prototype_traits<T>::ptype;

    template<typename Impl, typename...Ss>
    struct make_table {};
    template<typename Impl, prototype T, prototype...Rs>
    struct make_table<Impl, T, Rs...> : make_table<Impl, Rs...> 
    {
        using trait = prototype_traits<T>;
        using type = ptype_t<T>; // GCC doesnt know what is `<template_object><T> fn = <type>::template transfer<...>`.
        type stub_ = &trait::template transfer<Impl>;
    };
    template<typename Impl, prototype...Ss>
    struct make_meta : make_table<Impl, Ss...> 
    {
        MetaGenerator<Impl> stub_;
    };
    template<typename Impl>
    static constexpr auto typeinfos_ = make_meta<::std::remove_cvref_t<Impl>, Signs...>{};

protected:
    template<typename T>
    static constexpr auto suitable = ((prototype_traits<Signs>::template invocable<T&>) && ...);

    constexpr virtual_bundle() = default;

    template<typename T>
    constexpr virtual_bundle(T *impl)
    { this->reset(impl); }

    template<typename T>
    constexpr virtual_bundle(::std::in_place_type_t<T> stub, void *impl)
    { this->reset(stub, impl); }

    template<typename Other, prototype...Os>
    constexpr virtual_bundle(virtual_bundle<Other, MetaGenerator, Os...> const &other)
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
    { return (*(reinterpret_cast<::std::add_const_t<T>*>(ptr) + (sizeof...(Signs) - (index)))); }

protected:
    template<::std::size_t index, typename...Args>
    constexpr DYNAMIC_CONCEPT_FORCEINLINE decltype(auto) invoke(Args&&...args) 
    { return virtual_bundle::template 
        get<index + 1, typename prototype_traits<type_at_t<index, sign>>::ptype>
            (ptr_)(this_, static_cast<Args&&>(args)...); }

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
    { return *reinterpret_cast<MetaGenerator<void> const*>
        (reinterpret_cast<void(*const*)()>(ptr_) + sizeof...(Signs)); }
   
private:
    void const *ptr_{nullptr};
    void *this_{nullptr};
};

template<typename From, typename To>
struct append_ {};

// Avoid redundant.

template<template<typename...>typename TP, prototype From, typename...Rs>
    requires((!::std::same_as<From, Rs>) && ...)
struct append_<From, TP<Rs...>> : ::std::type_identity<TP<Rs..., From>> {};
template<template<typename...>typename TP, prototype From, typename...Rs>
    requires((::std::same_as<From, Rs>) || ...)
struct append_<From, TP<Rs...>> : ::std::type_identity<TP<Rs...>> {};

template< template<template<typename>typename, typename...>typename CD
        , template<typename>typename MG
        , prototype From, typename...Rs>
    requires((!::std::same_as<From, Rs>) && ...)
struct append_<From, CD<MG, Rs...>> : ::std::type_identity<CD<MG, Rs..., From>> {};
template< template<template<typename>typename, typename...>typename CD
        , template<typename>typename MG
        , prototype From, typename...Rs>
    requires((::std::same_as<From, Rs>) || ...)
struct append_<From, CD<MG, Rs...>> : ::std::type_identity<CD<MG, Rs..., From>> {};

template<template<typename...>typename TP, typename...Rs>
struct append_<TP<>, TP<Rs...>> : ::std::type_identity<TP<Rs...>> {};
template<template<typename...>typename TP, prototype From, typename...Fs, typename...Rs>
struct append_<TP<From, Fs...>, TP<Rs...>> : append_<TP<Fs...>, typename append_<From, TP<Rs...>>::type> {};

template< template<template<typename>typename, typename...>typename FT, template<typename...>typename RT
        , template<typename>typename MG, typename...From, typename...To>
struct append_<FT<MG, From...>, RT<To...>> : append_<RT<From...>, RT<To...>> {};
template<typename From, typename To>
    requires(requires{ typename From::dynamic; })
struct append_<From, To> : append_<typename From::dynamic, To> {}; // for interface.

// DO NOT SPECIALIZATION THIS TEMPLATE.
template<template<typename>typename MG, prototype...Ss>
struct concept_descriptor
{
    using sign = type_list<Ss...>;
    // FOR INNER USAGE, DO NOT USE IT.
    template<typename Interface>
    using attach = impl::virtual_bundle<Interface, MG, Ss...>;

    // @tparam MetaGenerator: MetaGenerator<void> should be layout compatible with MetaGenerator<<Any Concrete Type>>.
    template<template<typename>typename MetaGenerator>
    using switch_meta = concept_descriptor<MetaGenerator, Ss...>;

    template<typename S>
    using append = typename append_<S, concept_descriptor<MG, Ss...>>::type;

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

// if you occur error with this part, mostly because some type doesn't satisfy `prototype` or *`Interface`*.
template<typename T, typename C>
struct make_concept 
{ static_assert(prototype<T>, "Some or one of the prototypes expressed wasn't satisfy concept of prototype."); }; 
template<typename...Rs>
struct make_concept<type_list<>, type_list<Rs...>> 
    : ::std::type_identity<concept_descriptor<::std::type_identity, Rs...>> 
{};
template<typename From, typename...Fs, typename...Rs>
struct make_concept<type_list<From, Fs...>, type_list<Rs...>>
    : make_concept<type_list<Fs...>, typename append_<From, type_list<Rs...>>::type>
{};


#pragma region embedded ownership manager.


template<typename Base>
class view : public Base::dynamic::template attach<Base>
{
    template<typename Base>
    friend class view;

    using base = typename Base::dynamic::template attach<Base>;
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

    template<covariant_to<Base> O>
    constexpr view(view<O> &other) noexcept
        : base{static_cast<view<O>::base const&>(other)}
    {}
    template<covariant_to<Base> O>
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

    template<covariant_from<Base> Other>
    constexpr view<Other> as() noexcept { return {*this}; }
};

template<typename Base>
class box : public Base::dynamic::template attach<Base>
{
    template<typename Base>
    friend class box;

    using base = typename Base::dynamic::template attach<Base>;
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
        requires(base::template suitable<T>)
    { this->template emplace<T>(static_cast<Args&&>(args)...); }

    template<covariant_to<Base> O>
    constexpr box(box<O> &&other)
        : base{static_cast<typename box<O>::base&&>(other)}
        , deleter_{::std::exchange(other.deleter_, nullptr)}
    {}
    template<typename O>
    constexpr box(box<O>&&) = delete;

    template<covariant_to<Base> O>
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

    template<covariant_to<Base> To>
    constexpr box<To> as() && noexcept { return {static_cast<box&&>(*this)}; }

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

template<typename Base, typename Ownership>
class generic : public Base::dynamic::template attach<Base>
{
    template<typename Base, typename Ownership>
    friend class generic;
    using base = typename Base::dynamic::template attach<Base>;
    using owner = Ownership;
public:
    constexpr generic() = default;

    template<covariant_to<Base> Other, typename OtherOwnership>
    constexpr generic(generic<Other, OtherOwnership> const &other) 
        noexcept(::std::is_nothrow_constructible_v<owner, OtherOwnership const&>)
        requires(::std::constructible_from<owner, OtherOwnership const&>)
        : base{static_cast<typename generic<Other, Ownership>::base const&>(other)}
        , data_{other.data_}
    { base::tweak(data_.get()); }
    template<typename Other, typename OtherOwnership>
    constexpr generic(generic<Other, OtherOwnership> const&) = delete;

    template<covariant_to<Base> Other, typename OtherOwnership>
    constexpr generic(generic<Other, OtherOwnership> &&other) 
        noexcept(::std::is_nothrow_move_constructible_v<owner>)
        requires(::std::constructible_from<owner, OtherOwnership&&>)
        : base{static_cast<typename generic<Other, Ownership>::base&&>(other)}
        , data_{static_cast<OtherOwnership&&>(other.data_)}
    { base::tweak(data_.get()); }
    template<typename Other, typename OtherOwnership>
    constexpr generic(generic<Other, OtherOwnership>&&) = delete;

    template<covariant_to<Base> Other, typename OtherOwnership>
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

    template<covariant_to<Base> Other, typename OtherOwnership>
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
        requires(::std::convertible_to<FOT, owner> && base::template suitable<Impl>) 
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
        requires(::std::convertible_to<FOT, owner> && base::template suitable<Impl>)
    { auto &ptr = data_ = Factory::make(static_cast<Args&&>(args)...);
      static_cast<base&>(*this) = base{::std::in_place_type<typename owner_traits<decltype(Factory::make(::std::declval<Args&&>()...))>::type>, ptr.get()}; }

    constexpr auto detach() noexcept 
    { base::reset(); return::std::exchange(data_, {}); }

    constexpr auto &get() noexcept { return data_; }
    constexpr auto &get() const noexcept { return data_; }

    // constexpr const auto &info() const noexcept { return base::get_meta(); }

    constexpr operator bool() const noexcept { return static_cast<bool>(data_.get()); }

    template< covariant_from<Base> To
            , ::std::constructible_from<owner const&> OtherOwner=Ownership>
    constexpr generic<To, OtherOwner> as() const&
        noexcept(::std::is_nothrow_constructible_v<OtherOwner, owner const&>) 
    { return {*this}; }

    template< covariant_from<Base> To
            , ::std::constructible_from<owner const&> OtherOwner=Ownership>
    constexpr generic<To, OtherOwner> as() && 
        noexcept(::std::is_nothrow_constructible_v<OtherOwner, owner&&>) 
    { return {static_cast<generic&&>(*this)}; }

private:
    owner data_;
};

#pragma endregion

// @tparam T: avoid 'require<>', requirement same with `ProtoOrInterfaces`.
// @tparam ProtoOrInterfaces: type satisfied `prototype` or concrete *`Interfaces`*.	q+	QW0
template<typename...ProtoOrInterfaces>
using require = typename make_concept<type_list<ProtoOrInterfaces...>, type_list<>>::type;
}

// namespace objects
// {
// 	template<template<typename...>typename TP, typename...Ts>
// 	struct make_owner : ::std::type_identity<TP<Ts...>> {};
// }
// template<typename T>
// using view = typename objects::make_owner<impl::view, T>::type;
// template<typename T>
// using box = typename objects::make_owner<impl::box, T>::type;
// template<typename T, typename Ownership>
// using generic = typename objects::make_owner<impl::generic, T, Ownership>::type;

using impl::box;
using impl::view;
using impl::generic;

using impl::prototype;
using impl::require;

}

// #undef DYNAMIC_CONCEPT_FORCEINLINE
#undef DYNAMIC_CONCEPT_EXPORT_AS_MODULE

#endif
