// The example below is for using embed macro.
// This header is for virtual interface like interface definition.
#include "dyn_cc.hpp"

// struct or class is ok.
struct input_streamable
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
    // TODO: intellsense sometime doesn't recognize what is `self::dynamic'.

    // Or if you encounting with error "unclear 'some function'" 
    // change to use `DYN_OF` instead of `DYN_FN` for overload function.
    // \/
    DYN_CC(input_streamable EXTENDS());
};

struct stream
{
    DYN_FN(0 NAMED operator>> WITH RETURN bool TAKE (output NAMED char*));
    // this transfer to 'input_streamable::operator<<'.
    // explicit specify it to enable.
    // \/ 
    DYN_TO((input_streamable, 0) NAMED operator<< WITH noexcept);
    // Btw, if you wanna transfer to input_streamable, you must
    // EXTEND(input_streamable)
    // -----------\/
    DYN_CC(stream EXTENDS(input_streamable));
};


// TEST.

#include <iostream>

struct dummy_stream
{
    int operator<<(char const *what) 
    {
        ::std::cout << "Received message: " << what << ::std::endl;
        return int(::std::strlen(what));
    }
    int operator<<(char what) 
    {
        return 0;
    }
    int operator>>(char *where)
    {
        auto &value = "Hello world!";
        if (where) 
            ::std::memcpy(where, value, sizeof(value));
        return sizeof(value);
    }
};

void dosth(dyn::view<stream> view)
{
    auto &value = "Hello world!";
    view << value;
    char rec[sizeof(value)];
    view >> rec;
    ::std::cout << "Receive:" << rec << "\n";
}

int main()
{
    dummy_stream stream;
    dosth(stream);
}