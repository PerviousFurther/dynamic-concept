#if !defined(DYANMIC_CONCEPT_HELPER_HPP)
#define DYANMIC_CONCEPT_HELPER_HPP

#if !defined(DYANMIC_CONCEPT_HPP)
#include "dynamic_concept.hpp"
#endif


#define DYN_CONCEPT_CONCAT_IMPL_(_0, _1) _0##_1
#define DYN_CONCEPT_CONCAT_(_0, _1) DYN_CONCEPT_CONCAT_IMPL_(_0, _1)

#define DYN_CONCEPT_COUNTER_IMPL_(_0,_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,...) _15
#define DYN_CONCEPT_COUNTER_(...) \
    DYN_CONCEPT_COUNTER_IMPL_(__VA_OPT__(__VA_ARGS__,) 15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0)

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
    MACRO(_8), MACRO(_9), MACRO(_10), MACRO(_11), MACRO(_12), MACRO(_13)
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

namespace dyn::objects
{
    template<::std::size_t size=32>
    struct sstr
    {
        template<::std::size_t vsize>
        constexpr sstr(const char(&value)[vsize]) noexcept
        { for (auto i{0u}; i < vsize; i++) this->value[i] = value[i]; }

        constexpr bool operator==(sstr other) const noexcept
        { for (auto i{0u}; i < size; i++) 
            if (value[i] != other.value[i]) 
                return false;
        return true; }

        char value[size];
    };
    
    // template<::std::size_t size>
    // sstr(const char(&)[size]) -> sstr<size>;

    template<typename T, ::std::size_t index, typename...Ts>
    auto make_require_of() noexcept
    {
        if constexpr (::std::default_initializable<typename T::template conv<index>>)
            return objects::make_require_of<T, index + 1, Ts..., typename T::template conv<index>>();
        else 
            return dyn::impl::require<Ts...>{};
    };
}

#define DYN_CONCEPT_TO_STR_IMPL_(value) #value
#define DYN_CONCEPT_TO_STR_(value) DYN_CONCEPT_TO_STR_IMPL_(value)

#define DYN_CONCEPT_EMPTY_INHERITANCE_IMPL_(...) __VA_OPT__(,__VA_ARGS__) 
#define DYN_CONCEPT_EMPTY_INHERITANCE_(value) DYN_CONCEPT_EMPTY_INHERITANCE_IMPL_ value 

#define DYN_CONCEPT_CONVENSION_NAME_(name) convension<sizeof(#name), #name>
#define DYN_CONCEPT_FORWARD_(args) static_cast<decltype(args)&&>(args)

#define DYN_CONCEPT_DYN_REQUIRES_IMPL_(name, inheritance, ...) \
    private:\
        using self = name;\
    public:\
        using dynamic = \
            decltype(dyn::objects::make_require_of<name, 0 DYN_CONCEPT_EMPTY_INHERITANCE_(inheritance)>());\

#define DYN_CONCEPT_FORWARD_W_IMPL_(name, type, ...) static_cast<type>(name)
#define DYN_CONCEPT_FORWARD_W_(value) DYN_CONCEPT_FORWARD_W_IMPL_ value 

#define DYN_CONCEPT_EXPAND_PARAMETER_IMPL_(name, type, ...) type name
#define DYN_CONCEPT_EXPAND_PARAMETER_(value) DYN_CONCEPT_EXPAND_PARAMETER_IMPL_ value

#define DYN_CONCEPT_EXPAND_PARAMETER_TYPE_IMPL_(name, type, ...) type
#define DYN_CONCEPT_EXPAND_PARAMETER_TYPE_(value) DYN_CONCEPT_EXPAND_PARAMETER_IMPL_ value

#define DYN_CONCEPT_DYN_FN_IMPL_(index, name, qualifier, return_type, ...)\
    public:\
        template<::std::size_t> struct conv; \
        template<> struct conv<index> { \
        using prototype = \
            return_type(DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_EXPAND_PARAMETER_TYPE_,__VA_ARGS__));\
        template<typename T, typename...Args> \
            constexpr decltype(auto) operator()(T &&impl, Args&&...args) \
                { return static_cast<T&&>(impl).name(static_cast<Args&&>(args)...); } }; \
        constexpr return_type name(DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_EXPAND_PARAMETER_,__VA_ARGS__)) qualifier \
        { return self::dynamic::template invoke<typename self::template conv<index>>(this \
                __VA_OPT__(,DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_FORWARD_W_,__VA_ARGS__))); }

#define DYN_CONCEPT_DYN_FF_IMPL_(index, name, qualifier, return_type, ...)\
    public:\
        template<::std::size_t> struct conv; \
        template<> struct conv<index> { \
        using prototype = \
            return_type(DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_EXPAND_PARAMETER_TYPE_,__VA_ARGS__));\
        template<typename T, typename...Args> \
            constexpr decltype(auto) operator()(T &&impl, Args&&...args) \
                { return name(static_cast<T&&>(impl), static_cast<Args&&>(args)...); } }; \
        constexpr return_type name(DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_EXPAND_PARAMETER_,__VA_ARGS__)) qualifier \
        { return self::dynamic::template invoke<typename self::template conv<index>>(this \
                __VA_OPT__(,DYN_CONCEPT_OP_TABLE_(DYN_CONCEPT_FORWARD_W_,__VA_ARGS__))); }

#define DYN_CONCEPT_DYN_TRANSFER_CONV_IMPL_(base, index) typename base::template conv<index>
#define DYN_CONCEPT_DYN_TRANSFER_CONV_(value) DYN_CONCEPT_DYN_TRANSFER_CONV_IMPL_ value
#define DYN_CONCEPT_DYN_TRANSFER_IMPL_(base, name, qualifier)\
    public: \
        template<typename...Args>\
        constexpr decltype(auto) name(Args&&...args) qualifier \
        { return self::dynamic::template invoke<DYN_CONCEPT_DYN_TRANSFER_CONV_(base)>(this, static_cast<Args&&>(args)...); }

// Define member functionality, macro can be replace by ','.
// Pattern should like:
//  DYN_FN(<func index> NAMED <func name> WITH [qualifier] RETURN <return type> TAKE (<argument's name> NAMED <type>))
// N--------------------A
// <func name>: must samed with impl's.
#define DYN_FN(...) DYN_CONCEPT_DYN_FN_IMPL_(__VA_ARGS__)
// Define friend functionality, macro can be replace by ','.
// Pattern should like:
//  DYN_FF(<func index> NAMED <func name> WITH [qualifier] RETURN <return type> TAKE (<argument's name> NAMED <type>))
// N--------------------A
// <func name>: must samed with impl's.
#define DYN_FF(...) DYN_CONCEPT_DYN_FF_IMPL_(__VA_ARGS__)
// Inherit functionality from base, macro can be replace by ','.
// Pattern should like:
//  DYN_TO((<base type>, <func index>) NAMED <func name> WITH [qualifier])
// N--------------------A
// <func name>: not required to be samed with impl's
#define DYN_TO(...) DYN_CONCEPT_DYN_TRANSFER_IMPL_(__VA_ARGS__)
// Define friend functionality, 'EXTENDS(...)' can be replace by ', (...)'.
// Pattern should like:
//  DYN_CC(<interface name> EXTENDS([base type...]))
#define DYN_CC(...) DYN_CONCEPT_DYN_REQUIRES_IMPL_(__VA_ARGS__)

#if !defined(DYN_CONCEPT_NO_NETURAL_LANG_PRIMITIVE)
#  if defined(MUST) || defined(UNADORNED) || defined(RETURN) || defined(TAKE) || defined(EXTENDS)
#    warning "Some MACRO have same naming with dynamic-concept's, the behavior will be undefined."
#  endif
// [FROM DYNAMIC-CONCEPT] Mark that next part are qualifiers of functionality.
#  define MUST ,
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
