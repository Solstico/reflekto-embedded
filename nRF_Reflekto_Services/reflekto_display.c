#include "reflekto_display.h"
#include "reflekto_ble_services.h"
#include "reflekto_timers.h"
#include "nrf_gfx.h"
#include "weather_icons.h"

#define GRAY            0xC618
#define RED             0xF800
#define BLUE            0x001F
#define BLACK           0x0000
#define WHITE           0xFFFF

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
    nrf_gfx_rotation_set(p_lcd,NRF_LCD_ROTATE_90);
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
		nrf_gfx_rect_t my_rect = NRF_GFX_RECT(0,0,TFTWIDTH, TFTHEIGHT);
		switch (type){
			case TIME_G:
                            my_rect.x=0; my_rect.y=0;
                            my_rect.width=2*IMGWIDTH; my_rect.height=IMGWIDTH;
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,true,0x000);
                            break;
			case WEATHER_G:
                            my_rect.x=2*IMGWIDTH; my_rect.y=0;
                            my_rect.width=3*IMGWIDTH; my_rect.height=IMGWIDTH;
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0x0000,true,0x000);
                            break;
			case HELLO_G:
                            break;
			case NAME_G:
                            break;
			case COMPLIMENT_G:
                            break;
			case CALENDAR_G:
                            my_rect.x=0; my_rect.y=7*TFTHEIGHT/10;
                            my_rect.width = TFTWIDTH; my_rect.height = 3*TFTHEIGHT/10; 
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,true,0x000);
                            break;
			case CLR_SCR:
                            screen_clear();
                            break;
			default:
                            break;
			}
}

static void print_weather_icon(weather_icon icon)
{   
    uint8_t width, height;
    uint8_t x = 2*IMGWIDTH, y=0;
    const uint8_t *img_buf;

    switch (icon){
        case CLEAR_DAY:
            width = clear_dayWidthPages;
            height = clear_dayHeightPixels;
            img_buf = clear_dayBitmaps;
            break;
        case CLEAR_NIGHT:
            width = clear_nightWidthPages;
            height = clear_nightHeightPixels;
            img_buf = clear_nightBitmaps;
            break;
        case RAIN:
            width = rainWidthPages;
            height = rainHeightPixels;
            img_buf = rainBitmaps;
            break;
        case SNOW:
            width = snowWidthPages;
            height = snowHeightPixels;
            img_buf = snowBitmaps;
            break;
        case SLEET:
            width = sleetWidthPages;
            height = sleetHeightPixels;
            img_buf = sleetBitmaps;
            break;
        case WIND_I:
            width = windWidthPages;
            height = windHeightPixels;
            img_buf = windBitmaps;
            break;
        case FOG:
            width = fogWidthPages;
            height = fogHeightPixels;
            img_buf = fogBitmaps;
            break;
        case CLOUDY:
            width = cloudlyWidthPages;
            height = cloudlyHeightPixels;
            img_buf = cloudlyBitmaps;
            break;
        case PARTLY_CLOUDLY_DAY:
            width = partly_cloudly_dayWidthPages;
            height = partly_cloudly_dayHeightPixels;
            img_buf = partly_cloudly_dayBitmaps;
            break;
        case PARTLY_CLOUDLY_NIGHT:
            width = partly_cloudly_nightWidthPages;
            height = partly_cloudly_nightHeightPixels;
            img_buf = partly_cloudly_nightBitmaps;
            break;
        default:
            return;
    }
    nrf_gfx_bitmap_draw(p_lcd, width, height, x, y, img_buf);
}

void update_weather()
{
    if(!(weather_city.is_completed && weather_wind.is_completed && weather_advice.is_completed)) return;
    if(!(weather_city.needs_to_be_printed || weather_wind.needs_to_be_printed || weather_advice.needs_to_be_printed)) return;
    weather_city.needs_to_be_printed = false;
    weather_wind.needs_to_be_printed = false;
    weather_advice.needs_to_be_printed = false;

    clear_GUI(WEATHER_G);
    uint8_t weather_i = weather_city.data[weather_city.collected_chars-1];
    switch (weather_i){
        case 1:
            print_weather_icon(CLEAR_DAY);
            break;
        case 2:
            print_weather_icon(CLEAR_NIGHT);
            break;
        case 3:
            print_weather_icon(RAIN);
            break;
        case 4:
            print_weather_icon(SNOW);
            break;
        case 5:
            print_weather_icon(SLEET);
            break;
        case 6:
            print_weather_icon(WIND_I);
            break;
        case 7:
            print_weather_icon(FOG);
            break;
        case 8:
            print_weather_icon(CLOUDY);
            break;
        case 9:
            print_weather_icon(PARTLY_CLOUDLY_DAY);
            break;
        case 10:
            print_weather_icon(PARTLY_CLOUDLY_NIGHT);
            break;
        default:
            print_weather_icon(PARTLY_CLOUDLY_DAY);
            break;
    }
    //uncomment when icon ready
    //weather_city.data[weather_city.collected_chars-1]='\0';
    text_print(weather_city.data,3*IMGWIDTH+5,8,8);
    //weather_city.data[weather_city.collected_chars-1]=weather_i;
    text_print(weather_wind.data,3*IMGWIDTH+5,24,8);
    uint16_t xpos=TFTWIDTH/2-weather_advice.collected_chars*3.2;
    text_print(weather_advice.data,xpos,IMGWIDTH+3,8);
}


void update_timer()
{
    clear_GUI(TIME_G);
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


