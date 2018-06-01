#include "token.h"

Token::
Token(void) {
}

Token::
Token(const Token& v) {
    data = v.data;
    tag = v.tag;
    address = v.address;
    port = v.port;
}

Token::
~Token(void) {
}

Token&
Token::
operator = (const Token& v) {
    data = v.data;
    tag = v.tag;
    address = v.address;
    port = v.port;
    return(*this);
}

bool
Token::
operator == (const Token& v) const {
    bool result = true;
    result = result && (data == v.data);
    result = result && (tag == v.tag);
    result = result && (address == v.address);
    result = result && (port == v.port);
    return(result);
}

bool
Token::
operator != (const Token& v) const {
    return( ! ( operator == (v) ) );
}

ostream&
operator << (ostream& os, Token& v) {
    os << "Data: " << v.data << endl;
    os << "Tag: " << v.tag << endl;
    os << "Address: " << v.address << endl;
    os << "Port: " << v.port << endl;
    return(os);
}

void
sc_trace(sc_trace_file* tf, const Token& v, const std::string& name) {
    sc_trace(tf, v.data, name + ".data");
    sc_trace(tf, v.tag, name + ".tag");
    sc_trace(tf, v.address, name + ".address");
    sc_trace(tf, v.port, name + ".port");
}
