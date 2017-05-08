#ifndef  REFLEKTO_DATA_COLLECTOR_H__
#define  REFLEKTO_DATA_COLLECTOR_H__

#include "reflekto_ble_services.h"
#include "reflekto_timers.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define MAX_STRING_LENGTH 80

typedef struct
{
   char data[MAX_STRING_LENGTH];
   uint8_t collected_chars;
   bool is_completed;
} collected_string;

typedef enum {
    CITY, 
    WIND, 
    ADVICE
} weather_type;

extern collected_string weather_city;
extern collected_string weather_wind;
extern collected_string weather_advice;

extern collected_string next_calendar_event;
extern collected_string unread_emails;
extern collected_string work_eta;

void update_weather(uint8_t data[20], uint8_t length , weather_type type);


#endif