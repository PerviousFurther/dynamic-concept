// The example below is for using embed macro.
// This header is for virtual interface like interface definition.
#include "dyn_cc.hpp"

// struct or class is ok.
DYN_STRUCT(input_streamable)
{
    // NAMED, WITH and RETURN, these macro just an alias of ','.

    // Remember to add 'NAMED', or directly ',' if you like...but we recommand use 'NAMED'.
    //----------------------------------------------------\/ 
    DYN_FN(0 NAMED operator<< WITH RETURN int TAKE (input NAMED char const*));
    DYN_FN(1 NAMED operator<< WITH RETURN int TAKE (input NAMED char));
    // /\
    // If you encounting error reporting from intellsense with "return type mismatch", 
    // generally, this because you forget to place function in `dyn_cc`.
    // 
    // TODO: intellsense sometime doesn't recognize `self::dynamic'.

    // Or if you encounting with error "unclear 'some function'" 
    // change to use `DYN_OF` instead of `DYN_FN` for overload function.
    // \/
    DYN_CC();
};

DYN_STRUCT(stream)
{
    DYN_FN(0 NAMED operator>> WITH RETURN bool TAKE (output NAMED char*));
    // this transfer to 'input_streamable::operator<<'.
    // explicit specify it to enable. // noexcept is optional.
    // \/                             \                \/
    DYN_TO((input_streamable, 0) NAMED operator<< WITH noexcept);

    // Define friend operator.
    // Library use different strategy done with 'at right' friend 
    // operator. only friend operator can have these
    // operation.                         ///
    // \/                                 \/ conversion from implmentation to interface is allowed.    
    DYN_FO(1 NAMED operator>> WITH RETURN long TAKE (output NAMED const char*));

    // Btw, if you wanna transfer to input_streamable or as to it, you must place input_streamable
    // ----\/
    DYN_CC(input_streamable);
};

// TEST.

#include <iostream>
#include <cstring>

auto &value = "Hello world!";
struct dummy_stream
{
    int operator<<(char const *what) 
    {
        ::std::cout << "Received message: " << what << ::std::endl;
        return int(::std::strlen(what));
    }
    int operator<<(char c) 
    {
        ::std::cout << "Received char: " << c << ::std::endl; 
        return 1;
    }
    //\/ 'int' is not same with interface's 'long'.
    int operator>>(char *where)
    {
        if (where) ::std::memcpy(where, value, sizeof(value));
        return sizeof(value);
    }
    friend int operator>>(const char *value, dummy_stream &self)
    {
        ::std::cout << "Received message by friend: " << value << ::std::endl;
        return int(::std::strlen(value));
    }
};

void dosth(dyn::view<stream> view)
{
    view << value;
    char rec[sizeof(value)];
    view >> rec;
    ::std::cout << "Receive:" << rec << "\n";
    value >> view;
    view.as<input_streamable>() << '5';
}

int main()
{
    dummy_stream stream;
    dosth(stream);
}

