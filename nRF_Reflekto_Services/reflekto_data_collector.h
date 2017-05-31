/*
Copyright (c) 2017, Michal Wojcik, email: michal.t.wojcik@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef  REFLEKTO_DATA_COLLECTOR_H__
#define  REFLEKTO_DATA_COLLECTOR_H__

#include "reflekto_ble_services.h"
#include "reflekto_timers.h"


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define MAX_STRING_LENGTH 100
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
extern collected_string compliment;

void update_collected_string(uint8_t in_data[20], uint8_t length , string_type type);


#endif
