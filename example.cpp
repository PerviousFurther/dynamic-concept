#include "dynamic_concept.hpp"

#include <iostream>

// first:
// why we better than `microsoft\proxy` is that we can
// have more customization the invocation of impl with less code.
template<typename Constraint, typename Transform=::std::identity>
struct program_
{
	template<typename Impl, typename...Args>
	constexpr decltype(auto) operator()(Impl &impl, Args&&...args) 
		noexcept(noexcept(::std::declval<Impl&>().program(::std::declval<Args&&>()...))) 
	{ if(!Constraint()(impl)) ::std::cout << "Panic!\n"; // i dont know why std::exception is different.
	return Transform()(impl).program(static_cast<Args &&>(args)...); }
};

struct work_5time // constraint will be invoke by `program_`
{
	template<typename T>
	constexpr bool operator()(T &value) noexcept {
		if constexpr (requires{ value.time; }) {
			// yep, some dynamic constraints...
			if (value.time != 5) return true;
			else return false;
		} else // some static constraints...
			static_assert(::std::is_abstract_v<T> ? false : false, "Drawable must be blah blah...");
	}
};

struct no_constraints
{
	constexpr bool operator()(auto&) noexcept { return true; }
};

template<typename Signature, typename Constraint, typename Transform=::std::identity>
struct eprogram_ : program_<Constraint, Transform>
{
	using signature = Signature;
};

struct eprogram : eprogram_<void(int), no_constraints> {};

struct programable 
{
	using dynamic 
		= dyn::require< void(program_<work_5time>::*)()
		, eprogram_<void(), work_5time>
		, eprogram>;

	// these extend the functionality of dynamic concept.
	// btw, you also can add `const` to qualify or `noexcept` to strength `concept`.

	void program() { dynamic::invoke<0>(this); }
	void program_unsafe() { dynamic::invoke<1>(this); }
	void program_unsafe(int v) { dynamic::invoke<2>(this, v); }
};

// However, if you dont want these customization.
// define as follow:
//
// ```cpp
// #define DEFINE_CONVENSION(convension_name, invoke_name) 
// struct convension_name \
// { \ 
// 	template<typename Impl, typename...Args> \
// 	constexpr decltype(auto) operator()(Impl &&impl, Args&&...args) \ 
// 		noexcept(noexcept(::std::declval<Impl&>().invoke_name(::std::declval<Args&&>()...))) \
// 	{ return static_cast<Impl&&>(impl).invoke_name(static_cast<Args &&>(args)...); } \
// };
// ```
// `convension_name`: is what you express to require.
// `invoke_name`: is what function will be invoke in impl.
// these would do the samething with proxy.
// NOTE: not compatible with proxy.

// actually, `operator()` can also constraint `interface`. 
struct sleep_
{
	template<typename Impl>
	constexpr decltype(auto) operator()(Impl &&impl) { return static_cast<Impl&&>(impl).sleep(); }
};

// second why we are better than `microsoft\proxy` is that
// we can merge the interfaces.
struct programmer
{
	using dynamic // you can directly push other interface into it.
		= dyn::require< programable 
		// `dyn::self` is duck type,
		// use mark the implmentation can only 
		// invoke in `right value reference` state.
		// NOTE: `dyn::self` should not be initialized.
		, bool (sleep_::*)(dyn::self&&)>;

	// TODO: however, losing the functionality of `programable`,
	// we might implment it some other time...

	void program() { dynamic::invoke<0>(this); }
	bool deep_sleep(int v) 
	{ dynamic::invoke<2>(this, v); 
	// the function index is from `0-3`,
	// because `programable` have 3 functionality.
	// thus if we need to invoke `<impl>::sleep`,
	// we `dynamic::invoke<3>`. 
	return dynamic::invoke<3>(this); }
};



// IMPLMENTATION.


struct joe
{
	void program() const { ::std::cout << "Joe: Hello world: " << time << ::std::endl; }
	void program(int value) { ::std::cout << "Joe: Hello world with set: " << (time = value) << ::std::endl; }

	// `&&` qualified is because the interface definition.
	bool sleep() && { ::std::cout << "Joe: I sleep: " << ::std::exchange(time, 0) << ::std::endl; return true; }

	int time{5};
};

struct mary
{
	void program() const { ::std::cout << "Mary: Hello world and what: " << time << ::std::endl; }
	void program(int value) { ::std::cout << "Mary: Hello world with set: " << (time = value) << ::std::endl; }

	bool sleep() && { ::std::cout << "Mary: I sleep: " << ::std::exchange(time, 0) << ::std::endl; return true; }

	int time{4};
};


// some builtin ownership.

void something(dyn::view<programmer> who)
{
	who.program();
	who.deep_sleep(5);
}

void some(dyn::box<programmer> who)
{
	who.program();
	who.deep_sleep(5);
}

// third we better than proxy is
// we have more compatiblity with smart_pointer
// or some other "ownership manager".
// NOTE: although we doesnt recommand it because multithread.

#include <memory>

// this is for in place initialization.
template<typename T>
struct shared_ptr_factory 
{ 
	template<typename...Args>
	static constexpr auto make(Args&&...args)
	{ return::std::make_shared<T>(static_cast<Args&&>(args)...); }
};

// because `shared_ptr` have `element_type`, 
// thus builtin `owner_traits` have partially specialization 
// for compatible with it.
// if you want some other pointer that doesnt contain specified member.
// just specialize the traits.  
void somewhat(dyn::generic_<programmer, ::std::shared_ptr<void>> who)
{
	who.program();
	who.deep_sleep(9);
}

int main(void)
{
	// test 1:
	{
		joe j;
		mary m;
		something(j);
		something(m);
	}

	// test 2:
	some({::std::in_place_type<joe>});
	some({::std::in_place_type<mary>});

	// test 3:
	{
		auto j = ::std::make_shared<joe>();
		somewhat(j);
		somewhat({shared_ptr_factory<mary>()});
		// after sleep, it also resulting zero.
		j->program();
	}
}
