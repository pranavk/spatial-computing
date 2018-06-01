#include "processor.h"

// Processes
void
PROCESSOR::
read_thread (void) {
    while (true) {
	if (function != TRIGGER) {
		
	    input_data = input->read();
	    int i = input_data.tag%256;
	    switch (input_data.port) {

		case 0:
		matched_data[i].predicate_data = input_data.data;
		matched_data[i].predicate_tag = input_data.tag;
		matched_data[i].predicate_valid = true;
		break;

		case 1:
		matched_data[i].left_data = input_data.data;
		matched_data[i].left_tag = input_data.tag;
		matched_data[i].left_valid = true;
		break;

		case 2:
		matched_data[i].right_data = input_data.data;
		matched_data[i].right_tag = input_data.tag;
		matched_data[i].right_valid = true;
		break;
	    }
	    event_read.notify();
	}
	wait();
    }
}

void
PROCESSOR::
compute_thread (void) {
    if (function == TRIGGER) {
	left_output_data.data = 1;
	left_output_data.tag = 0;
	right_output_data.data = 1;
	right_output_data.tag = 0;
	output->write(left_output_data);
	wait();
	output->write(right_output_data);
	wait();
    } else {
	while (true) {
	    wait(event_read);
	    switch (function) {

		case CONSTANT:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].predicate_valid) { 
			left_output_data.data = value;
			left_output_data.tag = matched_data[i].predicate_tag;
			right_output_data.data = value;
			right_output_data.tag = matched_data[i].predicate_tag;
			matched_data[i].predicate_valid = false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    }
		}
		break;

		case ALPHA:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid &&
		    matched_data[i].predicate_valid && state == true &&
		    matched_data[i].predicate_data == 1) {
			left_output_data.data = matched_data[i].left_data;
			left_output_data.tag = matched_data[i].left_tag;
			tag = matched_data[i].left_tag;
			state = false;
			matched_data[i].predicate_valid = false;
			matched_data[i].left_valid = false;
			output->write(left_output_data);
			wait();
			break;
		    } else if (matched_data[i].left_valid &&
		    matched_data[i].predicate_valid && state == true &&
		    matched_data[i].predicate_data == 0) {
			right_output_data.data = matched_data[i].left_data;
			right_output_data.tag = matched_data[i].left_tag;
			matched_data[i].predicate_valid = false;
			matched_data[i].left_valid = false;
			output->write(right_output_data);
			wait();
			break;
		    } else if (matched_data[i].left_valid &&
		    matched_data[i].predicate_valid && state == false &&
		    matched_data[i].predicate_data == 1) {
			left_output_data.data = matched_data[i].left_data;
			left_output_data.tag = matched_data[i].left_tag;
			matched_data[i].predicate_valid = false;
			matched_data[i].left_valid = false;
			output->write(left_output_data);
			wait();
			break;
		    } else if (matched_data[i].left_valid &&
		    matched_data[i].predicate_valid && state == false &&
		    matched_data[i].predicate_data == 0) {
			right_output_data.data = matched_data[i].left_data;
			right_output_data.tag = tag;
			matched_data[i].predicate_valid = false;
			matched_data[i].left_valid = false;
			state = true;
			output->write(right_output_data);
			wait();
			break;
		    }
		}
		break;

		case BETA:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid && 
		    state == true) {
			state = false;
			left_output_data.data = matched_data[i].left_data;
			left_output_data.tag = matched_data[i].left_tag;
			right_output_data.data = matched_data[i].left_data;
			right_output_data.tag = matched_data[i].left_tag;
			matched_data[i].left_valid = false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    } else if (matched_data[i].predicate_valid &&
		    matched_data[i].right_valid &&
		    matched_data[i].predicate_data == 1 && state == false) {
			left_output_data.data = matched_data[i].right_data;
			left_output_data.tag = matched_data[i].right_tag + 1;
			right_output_data.data = matched_data[i].right_data;
			right_output_data.tag = matched_data[i].right_tag + 1;
			matched_data[i].right_valid = false;
			matched_data[i].predicate_valid	= false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    } else if (matched_data[i].predicate_valid && 
		    matched_data[i].right_valid &&
		    matched_data[i].predicate_data == 0 && state == false) {
			state = true;
			matched_data[i].right_valid = false;
			matched_data[i].predicate_valid = false;
			wait();
			break;
		    }
		}
		break;

		case GAMMA:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid &&
		    matched_data[i].predicate_valid &&
		    matched_data[i].predicate_data == 1) {
			left_output_data.data = matched_data[i].left_data;
			left_output_data.tag = matched_data[i].left_tag;
			right_output_data.data = matched_data[i].left_data;
			right_output_data.tag = matched_data[i].left_tag;
			matched_data[i].left_valid = false;
			matched_data[i].predicate_valid = false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    } else if (matched_data[i].right_valid &&
		    matched_data[i].predicate_valid &&
		    matched_data[i].predicate_data == 0) {
			left_output_data.data = matched_data[i].right_data;
			left_output_data.tag = matched_data[i].right_tag;
			right_output_data.data = matched_data[i].right_data;
			right_output_data.tag = matched_data[i].right_tag;
			matched_data[i].right_valid = false;
			matched_data[i].predicate_valid = false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    }
		}
		break;

		case DELTA:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid &&
		    matched_data[i].predicate_valid &&
		    matched_data[i].predicate_data == 0) {
			left_output_data.data = matched_data[i].left_data;
			left_output_data.tag = matched_data[i].left_tag;
			output->write (left_output_data);
			matched_data[i].left_valid = false;
			matched_data[i].predicate_valid = false;
			output->write(left_output_data);
			wait();
			break;
		    } else if (matched_data[i].left_valid &&
		    matched_data[i].predicate_valid &&
		    matched_data[i].predicate_data == 1) {
			right_output_data.data = matched_data[i].left_data;
			right_output_data.tag = matched_data[i].left_tag;
			output->write (right_output_data);
			matched_data[i].left_valid = false;
			matched_data[i].predicate_valid = false;
			output->write(right_output_data);
			wait();
			break;
		    }
		}
		break;

		case EPSILON:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid &&
		    matched_data[i].predicate_valid) {
			left_output_data.data = matched_data[i].left_data;
			left_output_data.tag = matched_data[i].left_tag;
			left_output_data.address =
			    matched_data[i].predicate_data;
			right_output_data.data = matched_data[i].left_data;
			right_output_data.tag = matched_data[i].left_tag;
			right_output_data.address =
			    matched_data[i].predicate_data;
			matched_data[i].left_valid = false;
			matched_data[i].predicate_valid = false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    }
		}
		break;

		case COMPARE:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid && 
		    matched_data[i].right_valid) {
			if (matched_data[i].left_data >	
			matched_data[i].right_data) {
			    left_output_data.data = 1;
			    left_output_data.tag = matched_data[i].left_tag;
			    right_output_data.data = 1;
			    right_output_data.tag = matched_data[i].left_tag;
			    matched_data[i].left_valid = false;
			    matched_data[i].right_valid = false;
			    output->write(left_output_data);
			    wait();
			    output->write(right_output_data);
			    wait();
			} else {
			    left_output_data.data = 0;
			    left_output_data.tag = matched_data[i].left_tag;
			    right_output_data.data = 0;
			    right_output_data.tag = matched_data[i].left_tag;
			    matched_data[i].left_valid = false;
			    matched_data[i].right_valid = false;
			    output->write(left_output_data);
			    wait();
			    output->write(right_output_data);
			    wait();
			}
			break;
		    }
		}
		break;

		case COPY:
		    left_output_data.data = input_data.data;
		    left_output_data.tag = input_data.tag;
		    right_output_data.data = input_data.data;
		    right_output_data.tag = input_data.tag;
		    output->write(left_output_data);
		    wait();
		    output->write(right_output_data);
		    wait();
		    break;

		case SUBTRACT:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid &&
		    matched_data[i].right_valid) {
			left_output_data.data = matched_data[i].left_data - 
			    matched_data[i].right_data;
			left_output_data.tag = matched_data[i].left_tag;
			right_output_data.data = matched_data[i].left_data -
			    matched_data[i].right_data;
			right_output_data.tag = matched_data[i].left_tag;
			matched_data[i].left_valid = false;
			matched_data[i].right_valid = false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    }
		}
		break;

		case MULTIPLY:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid &&
		    matched_data[i].right_valid) {
			left_output_data.data = matched_data[i].left_data * 
			    matched_data[i].right_data;
			left_output_data.tag = matched_data[i].left_tag;
			right_output_data.data = matched_data[i].left_data *
			    matched_data[i].right_data;
			right_output_data.tag = matched_data[i].left_tag;
			matched_data[i].left_valid = false;
			matched_data[i].right_valid = false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    }
		}
		break;

		case TAG:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid) {
			left_output_data.data = matched_data[i].left_data;
			left_output_data.tag = matched_data[i].left_tag + 1;
			right_output_data.data = matched_data[i].left_data;
			right_output_data.tag = matched_data[i].left_tag + 1;
			matched_data[i].left_valid = false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    }
		}
		break;

		case UNTAG:
		for (int i = 0; i < 256; i++) {
		    if (matched_data[i].left_valid) {
			left_output_data.data = matched_data[i].left_data;
			left_output_data.tag = matched_data[i].left_tag - 1;
			right_output_data.data = matched_data[i].left_data;
			right_output_data.tag = matched_data[i].left_tag - 1;
			matched_data[i].left_valid = false;
			output->write(left_output_data);
			wait();
			output->write(right_output_data);
			wait();
			break;
		    }
		}
		break;

		case SINK:
		default:
		break;
	    }
	}
    }
}
