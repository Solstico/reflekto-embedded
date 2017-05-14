#ifndef  REFLEKTO_DISPLAY_H__
#define  REFLEKTO_DISPLAY_H__

#include "reflekto_data_collector.h"

#include "nrf_gfx.h"
#include "nrf52.h"
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
void background_set(void);
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

void update_gui(string_type type);
void clear_GUI(clear_gui_type type);

static void print_weather_icon(weather_icon icon);
void update_weather();

void update_timer();
void update_calendar();
void update_emails();
void update_work_eta();
void update_hello();

#endif
