#ifndef  REFLEKTO_TIMERS_H__
#define  REFLEKTO_TIMERS_H__

#include <time.h>

static time_t current_unix_seconds;

static void test_time_conv(time_t unix_time);
static void clock_timer_handler(void *p_context);
void timers_init(void);
void application_timers_start(void);
void set_current_time(time_t new_time);

#endif