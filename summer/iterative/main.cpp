#include"systemc.h"
#include"ash.h"
#include"token.h"
#include"router.h"
#include"processor.h"

int
sc_main(int argc, char *argv[]) {

    char stdbuf[256];

    sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);

    // Signals (Data)
    int i;
    int j;
    sc_fifo<Token>* fifo_router_east[4][8];
    sc_fifo<Token>* fifo_router_south[4][8];
    sc_fifo<Token>* fifo_router_processor[4][8];
    sc_fifo<Token>* fifo_processor_router[4][8];
    for (i = 0; i < 4; i++) {
	for (j = 0; j < 8; j++) {
	    fifo_router_east[i][j] = new sc_fifo<Token>(8);
	    fifo_router_south[i][j] = new sc_fifo<Token>(8);
	    fifo_router_processor[i][j] = new sc_fifo<Token>(8);
	    fifo_processor_router[i][j] = new sc_fifo<Token>(8);
	}
    }

    // Modules
    ROUTER* router[4][8];
    PROCESSOR* processor[4][8];
    for (i = 0; i < 4; i++) {
	for (j = 0; j < 8; j++) {
	    sprintf(stdbuf, "router[%d][%d]", i, j);
	    router[i][j] = new ROUTER (stdbuf, i, j);
	}
    }
    sprintf(stdbuf, "processor[%d][%d]", 0, 0);
    processor[0][0] = new PROCESSOR (stdbuf, 0, TRIGGER, 0x00000001, 0,
	0x00000006, 0);
    sprintf(stdbuf, "processor[%d][%d]", 0, 1);
    processor[0][1] = new PROCESSOR (stdbuf, 3, CONSTANT, 0x00000002, 0,
	0x00000005, 1);
    sprintf(stdbuf, "processor[%d][%d]", 0, 2);
    processor[0][2] = new PROCESSOR (stdbuf, 2, EPSILON, 0x00000002, 1,
	0x00000004, 1);
    sprintf(stdbuf, "processor[%d][%d]", 0, 3);
    processor[0][3] = new PROCESSOR (stdbuf, 3, SINK, 0x00000003, 1,
	0x00000003, 1);
    sprintf(stdbuf, "processor[%d][%d]", 0, 4);
    processor[0][4] = new PROCESSOR (stdbuf, 4, SINK, 0x00000004, 1,
	0x00000004, 1);
    sprintf(stdbuf, "processor[%d][%d]", 0, 5);
    processor[0][5] = new PROCESSOR (stdbuf, 5, SINK, 0x00000005, 1,
	0x00000005, 1);
    sprintf(stdbuf, "processor[%d][%d]", 0, 6);
    processor[0][6] = new PROCESSOR (stdbuf, 5, CONSTANT, 0x00000007, 1,
	0x00030001, 1);
    sprintf(stdbuf, "processor[%d][%d]", 0, 7);
    processor[0][7] = new PROCESSOR (stdbuf, 7, BETA, 0x00010000, 1,
	0x00010003, 1);
    sprintf(stdbuf, "processor[%d][%d]", 1, 0);
    processor[1][0] = new PROCESSOR (stdbuf, 8, COMPARE, 0x00010001, 1,
	0x00030000, 1);
    sprintf(stdbuf, "processor[%d][%d]", 1, 1);
    processor[1][1] = new PROCESSOR (stdbuf, 9, COPY, 0x00010002, 1,
	0x00020003, 1);
    sprintf(stdbuf, "processor[%d][%d]", 1, 2);
    processor[1][2] = new PROCESSOR (stdbuf, 10, COPY, 0x00000007, 0,
	0x00010003, 0);
    sprintf(stdbuf, "processor[%d][%d]", 1, 3);
    processor[1][3] = new PROCESSOR (stdbuf, 11, ALPHA, 0x00010004, 1,
	0x00020002, 1);
    sprintf(stdbuf, "processor[%d][%d]", 1, 4);
    processor[1][4] = new PROCESSOR (stdbuf, 12, SUBTRACT, 0x00010005, 1,
	0x00000007, 2);
    sprintf(stdbuf, "processor[%d][%d]", 1, 5);
    processor[1][5] = new PROCESSOR (stdbuf, 13, MULTIPLY, 0x00010006, 2,
	0x00020001, 1);
    sprintf(stdbuf, "processor[%d][%d]", 1, 6);
    processor[1][6] = new PROCESSOR (stdbuf, 14, BETA, 0x00010007, 1,
	0x00020000, 0);
    sprintf(stdbuf, "processor[%d][%d]", 1, 7);
    processor[1][7] = new PROCESSOR (stdbuf, 15, ALPHA, 0x00010005, 2,
	0x00000002, 1);
    sprintf(stdbuf, "processor[%d][%d]", 2, 0);
    processor[2][0] = new PROCESSOR (stdbuf, 16, SINK, 0x00020000, 1,
	0x00020000, 1);
    sprintf(stdbuf, "processor[%d][%d]", 2, 1);
    processor[2][1] = new PROCESSOR (stdbuf, 17, SINK, 0x00020001, 1,
	0x00020001, 1);
    sprintf(stdbuf, "processor[%d][%d]", 2, 2);
    processor[2][2] = new PROCESSOR (stdbuf, 18, SINK, 0x00020002, 1,
	0x00020002, 1);
    sprintf(stdbuf, "processor[%d][%d]", 2, 3);
    processor[2][3] = new PROCESSOR (stdbuf, 19, COPY, 0x00020004, 0,
	0x00010007, 0);
    sprintf(stdbuf, "processor[%d][%d]", 2, 4);
    processor[2][4] = new PROCESSOR (stdbuf, 20, ALPHA, 0x00020005, 1,
	0x00020007, 1);
    sprintf(stdbuf, "processor[%d][%d]", 2, 5);
    processor[2][5] = new PROCESSOR (stdbuf, 21, COPY, 0x00010004, 2,
	0x00020006, 2);
    sprintf(stdbuf, "processor[%d][%d]", 2, 6);
    processor[2][6] = new PROCESSOR (stdbuf, 22, BETA, 0x00010000, 2,
	0x00020004, 1);
    sprintf(stdbuf, "processor[%d][%d]", 2, 7);
    processor[2][7] = new PROCESSOR (stdbuf, 23, SINK, 0x00020007, 1,
	0x00020007, 1);
    sprintf(stdbuf, "processor[%d][%d]", 3, 0);
    processor[3][0] = new PROCESSOR (stdbuf, 24, COPY, 0x00010006, 0,
	0x00020006, 0);
    sprintf(stdbuf, "processor[%d][%d]", 3, 1);
    processor[3][1] = new PROCESSOR (stdbuf, 25, COPY, 0x00010006, 1,
	0x00030002, 0);
    sprintf(stdbuf, "processor[%d][%d]", 3, 2);
    processor[3][2] = new PROCESSOR (stdbuf, 1, CONSTANT, 0x00020006, 1,
	0x00030003, 1);
    sprintf(stdbuf, "processor[%d][%d]", 3, 3);
    processor[3][3] = new PROCESSOR (stdbuf, 27, SINK, 0x00030003, 1,
	0x00030003, 1);
    sprintf(stdbuf, "processor[%d][%d]", 3, 4);
    processor[3][4] = new PROCESSOR (stdbuf, 28, SINK, 0x00030004, 1,
	0x00030004, 1);
    sprintf(stdbuf, "processor[%d][%d]", 3, 5);
    processor[3][5] = new PROCESSOR (stdbuf, 29, SINK, 0x00030005, 1,
	0x00030005, 1);
    sprintf(stdbuf, "processor[%d][%d]", 3, 6);
    processor[3][6] = new PROCESSOR (stdbuf, 30, SINK, 0x00030006, 1,
	0x00030006, 1);
    sprintf(stdbuf, "processor[%d][%d]", 3, 7);
    processor[3][7] = new PROCESSOR (stdbuf, 31, SINK, 0x00030007, 1,
	0x00030007, 1);

    sc_clock clk("Clock", 10, 0.5, 0, false);               // Clock

    // Connections
    for (i = 0; i < 4; i++) {
	for (j = 0; j < 8; j++) {
	    router[i][j]-> west_input(*fifo_router_east[i][(j+7)%8]);
	    router[i][j]-> north_input(*fifo_router_south[(i+3)%4][j]);
	    router[i][j]-> processor_input(*fifo_processor_router[i][j]);
	    router[i][j]-> east_output(*fifo_router_east[i][j]);
	    router[i][j]-> south_output(*fifo_router_south[i][j]);
	    router[i][j]-> processor_output(*fifo_router_processor[i][j]);
	    router[i][j]->clk(clk);
	    processor[i][j]-> input(*fifo_router_processor[i][j]);
	    processor[i][j]-> output(*fifo_processor_router[i][j]);
	    processor[i][j]->clk(clk);
	}
    }

    // Tracing
    sc_trace_file* tf;
    tf = sc_create_vcd_trace_file("ash");

    sc_trace(tf, clk, "Clock");
    sc_trace(tf, processor[0][1]->input_data, "Constant_01_In");
    sc_trace(tf, processor[0][2]->input_data, "Epsilon_02_In");
    sc_trace(tf, processor[0][3]->input_data, "Sink_03_In");
    sc_trace(tf, processor[0][4]->input_data, "Sink_04_In");
    sc_trace(tf, processor[0][5]->input_data, "Sink_05_In");
    sc_trace(tf, processor[0][6]->input_data, "Constant_06_In");
    sc_trace(tf, processor[0][7]->input_data, "Beta_07_In");
    sc_trace(tf, processor[1][0]->input_data, "Compare_10_In");
    sc_trace(tf, processor[1][1]->input_data, "Copy_11_In");
    sc_trace(tf, processor[1][2]->input_data, "Copy_12_In");
    sc_trace(tf, processor[1][3]->input_data, "Alpha_13_In");
    sc_trace(tf, processor[1][4]->input_data, "Subtract_14_In");
    sc_trace(tf, processor[1][5]->input_data, "Multiply_15_In");
    sc_trace(tf, processor[1][6]->input_data, "Beta_16_In");
    sc_trace(tf, processor[1][7]->input_data, "Alpha_17_In");
    sc_trace(tf, processor[2][0]->input_data, "Sink_20_In");
    sc_trace(tf, processor[2][1]->input_data, "Sink_21_In");
    sc_trace(tf, processor[2][2]->input_data, "Sink_22_In");
    sc_trace(tf, processor[2][3]->input_data, "Copy_23_In");
    sc_trace(tf, processor[2][4]->input_data, "Alpha_24_In");
    sc_trace(tf, processor[2][5]->input_data, "Copy_25_In");
    sc_trace(tf, processor[2][6]->input_data, "Beta_26_In");
    sc_trace(tf, processor[2][7]->input_data, "Sink_27_In");
    sc_trace(tf, processor[3][0]->input_data, "Copy_30_In");
    sc_trace(tf, processor[3][1]->input_data, "Copy_31_In");
    sc_trace(tf, processor[3][2]->input_data, "Constant_32_In");
    sc_trace(tf, router[3][2]->processor_input_data, "Constant_32_Out");
    sc_trace(tf, processor[3][3]->input_data, "Sink_33_In");

    sc_start(3000, SC_NS);

    sc_close_vcd_trace_file(tf);

    return 0;
}
