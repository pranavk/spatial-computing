#ifndef ROUTER_H
#define ROUTER_H

#include "systemc.h"
#include "ash.h"
#include "token.h"

class ROUTER:
public sc_module {

    public:

    // Ports
    sc_port < sc_fifo_in_if< Token > > west_input;
    sc_port < sc_fifo_in_if< Token > > north_input;
    sc_port < sc_fifo_in_if< Token > > processor_input;
    sc_port < sc_fifo_out_if< Token > > east_output;
    sc_port < sc_fifo_out_if< Token > > south_output;
    sc_port < sc_fifo_out_if< Token > > processor_output;
    sc_in<bool> clk;

    // Process declaration
    void read_west_input_thread(void);
    void read_north_input_thread(void);
    void read_processor_input_thread(void);
    void compute_west_route_thread(void);
    void compute_north_route_thread(void);
    void compute_processor_route_thread(void);

    // Internal data
    Token west_input_data;
    Token north_input_data;
    Token processor_input_data;
    sc_uint<16> x_address;
    sc_uint<16> y_address;

    // Events
    sc_event event_read_west_input;
    sc_event event_read_north_input;
    sc_event event_read_processor_input;

    // Module constructor
    SC_HAS_PROCESS (ROUTER);
    ROUTER ( sc_module_name name, sc_uint<16> i, sc_uint<16> j):
    sc_module (name) {
	SC_THREAD(read_west_input_thread);
	sensitive << clk.neg();
	SC_THREAD(read_north_input_thread);
	sensitive << clk.neg();
	SC_THREAD(read_processor_input_thread);
	sensitive << clk.neg();
	SC_THREAD(compute_west_route_thread);
	sensitive << clk.neg();
	SC_THREAD(compute_north_route_thread);
	sensitive << clk.neg();
	SC_THREAD(compute_processor_route_thread);
	sensitive << clk.neg();
	x_address = i;
	y_address = j;
    }
};

#endif
