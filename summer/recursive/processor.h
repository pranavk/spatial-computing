#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "systemc.h"
#include "ash.h"
#include "token.h"

class PROCESSOR:
public sc_module {

    public:

    // Ports
    sc_port < sc_fifo_in_if< Token > > input;
    sc_port < sc_fifo_out_if < Token > > output;
    sc_in<bool> clk;

    // Process declaration
    void read_thread(void);
    void compute_thread(void);

    // Internal data
    Matched_Data matched_data[256];
    Token input_data;
    Token left_output_data;
    Token right_output_data;
    bool state;
    int value;
    int function;
    int tag;

    // Events
    sc_event event_read;

    // Module constructor
    SC_HAS_PROCESS(PROCESSOR);
    PROCESSOR (sc_module_name name, int val, int func, sc_uint<32> address_left,
	int port_left, sc_uint<32> address_right, int port_right):
    sc_module (name) {
	SC_THREAD(read_thread);
	sensitive << clk.neg();
	SC_THREAD(compute_thread);
	sensitive << clk.neg();
	for (int i = 0; i < 256; i++) {
	    matched_data[i].left_valid = false;
	    matched_data[i].right_valid = false;
	    matched_data[i].predicate_valid = false;
	}
	left_output_data.address = address_left;
	left_output_data.port = port_left;
	right_output_data.address = address_right;
	right_output_data.port = port_right;
	state = true;
	value = val;
	function = func;
    }
};

#endif
