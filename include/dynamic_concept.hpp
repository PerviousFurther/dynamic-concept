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
template<::std::size_t index, template<typename...>typename Tp, typename F, typename...Ts>
struct type_at<index, Tp<F, Ts...>> : type_at<index - 1, Tp<Ts...>> {};
template<template<typename...>typename Tp, typename F, typename...Ts>
struct type_at<0, Tp<F, Ts...>> : ::std::type_identity<F> {};
template<::std::size_t index, typename T>
using type_at_t = typename type_at<index, T>::type;

template<typename T, ::std::size_t index, typename F, typename...Ts>
inline consteval auto index_of() noexcept 
{
	if constexpr (::std::same_as<F, T>) return index;
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

// @tparam MetaGenerator: Might use in static reflection or something.
// @tparam Base: User defined interface.
template<typename Base, template<typename>typename MetaGenerator, prototype...Signs>
struct virtual_bundle : public Base
{
protected:
	using sign = type_list<Signs...>;

private:
    template<template<typename>typename MG, prototype...Ss>
    friend struct concept_descriptor;

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
	constexpr virtual_bundle() = default;

	template<typename T>
	constexpr virtual_bundle(T *impl)
	{ this->reset(impl); }

	template<typename T>
	constexpr virtual_bundle(::std::in_place_type_t<T> stub, void *impl)
	{ this->reset(stub, impl); }

	constexpr virtual_bundle(virtual_bundle const&) noexcept = default;
	constexpr virtual_bundle &operator=(virtual_bundle const&) noexcept = default;

	constexpr virtual_bundle(virtual_bundle &&other) noexcept 
		: ptr_{::std::exchange(other.ptr_, nullptr)} 
		, this_{::std::exchange(other.this_, nullptr)} {}
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
			(ptr_)(this_, static_cast<Args &&>(args)...); }

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

	constexpr auto &get_meta() const noexcept 
	{ return *reinterpret_cast<MetaGenerator<void> const*>
		(reinterpret_cast<void(*const*)()>(ptr_) + sizeof...(Signs)); }
	
private:
	void const *ptr_{nullptr};
	void *this_{nullptr};
};


// DO NOT SPECIALIZATION THIS TEMPLATE.
template<typename From, typename To>
struct append_ {};

// Avoid redundant.

template<template<typename...>typename TP, prototype F, typename...Rs>
	requires((!::std::same_as<F, Rs>) && ...)
struct append_<F, TP<Rs...>> : ::std::type_identity<TP<Rs..., F>> {};
template<template<typename...>typename TP, prototype F, typename...Rs>
	requires((::std::same_as<F, Rs>) || ...)
struct append_<F, TP<Rs...>> : ::std::type_identity<TP<Rs...>> {};

template< template<template<typename>typename, typename...>typename CD
		, template<typename>typename MG
		, prototype F, typename...Rs>
	requires((!::std::same_as<F, Rs>) && ...)
struct append_<F, CD<MG, Rs...>> : ::std::type_identity<CD<MG, Rs..., F>> {};
template< template<template<typename>typename, typename...>typename CD
		, template<typename>typename MG
		, prototype F, typename...Rs>
	requires((::std::same_as<F, Rs>) || ...)
struct append_<F, CD<MG, Rs...>> : ::std::type_identity<CD<MG, Rs..., F>> {};

template<template<typename...>typename TP, typename...Rs>
struct append_<TP<>, TP<Rs...>> : ::std::type_identity<TP<Rs...>> {};
template<template<typename...>typename TP, prototype F, typename...Fs, typename...Rs>
struct append_<TP<F, Fs...>, TP<Rs...>> : append_<TP<Fs...>, typename append_<F, TP<Rs...>>::type> {};

template< template<template<typename>typename, typename...>typename FT, template<typename...>typename RT
	    , template<typename>typename MG, typename...From, typename...To>
struct append_<FT<MG, From...>, RT<To...>> : append_<RT<From...>, RT<To...>> {};
template<typename F, typename R>
	requires(requires{ typename F::dynamic; })
struct append_<F, R> : append_<typename F::dynamic, R> {}; // for interface.

// DO NOT SPECIALIZATION THIS TEMPLATE.
template<template<typename>typename MG, prototype...Ss>
struct concept_descriptor
{
	// FOR INNER USAGE, DO NOT USE IT.
	template<typename Interface>
	using attach = impl::virtual_bundle<Interface, ::std::type_identity, Ss...>;

	// @tparam MetaGenerator: MetaGenerator<void> should be layout compatible with MetaGenerator<<Any Concrete Type>>.
	template<template<typename>typename MetaGenerator>
	using set_meta_generator = concept_descriptor<MetaGenerator, Ss...>;
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
		template invoke<impl::index_of<Proto, 0, Ss...>()>(static_cast<Args &&>(args)...); }

	// This is not the right way.
	// template parameter Proto was not being placed in `dynamic`.
	// template<typename Proto, typename Interface, typename...Args>
	// static constexpr DYNAMIC_CONCEPT_FORCEINLINE void invoke(Interface *self, Args&&...args)
	// 	requires((!::std::same_as<Proto, Ss>) && ...)
	// { static_assert(prototype<Proto>?0:0, "Cannot index specified prototype from given `dynamic`."); }
};

// if you occur error with this part, mostly because some type doesn't satisfy `prototype` or *`Interface`*.
template<typename T, typename C>
struct make_concept 
{ static_assert(prototype<T>, "Some or one of the prototypes expressed wasn't satisfy concept of prototype."); }; 
template<typename...Rs>
struct make_concept<type_list<>, type_list<Rs...>> 
	: ::std::type_identity<concept_descriptor<::std::type_identity, Rs...>> 
{};
template<typename F, typename...Fs, typename...Rs>
struct make_concept<type_list<F, Fs...>, type_list<Rs...>>
	: make_concept<type_list<Fs...>, typename append_<F, type_list<Rs...>>::type>
{};


#pragma region embedded ownership manager.

template<typename Base>
class view : public Base::dynamic::template attach<Base>
{
	using base = typename Base::dynamic::template attach<Base>;
public:
	constexpr view() = default;
	constexpr view(view const&) = default;
	constexpr view(view &&) noexcept = default;
	constexpr view& operator=(view const&) = default;
	constexpr view& operator=(view &&) noexcept = default;

	template<typename T>
	constexpr view(T &impl)
		: base{&impl}
	{}

	template<typename T>
	constexpr void emplace(T &impl) noexcept
	{ base::reset(&impl); }

	constexpr void reset() noexcept 
	{ base::reset(); }

	constexpr auto info() const noexcept { return base::get_meta(); }

	operator bool() const noexcept { return base::get_this(); }
};

template<typename Base>
class box : public Base::dynamic::template attach<Base>
{
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
    { this->template emplace<T>(static_cast<Args&&>(args)...); }

	~box() { this->reset(); }

	template<typename T, typename...Args>
	constexpr void emplace(Args&&...args) noexcept
	{ if(deleter_) deleter_(base::get_this());
	  base::reset(new T{static_cast<Args&&>(args)...});
	  deleter_ = &do_delete<T>; }

	constexpr void reset() noexcept 
	{ if(deleter_) { ::std::exchange(deleter_, nullptr)(base::get_this()); base::reset(); } }

	constexpr const auto &info() const noexcept { return base::get_meta(); }

	operator bool() const noexcept { return deleter_; }

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
class generic_ : public Base::dynamic::template attach<Base>
{
	using base = typename Base::dynamic::template attach<Base>;
	using owner = Ownership;
public:
	constexpr generic_() = default;

	constexpr generic_(generic_ const &other)
		requires(::std::copy_constructible<owner>)
		: base{other}
		, data_{other.data_}
	{ base::tweak(data_.get()); }

	constexpr generic_(generic_ &&other) noexcept
		requires(::std::move_constructible<owner>)
		: base{static_cast<base&&>(other)}
		, data_{::std::exchange(other.data_, {})}
	{ base::tweak(data_.get()); };

	constexpr generic_& operator=(generic_ const &other)
		requires(::std::is_copy_assignable_v<owner>)
	{ if (this != &other) 
	{ data_ = other.data_;
	  static_cast<base&>(*this) = static_cast<base const&>(other);
	  base::tweak(data_.get()); }
	  return *this; }

	constexpr generic_& operator=(generic_ &&other) noexcept 
		requires(::std::is_move_assignable_v<owner>)
	{ if (this != &other) 
	{ ::std::swap(data_, other.data_);
	  ::std::swap(static_cast<base&>(*this), static_cast<base&&>(other)); 
	  base::tweak(data_.get()); }
	  return *this; }

	template<typename Factory, typename...Args>
	constexpr generic_(Factory &&factory, Args&&...args)
		requires(requires{ {Factory::make(::std::declval<Args&&>()...)} -> ::std::convertible_to<owner>; }) 
		: base{::std::in_place_type<typename owner_traits<decltype(Factory::make(::std::declval<Args&&>()...))>::type>, nullptr}
		, data_{Factory::make(static_cast<Args&&>(args)...)}
	{ base::tweak(data_.get()); }

	template<typename T>
	constexpr generic_(T &&something_canbe_narrowized)
		requires(::std::convertible_to<T&&, owner>)
		: base{::std::in_place_type<typename owner_traits<::std::remove_cvref_t<T>>::type>, nullptr}
		, data_{static_cast<T&&>(something_canbe_narrowized)}
	{ base::tweak(data_.get()); }

	template<typename T>
	constexpr generic_& operator=(T &&something_canbe_narrowized)
		requires(::std::convertible_to<T&&, owner>)
	{ data_ = static_cast<T&&>(something_canbe_narrowized);
	  static_cast<base&>(*this) = base{::std::in_place_type<typename owner_traits<::std::remove_cvref_t<T>>::type>, data_.get()}; }

	~generic_() = default;

	template<typename Factory, typename...Args>
	constexpr void emplace(Args&&...args) 
        requires(requires{ {Factory::make(::std::declval<Args&&>()...)} -> ::std::convertible_to<owner>; })
	{ auto &ptr = data_ = Factory::make(static_cast<Args&&>(args)...);
	  static_cast<base&>(*this) = base{::std::in_place_type<typename owner_traits<decltype(Factory::make(::std::declval<Args&&>()...))>::type>, ptr.get()}; }

	constexpr auto detach() noexcept 
	{ base::reset(); return::std::exchange(data_, {}); }

	constexpr auto &get() noexcept { return data_; }
	constexpr auto &get() const noexcept { return data_; }

	constexpr const auto &info() const noexcept { return base::get_meta(); }

	constexpr operator bool() const noexcept { return static_cast<bool>(data_.get()); }

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
// using generic = typename objects::make_owner<impl::generic_, T, Ownership>::type;

using impl::box;
using impl::view;

template<typename T, typename Ownership>
using generic = impl::generic_<T, Ownership>;

using impl::prototype;
using impl::require;

}

#undef DYNAMIC_CONCEPT_FORCEINLINE
#undef DYNAMIC_CONCEPT_EXPORT_AS_MODULE
#endif