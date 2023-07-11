#pragma once

#include <stdio.h>

typedef struct 
{
    uint16_t x;
    uint16_t y;
    uint8_t strength;
    uint8_t area;
}touchpos_t;


typedef struct {
    uint8_t type;
    uint8_t touches;
    touchpos_t points[2];
} touchevent_t;

void eduboard_init_ft6236(void);