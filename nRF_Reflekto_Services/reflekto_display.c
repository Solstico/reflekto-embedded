#include "reflekto_display.h"
#include "reflekto_ble_services.h"
#include "reflekto_timers.h"
#include "nrf_gfx.h"


#define GRAY            0xC618
#define RED             0xF800
#define BLUE            0x001F
#define BLACK           0x0000
#define WHITE           0xFFFF

#define BORDER          2
#define SEPARATOR       2
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
} 

void screen_clear(void)
{
    nrf_gfx_screen_fill(p_lcd, BLACK);
}

void update_gui(string_type type)
{
    switch(type) {
        case CITY:
            update_weather();
            break;
        case WIND:
            update_weather();
            break;
        case ADVICE:
            update_weather();
            break;
        case CALENDAR:
            update_calendar();
            break;
        case EMAIL:
            update_emails();
            break;
        case WORK_ETA:
            update_work_eta();
            break;
        case NAME:
            update_hello();
            break;
        case HELLO:
            update_hello();
            break;
        case COMPLIMENT:
            update_hello();
            break;
        default:
            return;
    }

}

void clear_GUI(clear_gui_type type)
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

void update_weather()
{
    if(!(weather_city.is_completed && weather_wind.is_completed && weather_advice.is_completed)) return;
    if(!(weather_city.needs_to_be_printed || weather_wind.needs_to_be_printed || weather_advice.needs_to_be_printed)) return;
    weather_city.needs_to_be_printed = false;
    weather_wind.needs_to_be_printed = false;
    weather_advice.needs_to_be_printed = false;

    clear_GUI(WEATHER_G);
    text_print(weather_city.data,ILI9341_TFTWIDTH/2+3,3,8);
    text_print(weather_wind.data,ILI9341_TFTWIDTH/2+3,16,8);
    text_print(weather_advice.data,ILI9341_TFTWIDTH/2+3,29,8);
}


void update_calendar()
{
}

void update_emails()
{
}

void update_work_eta()
{
}

void update_hello()
{
}


