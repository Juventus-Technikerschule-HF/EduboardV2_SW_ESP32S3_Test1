#pragma once

#include <stdlib.h>
#include <stdbool.h>

#define RTC_ALARM_DEACTIVATED -1

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

typedef enum {
    RTCALARM_DISABLED,
    RTCALARM_ENABLED,
}rtc_alarm_mode_t;

typedef enum {
    RTCTIMER_DISABLED,
    RTCTIMER_ENABLED,
}rtc_timer_mode_t;

typedef enum {
    RTCFREQ_4096HZ = 0x00,
    RTCFREQ_64HZ = 0x01,
    RTCFREQ_1HZ = 0x02,
    RTCFREQ_1MIN = 0x03,
} rtc_timer_frequency_t;


void eduboard_init_rtc();

void rtc_setAlarmTime(int8_t hour, int8_t minute, int8_t day, int8_t weekday);
void rtc_configAlarm(rtc_alarm_mode_t mode, SemaphoreHandle_t alarm_semaphore);

void rtc_setTimerTime(uint8_t value);
void rtc_configTimer(rtc_timer_mode_t mode, SemaphoreHandle_t timer_semaphore, rtc_timer_frequency_t frequency_mode);

bool rtc_alarm_occured();
bool rtc_timer_elapsed();

void rtc_setTime(uint8_t hour, uint8_t minute, uint8_t sec);
void rtc_setDate(uint8_t day, uint8_t weekday, uint8_t month, uint16_t year);
void rtc_getTime(uint8_t* hour, uint8_t* minute, uint8_t* sec);
void rtc_getDate(uint16_t* year, uint8_t* month, uint8_t* day, uint8_t* weekday);
uint32_t rtc_getUnixTimestamp();