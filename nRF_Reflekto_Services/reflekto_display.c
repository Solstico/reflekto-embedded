#include "reflekto_display.h"
#include "reflekto_ble_services.h"
#include "reflekto_timers.h"
#include "nrf_gfx.h"

#define GRAY            0xC618
#define RED             0xF800
#define BLUE            0x001F
#define BLACK           0x0000
#define WHITE           0xFFFF

#define LINE_STEP       10

#define CIRCLE_RADIUS   10
#define CIRCLE_STEP     ((2 * CIRCLE_RADIUS) + 1)

#define BORDER          2

extern const nrf_gfx_font_desc_t microsoftSansSerif_8ptFontInfo;
extern const nrf_gfx_font_desc_t microsoftSansSerif_16ptFontInfo;
extern const nrf_gfx_font_desc_t microsoftSansSerif_20ptFontInfo;
extern const nrf_lcd_t nrf_lcd_ili9341;
extern const nrf_lcd_t nrf_lcd_st7735;

static const nrf_gfx_font_desc_t * p_font = &microsoftSansSerif_20ptFontInfo;
static const nrf_lcd_t * p_lcd = &nrf_lcd_ili9341;

void gfx_initialization(void)
{
    APP_ERROR_CHECK(nrf_gfx_init(p_lcd));
}

void background_set(void)
{
    //nrf_gfx_invert(p_lcd, true);
    //nrf_gfx_background_set(p_lcd, nrf52);
    //nrf_gfx_invert(p_lcd, false);
}

void text_print(char *text_in, uint16_t posX, uint16_t posY, uint8_t size)
{
    nrf_gfx_point_t text_start = NRF_GFX_POINT(posX, posY);
    switch(size){
        case 8:
            APP_ERROR_CHECK(nrf_gfx_print(p_lcd, &text_start, 0xFFFF, text_in, &microsoftSansSerif_8ptFontInfo, true));
            break;
        case 16:
            APP_ERROR_CHECK(nrf_gfx_print(p_lcd, &text_start, 0xFFFF, text_in, &microsoftSansSerif_16ptFontInfo, true));
            break;
        case 20:
            APP_ERROR_CHECK(nrf_gfx_print(p_lcd, &text_start, 0xFFFF, text_in, &microsoftSansSerif_20ptFontInfo, true));
            break;
        default:
            break;          
} 

void screen_clear(void)
{
    nrf_gfx_screen_fill(p_lcd, BLACK);
}

static void line_draw(void)
{
    nrf_gfx_line_t my_line = NRF_GFX_LINE(0, 0, 0, nrf_gfx_height_get(p_lcd), 2);
    nrf_gfx_line_t my_line_2 = NRF_GFX_LINE(nrf_gfx_width_get(p_lcd), nrf_gfx_height_get(p_lcd), 0, nrf_gfx_height_get(p_lcd), 1);

    for (uint16_t i = 0; i <= nrf_gfx_width_get(p_lcd); i += LINE_STEP)
    {
        my_line.x_end = i;
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line, RED));
    }

    my_line.x_end = nrf_gfx_width_get(p_lcd);

    for (uint16_t i = 0; i <= nrf_gfx_height_get(p_lcd); i += LINE_STEP)
    {
        my_line.y_end = (nrf_gfx_height_get(p_lcd) - i);
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line, RED));
    }

    for (uint16_t i = 0; i <= nrf_gfx_height_get(p_lcd); i += LINE_STEP)
    {
        my_line_2.y_end = (nrf_gfx_height_get(p_lcd) - i);
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line_2, BLUE));
    }

    my_line_2.y_end = 0;

    for (uint16_t i = 0; i <= nrf_gfx_width_get(p_lcd); i += LINE_STEP)
    {
        my_line_2.x_end = i;
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line_2, BLUE));
    }
}

static void circle_draw(void)
{
    nrf_gfx_circle_t my_circle = NRF_GFX_CIRCLE(0, 0, CIRCLE_RADIUS);

    for (uint16_t j = 0; j <= nrf_gfx_height_get(p_lcd); j += CIRCLE_STEP)
    {
        my_circle.y = j;
        for (uint16_t i = 0; i <= nrf_gfx_width_get(p_lcd); i += CIRCLE_STEP)
        {
            my_circle.x = i;
            APP_ERROR_CHECK(nrf_gfx_circle_draw(p_lcd, &my_circle, BLUE, true));
        }
    }

    for (uint16_t j = CIRCLE_RADIUS; j <= nrf_gfx_height_get(p_lcd) + CIRCLE_RADIUS; j += CIRCLE_STEP)
    {
        my_circle.y = j;
        for (uint16_t i = CIRCLE_RADIUS; i <= nrf_gfx_width_get(p_lcd) + CIRCLE_RADIUS; i += CIRCLE_STEP)
        {
            my_circle.x = i;
            APP_ERROR_CHECK(nrf_gfx_circle_draw(p_lcd, &my_circle, RED, false));
        }
    }
}

// REFLEKTO FUNCTIONS

void clear_GUI(gui_type type)
{
		nrf_gfx_rect_t my_rect = NRF_GFX_RECT(0,0,ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT);
		switch (type){
			case TIME_G:
				my_rect.x=0; my_rect.y=0;
				my_rect.width = ILI9341_TFTWIDTH/2; my_rect.height = ILI9341_TFTHEIGHT/5; 
                                nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,true,0x000);
				break;
			case WEATHER_G:
				my_rect.x=ILI9341_TFTWIDTH/2; my_rect.y=0;
				my_rect.width = ILI9341_TFTWIDTH/2; my_rect.height = ILI9341_TFTHEIGHT/5; 
				nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,true,0x000);
				break;
			case HELLO_G:
				break;
			case NAME_G:
				my_rect.x=0; my_rect.y=ILI9341_TFTHEIGHT/5;
				my_rect.width = ILI9341_TFTWIDTH; my_rect.height = ILI9341_TFTHEIGHT/5; 
				nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,true,0x000);
				break;
			case COMPLIMENT_G:
				my_rect.x=0; my_rect.y=3*ILI9341_TFTHEIGHT/5-10;
				my_rect.width = ILI9341_TFTWIDTH; my_rect.height = ILI9341_TFTHEIGHT/10+10; 
				nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,true,0x000);
				break;
			case CALENDAR_G:
				my_rect.x=0; my_rect.y=7*ILI9341_TFTHEIGHT/10;
				my_rect.width = ILI9341_TFTWIDTH; my_rect.height = 3*ILI9341_TFTHEIGHT/10; 
				nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,true,0x000);
				break;
			case CLR_SCR:
				screen_clear();
				break;
			default:
				break;
			}
}

void print_actual_time(char string_to_print[18])
{
    clear_GUI(TIME_G);

}

void print_actual_weather(char string_to_print[18])
{

}

void print_hello(char string_to_print[18])
{

}

void print_name(char string_to_print[18])
{

}

void print_additional_weather(char string_to_print[18])
{

}

void print_weather_advise(char string_to_print[18])
{

}

void print_sex(char string_to_print[18])
{		

}

void print_compliment(char string_to_print[18])
{

}

void print_next_event(char string_to_print[18])
{

}
