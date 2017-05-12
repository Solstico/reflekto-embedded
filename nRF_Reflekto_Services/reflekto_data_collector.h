#ifndef  REFLEKTO_DATA_COLLECTOR_H__
#define  REFLEKTO_DATA_COLLECTOR_H__

#include "reflekto_ble_services.h"
#include "reflekto_timers.h"


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define MAX_STRING_LENGTH 80
#define  STX 2 // Start of text
#define  ETX 3 // End of text

typedef struct
{
   char data[MAX_STRING_LENGTH];
   uint8_t collected_chars;
   bool is_completed;
   bool needs_to_be_printed;
} collected_string;

typedef enum {
    CITY, 
    WIND, 
    ADVICE,
    CALENDAR,
    EMAIL,
    WORK_ETA,
    NAME,
    HELLO,
    COMPLIMENT
} string_type;

extern collected_string weather_city;
extern collected_string weather_wind;
extern collected_string weather_advice;

extern collected_string next_calendar_event;
extern collected_string unread_emails;
extern collected_string work_eta;

extern collected_string name;
extern collected_string hello;

void update_collected_string(uint8_t in_data[20], uint8_t length , string_type type);


#endif