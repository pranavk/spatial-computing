#ifndef ASH_H
#define ASH_H

#include "systemc.h"

typedef struct {
    int left_data;
    int right_data;
    int predicate_data;
    int left_tag;
    int right_tag;
    int predicate_tag;
    bool left_valid;
    bool right_valid;
    bool predicate_valid;
} Matched_Data;

#define TRIGGER 0
#define CONSTANT 1
#define ALPHA 2
#define BETA 3
#define GAMMA 4
#define DELTA 5
#define EPSILON 6
#define COMPARE 7
#define COPY 8
#define SUBTRACT 9
#define MULTIPLY 10
#define TAG 11
#define UNTAG 12
#define SINK 13

#endif
