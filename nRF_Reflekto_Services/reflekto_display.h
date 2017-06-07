/*
Copyright (c) 2017, Michal Wojcik, email: michal.t.wojcik@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef  REFLEKTO_DISPLAY_H__
#define  REFLEKTO_DISPLAY_H__

#include "reflekto_data_collector.h"

#include "nrf_gfx.h"
#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_font.h"


#define TFTWIDTH    320
#define TFTHEIGHT   240
#define IMGWIDTH    64
extern uint8_t prev_hour, prev_min, prev_wday;
void gfx_initialization(void);
void screen_clear(void);
void text_print(char *text_in, uint16_t posX, uint16_t posY, uint8_t size);

typedef enum{
    TIME_A,
    TIME_H,
    TIME_S,
    WEATHER_G,
    HELLO_G,
    NAME_G,
    COMPLIMENT_G,
    CALENDAR_G,
    WORK_ETA_G,
    MAIL_G,
    CLR_SCR
}clear_gui_type;

typedef enum{
    CLEAR_DAY,
    CLEAR_NIGHT,
    CLOUDY,
    FOG,
    PARTLY_CLOUDLY_DAY,
    PARTLY_CLOUDLY_NIGHT,
    RAIN,
    SLEET,
    SNOW,
    WIND_I
}weather_icon;

void update_gui(string_type_t type);
void clear_GUI(clear_gui_type type);

static void print_weather_icon(weather_icon icon);
void update_weather();

void update_timer();
void update_calendar();
void update_emails();
void update_work_eta();
void update_hello();

#endif
