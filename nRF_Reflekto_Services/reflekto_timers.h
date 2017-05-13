#ifndef  REFLEKTO_TIMERS_H__
#define  REFLEKTO_TIMERS_H__

#include <time.h>

extern time_t current_unix_seconds;
extern struct tm* local_time;

static void test_time_conv(time_t unix_time);
static void clock_timer_handler(void *p_context);
void timers_init(void);
void disconnect_timer_start(void);
void disconnect_timer_stop(void);
void set_current_time(time_t new_time);

#endif