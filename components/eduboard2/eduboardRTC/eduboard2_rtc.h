#pragma once

#include <stdlib.h>
#include <stdbool.h>

enum rtc_day_names {
    SUNDAY = 0,
    MONDAY = 1,
    TUESDAY = 2,
    WEDNESDAY = 3,
    THRUSDAY = 4,
    FRIDAY = 5,
    SATURDAY = 6,
};

enum rtc_month_names {
    JANUARY = 1,
    FEBRUARY = 2,
    MARCH = 3,
    APRIL = 4,
    MAY = 5,
    JUNE = 6,
    JULY = 7,
    AUGUST = 8,
    SEPTEMBER = 9,
    OCTOBER = 10,
    NOVEMBER = 11,
    DECEMBER = 12,
};


void eduboard_init_rtc();

void rtc_setTime(uint8_t hour, uint8_t minute, uint8_t sec);
void rtc_setDate(uint8_t day, uint8_t weekday, uint8_t month, uint16_t year);
void rtc_getTime(uint8_t* hour, uint8_t* minute, uint8_t* sec);
void rtc_getDate(uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* weekday);
uint32_t rtc_getUnixTimestamp();