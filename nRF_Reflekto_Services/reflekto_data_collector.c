#include "reflekto_data_collector.h"
#include "SEGGER_RTT.h"

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
