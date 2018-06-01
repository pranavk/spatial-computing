#include "router.h"

// Processes
void
ROUTER::
read_west_input_thread (void) {
    while (true) {
	west_input_data = west_input->read();
	event_read_west_input.notify();
	wait();
    }
}

void
ROUTER::
read_north_input_thread (void) {
    while (true) {
	north_input_data = north_input->read();
	event_read_north_input.notify();
	wait();
    }
}

void
ROUTER::
read_processor_input_thread (void) {
    while (true) {
	processor_input_data = processor_input->read();
	event_read_processor_input.notify();
	wait();
    }
}

void
ROUTER::
compute_west_route_thread (void) {
    while (true) {
	wait(event_read_west_input);
	if (west_input_data.address.range(15,0) == y_address) { 
	    if (west_input_data.address.range(31,16) == x_address) {
		wait();
		processor_output->write (west_input_data);
	    } else {  
		wait();
		south_output->write (west_input_data);
	    }
	} else {
	    wait();
	    east_output->write (west_input_data);
	}
    } 
}

void
ROUTER::
compute_north_route_thread (void) {
    while (true) {
	wait(event_read_north_input);
	if (north_input_data.address.range(15,0) == y_address) { 
	    if (north_input_data.address.range(31,16) == x_address){
		wait();
		processor_output->write (north_input_data);
	    } else {  
		wait();
		south_output->write (north_input_data);
	    }
	} else {
	    wait();
	    east_output->write (north_input_data);
	}
    } 
}

void
ROUTER::
compute_processor_route_thread (void) {
    while (true) {
	wait(event_read_processor_input);
	if (processor_input_data.address.range(15,0) == y_address) { 
	    if (processor_input_data.address.range(31,16) == x_address) {
		wait();
		processor_output-> write (processor_input_data);
	    } else {  
		wait();
		south_output->write (processor_input_data);
	    }
	} else {
	    wait();
	    east_output->write (processor_input_data);
	}
    }
}
