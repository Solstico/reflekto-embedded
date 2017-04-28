#include <stdint.h>
#include <string.h>

#include "time_service.h"
#include "SEGGER_RTT.h"
#include "boards.h"
#include "app_error.h"
#include "ble_srv_common.h"

time_t unix_seconds;

void test_time_conv(time_t unix_time){
    struct tm* local_time=localtime(&unix_time);
    SEGGER_RTT_printf(0, " %d %d %d",local_time->tm_hour,local_time->tm_min,local_time->tm_sec);
}