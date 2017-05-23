/*
Copyright (c) 2017, Michal Wojcik, email: michal.t.wojcik@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "reflekto_data_collector.h"
#include "SEGGER_RTT.h"
#include "reflekto_display.h"

collected_string weather_city = {.data = "", .collected_chars = 0, .is_completed = false, .needs_to_be_printed = false};
collected_string weather_wind  = {.data = "", .collected_chars = 0, .is_completed = false, .needs_to_be_printed = false};
collected_string weather_advice  = {.data = "", .collected_chars = 0, .is_completed = false, .needs_to_be_printed = false};

collected_string next_calendar_event  = {.data = "", .collected_chars = 0, .is_completed = false, .needs_to_be_printed = false};
collected_string unread_emails  = {.data = "", .collected_chars = 0, .is_completed = false, .needs_to_be_printed = false};
collected_string work_eta  = {.data = "", .collected_chars = 0, .is_completed = false, .needs_to_be_printed = false};

collected_string name  = {.data = "", .collected_chars = 0, .is_completed = false, .needs_to_be_printed = false};
collected_string hello  = {.data = "", .collected_chars = 0, .is_completed = false, .needs_to_be_printed = false};
collected_string compliment  = {.data = "", .collected_chars = 0, .is_completed = false, .needs_to_be_printed = false};

void update_collected_string(uint8_t in_data[20], uint8_t length, string_type type)
{
    if(!has_permission_to_write) 
    {
        disconnect_peripheral();
        return;
    }
    collected_string *updated_string;
    switch(type) {
        case CITY:
            updated_string = &weather_city;
            break;
        case WIND:
            updated_string = &weather_wind;
            break;
        case ADVICE:
            updated_string = &weather_advice;
            break;
        case CALENDAR:
            updated_string = &next_calendar_event;
            break;
        case EMAIL:
            updated_string = &unread_emails;
            break;
        case WORK_ETA:
            updated_string = &work_eta;
            break;
        case NAME:
            updated_string = &name;
            break;
        case HELLO:
            updated_string = &hello;
            break;
        case COMPLIMENT:
            updated_string = &compliment;
            break;
        default:
            return;
    }
    for(uint8_t i = 0; i<length && updated_string->collected_chars < MAX_STRING_LENGTH; i++)
    {
        switch(in_data[i]){
            case STX:
                updated_string->collected_chars = 0;
                updated_string->is_completed = false;
                updated_string->needs_to_be_printed = false;
                break;
            case ETX:
                updated_string->is_completed = true;
                updated_string->data[updated_string->collected_chars]='\0';
                SEGGER_RTT_printf(0,"String updated: %s \nChars: %d \nUpdate %d\nType: %d\n", updated_string->data, updated_string->collected_chars, updated_string->needs_to_be_printed, type);
                if(updated_string->needs_to_be_printed) update_gui(type);
                break;
            default:
                if(updated_string->data[updated_string->collected_chars] != in_data[i])
                {
                    updated_string->data[updated_string->collected_chars] = in_data[i];
                    updated_string->needs_to_be_printed = true;
                }
                updated_string->collected_chars++;
                break;
        }
    }
}
