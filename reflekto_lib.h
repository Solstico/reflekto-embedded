#ifndef REFLEKTO_LIB_H__
#define REFLEKTO_LIB_H__

#include "nrf_gfx.h"
#include "ili9341.h"
#include "nrf52.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


void gfx_initialization(void);
void brackground_set(void);
void text_print(char text_in[], uint16_t posX, uint16_t posY);
void screen_clear(void);
void line_draw(void);
void circle_draw(void);
void rect_draw(void);
void print_actual_time(char string_to_print[18]);
void print_actual_weather(char string_to_print[18]);
void print_hello(char string_to_print[18]);
void print_name(char string_to_print[18]);
void print_additional_weather(char string_to_print[18]);
#endif
