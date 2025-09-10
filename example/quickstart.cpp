// The example below is for using embed macro.
// Define this header is for virtual interface like interface definition.
// 
#define DYNAMIC_CONCEPT_INTERFACE
#include "dynamic_concept.hpp"

// struct or class is ok.
struct input_streamable
{
    // Or if you encounting with error "unclear 'some function'" 
    // change to use `DYN_OF` instead of `DYN_FN` for overload function.
    // \/
    DYN_CC();

    // NAMED, WITH and RETURN, these macro just an alias of ','.

    // Remember to add 'NAMED', or directly ',' if you like...but we recommand use 'NAMED'.
    //----------------------------------------------------\/ 
    DYN_FN(0 NAMED operator<< WITH RETURN int TAKE (input NAMED char const*));
    DYN_FN(1 NAMED operator<< WITH RETURN int TAKE (input NAMED char));
    // /\
    // If you encounting error reporting from intellsense with "return type mismatch", 
    // generally, this because you forget to place function in `dyn_cc`.
};

struct stream
{
    // Btw, if you wanna transfer to input_streamable or as to it, you must place input_streamable
    // ----\/
    DYN_CC(input_streamable);

    DYN_FN(0 NAMED operator>> WITH RETURN bool TAKE (output NAMED char*));
    // this transfer to 'input_streamable::operator<<'.
    // explicit specify it to enable. // noexcept is optional.
    // \/                             \                \/
    DYN_TO((input_streamable, 0) NAMED operator<< WITH noexcept);
    // Define friend function.
    // Library use different strategy done with 'at right' friend 
    // operator. only friend operator can have these
    // operation.                    // conversion from implmentation 
    // \/                            \/ to interface is allowed.          \/ mark position is stream. no need to name it.
    DYN_FF(1 NAMED send WITH RETURN long TAKE (output NAMED const char*), (stream));
};

// TEST.

#include <iostream>
#include <cstring>

auto &value = "Hello world!";
struct dummy_stream
{
    ~dummy_stream() { ::std::cout << "Destructed in " << scope_ << "\n"; }

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
    friend int send(const char *value, dummy_stream &self)
    {
        ::std::cout << "Received message by friend: " << value << ::std::endl;
        return int(::std::strlen(value));
    }

    const char *scope_{"Undefined"};
};

void dosth(dyn::view<stream> view)
{
    view << value; // call input_stream operator<<.
    char rec[sizeof(value)];
    view >> rec; // call stream operator>>
    ::std::cout << "Receive:" << rec << "\n";
    // call stream friend function send.
    send(value, view);
    // call input 
    view.as<input_streamable>() << '5';
}

void dosth(dyn::box<stream> bx)
{
    bx << value;
    bx.as<input_streamable>() << '5'; // become view.
    // then outof socpe, it would destructed.
}

int main()
{
    dummy_stream stream;
    dosth(stream);
    dosth({::std::in_place_type<dummy_stream>, "Dth scope."});
    return 0;
}

