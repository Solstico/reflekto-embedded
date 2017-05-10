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

#define ILI9341_TFTWIDTH    240
#define ILI9341_TFTHEIGHT   320

void gfx_initialization(void);
void background_set(void);
void screen_clear(void);
void text_print(char *text_in, uint16_t posX, uint16_t posY, uint8_t size);

typedef enum{
    TIME_G,
    WEATHER_G,
    HELLO_G,
    NAME_G,
    COMPLIMENT_G,
    CALENDAR_G,
    CLR_SCR
}gui_type;

void clear_GUI(gui_type type);
void print_actual_time(char string_to_print[18]);
void print_actual_weather(char string_to_print[18]);
void print_hello(char string_to_print[18]);
void print_name(char string_to_print[18]);
void print_additional_weather(char string_to_print[18]);
void print_weather_advise(char string_to_print[18]);
void print_sex(char string_to_print[18]);
void print_compliment(char string_to_print[18]);
void print_next_event(char string_to_print[18]);

#endif
