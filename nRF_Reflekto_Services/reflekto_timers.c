#include "reflekto_timers.h"
#include "app_timer.h"
#include "app_error.h"
#include "SEGGER_RTT.h"
#include "reflekto_ble_services.h"
#include "reflekto_display.h"

time_t current_unix_seconds;
struct tm* local_time;

APP_TIMER_DEF(clock_timer);
APP_TIMER_DEF(disconnect_timer);
APP_TIMER_DEF(clear_screen_timer);
#define TIME_TO_CLEAR 30 // Time to clear the screen in seconds

#define CLOCK_INTERVAL APP_TIMER_TICKS(1000)
#define DISCONNECT_INTERVAL APP_TIMER_TICKS(2000)
#define CLEAR_SCREEN_INTERVAL APP_TIMER_TICKS(TIME_TO_CLEAR*1000)

static void time_convert_and_update(){
    local_time = localtime(&current_unix_seconds);
    SEGGER_RTT_printf(0, " %d %d %d\n",local_time->tm_hour,local_time->tm_min,local_time->tm_sec);
    update_timer();
}

static void clock_timer_handler(void * p_context)
{
    current_unix_seconds++;
    time_convert_and_update();
}

static void disconnect_timer_handler(void * p_context)
{
    disconnect_peripheral();
    has_permission_to_write = false;
}

static void clear_screen_handler(void * p_context)
{
    screen_clear();
    scr_clr_timer_stop();
}

void timers_init(void)
{
    // Initialize timer module.
    prev_hour= 255;
    prev_min = 255;
    prev_wday= 255;
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    // Create timers.
    err_code = app_timer_create(&clock_timer,APP_TIMER_MODE_REPEATED, clock_timer_handler);
    err_code = app_timer_create(&disconnect_timer,APP_TIMER_MODE_REPEATED, disconnect_timer_handler);
    err_code = app_timer_create(&clear_screen_timer,APP_TIMER_MODE_SINGLE_SHOT, clear_screen_handler);
    APP_ERROR_CHECK(err_code);
    /* For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by one.*/
}

void scr_clr_timer_start(void)
{
    ret_code_t err_code;
    err_code = app_timer_start(clear_screen_timer, CLEAR_SCREEN_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

void scr_clr_timer_stop(void)
{
    app_timer_stop(clear_screen_timer);
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