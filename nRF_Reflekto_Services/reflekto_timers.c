#include "reflekto_timers.h"
#include "app_timer.h"
#include "app_error.h"
#include "SEGGER_RTT.h"
#include "reflekto_ble_services.h"

APP_TIMER_DEF(clock_timer);
APP_TIMER_DEF(disconnect_timer);
#define TIME_TO_CLEAR 25 // Time to clear the screen in seconds

#define CLOCK_INTERVAL APP_TIMER_TICKS(1000)
#define DISCONNECT_INTERVAL APP_TIMER_TICKS(2000)

extern time_t current_unix_seconds;

static void test_time_conv(time_t unix_time){
    struct tm* local_time=localtime(&unix_time);
    SEGGER_RTT_printf(0, " %d %d %d\n",local_time->tm_hour,local_time->tm_min,local_time->tm_sec);
}

static void clock_timer_handler(void * p_context)
{
    extern time_t current_unix_seconds;
    current_unix_seconds++;
    test_time_conv(current_unix_seconds);
}

static void disconnect_timer_handler(void * p_context)
{
    disconnect_peripheral();
    has_permission_to_write = false;
}

void timers_init(void)
{
    // Initialize timer module.
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    // Create timers.
    err_code = app_timer_create(&clock_timer,APP_TIMER_MODE_REPEATED, clock_timer_handler);
    err_code = app_timer_create(&disconnect_timer,APP_TIMER_MODE_REPEATED, disconnect_timer_handler);
    APP_ERROR_CHECK(err_code);
    /* For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by one.*/
}

void disconnect_timer_start(void)
{
    ret_code_t err_code;
    err_code = app_timer_start(disconnect_timer, DISCONNECT_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

void disconnect_timer_stop(void)
{
    app_timer_stop(disconnect_timer);
}

void set_current_time(time_t new_time)
{
    ret_code_t err_code;
    app_timer_stop(clock_timer);
    current_unix_seconds = new_time;
    err_code = app_timer_start(clock_timer, CLOCK_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}