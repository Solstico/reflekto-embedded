#include "reflekto_data_collector.h"

collected_string weather_city;
collected_string weather_wind;
collected_string weather_advice;

collected_string next_calendar_event;
collected_string unread_emails;
collected_string work_eta;

void update_weather(uint8_t data[20], uint8_t length, weather_type type)
{
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
        default:
            return;
    }
    for(uint8_t i = 0; i<length && i<MAX_STRING_LENGTH; i++)
    {
        if(data[i] == 1) 
        {
            updated_string->collected_chars = 0;
            memset(&(updated_string->data), 0, sizeof(updated_string->data));
        }
        if(data[i] == 4) 
        {
            updated_string->is_completed = true;
        }
    }
}
