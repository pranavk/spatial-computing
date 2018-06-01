#ifndef TOKEN_H
#define TOKEN_H

#include "systemc.h"

class Token {

    // Data
    public:
    int data;
    int tag;
    sc_uint<32> address;
    int port;

    // Methods
    public:
    Token(void);
    Token(const Token& v);
    ~Token(void);
    Token& operator = (const Token& v);
    bool operator == (const Token& v) const;
    bool operator != (const Token& v) const;
    friend ostream& operator << (ostream& os, Token& v);
    friend void sc_trace(sc_trace_file* tf, const Token& v,
	const std::string& name);
};

#endif
