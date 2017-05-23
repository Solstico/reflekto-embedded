/*
Copyright (c) 2017, Michal Wojcik, email: michal.t.wojcik@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "reflekto_display.h"
#include "reflekto_ble_services.h"
#include "reflekto_timers.h"
#include "nrf_gfx.h"
#include "weather_icons.h"
#include "SEGGER_RTT.h"

#define GRAY            0xC618
#define RED             0xF800
#define BLUE            0x001F
#define BLACK           0x0000
#define WHITE           0xFFFF

extern const nrf_gfx_font_desc_t microsoftSansSerif_8ptFontInfo;
extern const nrf_gfx_font_desc_t microsoftSansSerif_16ptFontInfo;
extern const nrf_gfx_font_desc_t microsoftSansSerif_20ptFontInfo;
extern const nrf_gfx_font_desc_t microsoftSansSerif_28ptFontInfo;
extern const nrf_lcd_t nrf_lcd_ili9341;
extern const nrf_lcd_t nrf_lcd_st7735;

static const nrf_gfx_font_desc_t * p_font = &microsoftSansSerif_20ptFontInfo;
static const nrf_lcd_t * p_lcd = &nrf_lcd_ili9341;

uint8_t prev_hour, prev_min, prev_wday;

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
            nrf_gfx_print(p_lcd, &text_start, 0xFFFF, text_in, &microsoftSansSerif_8ptFontInfo, true);
            break;
        case 16:
            nrf_gfx_print(p_lcd, &text_start, 0xFFFF, text_in, &microsoftSansSerif_16ptFontInfo, true);
            break;
        case 20:
            nrf_gfx_print(p_lcd, &text_start, 0xFFFF, text_in, &microsoftSansSerif_20ptFontInfo, true);
            break;
        case 28:
            nrf_gfx_print(p_lcd, &text_start, 0xFFFF, text_in, &microsoftSansSerif_28ptFontInfo, true);
            break;
        default:
            break;
    }
} 

void screen_clear(void)
{
    nrf_gfx_screen_fill(p_lcd, BLACK);
    scr_clr_timer_stop();
    weather_city.data[0]='\0';
    weather_wind.data[0]='\0';
    weather_wind.data[0]='\0';

    next_calendar_event.data[0]='\0';
    unread_emails.data[0]='\0';
    work_eta.data[0]='\0';

    name.data[0]='\0';
    hello.data[0]='\0';
    compliment.data[0]='\0';
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
			case TIME_A:
                            my_rect.x=0; my_rect.y=0;
                            my_rect.width=2*IMGWIDTH; my_rect.height=IMGWIDTH;
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0x0000,true,0x0000);
                            break;
                        case TIME_H:
                            my_rect.x=0; my_rect.y=28;
                            my_rect.width=2*IMGWIDTH; my_rect.height=IMGWIDTH-my_rect.y;
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0x0000,true,0x0000);
                            break;
                        case TIME_S:
                            my_rect.x=90; my_rect.y=28;
                            my_rect.width=38; my_rect.height=IMGWIDTH-my_rect.y;
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0x0000,true,0x0000);
                            break;
			case WEATHER_G:
                            my_rect.x=2*IMGWIDTH; my_rect.y=0;
                            my_rect.width=3*IMGWIDTH; my_rect.height=IMGWIDTH;
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0x0000,true,0x000);
                            my_rect.x=0; my_rect.y=IMGWIDTH+3;
                            my_rect.width=TFTWIDTH; my_rect.height=14;
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0x0000,true,0x000);
                            break;
			case HELLO_G:
                            my_rect.x=0; my_rect.y=IMGWIDTH+20;
                            my_rect.width=TFTWIDTH; my_rect.height=48;
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0x0000,true,0x000);
                            break;
			case CALENDAR_G:
                            my_rect.x=0; my_rect.y=2*IMGWIDTH+56;
                            my_rect.width = TFTWIDTH; my_rect.height = 56; 
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,true,0x0000);
                            break;
                        case WORK_ETA_G:
                            my_rect.x=0; my_rect.y=2*IMGWIDTH+4;
                            my_rect.width = TFTWIDTH; my_rect.height = 26; 
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0x0000,true,0x000);
                            my_rect.x=0; my_rect.y=2*IMGWIDTH+4;
                            my_rect.width = TFTWIDTH; my_rect.height = 53; 
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,false,0x000);
                            break;
                        case MAIL_G:
                            my_rect.x=0; my_rect.y=2*IMGWIDTH+30;
                            my_rect.width = TFTWIDTH; my_rect.height = 26; 
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0x0000,true,0x000);
                            my_rect.x=0; my_rect.y=2*IMGWIDTH+4;
                            my_rect.width = TFTWIDTH; my_rect.height = 53; 
                            nrf_gfx_rect_draw(p_lcd,&my_rect,1,0xFFFF,false,0x000);
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
    weather_city.is_completed = false;
    weather_wind.is_completed = false;
    weather_advice.is_completed = false;

    clear_GUI(WEATHER_G);
    uint8_t weather_i = weather_city.data[weather_city.collected_chars-1];
    weather_i -= 48;
    SEGGER_RTT_printf(0,"WEATHER ICON: %d\n", weather_i);
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
    weather_city.data[weather_city.collected_chars-1]='\0';//
    text_print(weather_city.data,3*IMGWIDTH+8,8,8);
    weather_city.data[weather_city.collected_chars-1]=weather_i+48;//
    text_print(weather_wind.data,3*IMGWIDTH+8,24,8);
    uint16_t xpos=TFTWIDTH/2-weather_advice.collected_chars*3.2;
    text_print(weather_advice.data,xpos,IMGWIDTH+3,8);
}

void update_timer()
{
    static char hour[3];
    static char minutes[3];
    static char seconds[3];
    static char final[10];
    static char final_sec[3];
    static char week_day[32];
    static char day[3];

    if(local_time->tm_hour != prev_hour) 
    {
        clear_GUI(TIME_H);
        sprintf(hour,"%d",local_time->tm_hour);
    }
    if(local_time->tm_min != prev_min)
    {
        clear_GUI(TIME_H);
        sprintf(minutes,"%d",local_time->tm_min);
    }
    if(local_time->tm_wday != prev_wday)
    {
        week_day[0]='\0';
        day[0]='\0';
        switch(local_time->tm_wday){
            case 0:
                strcat(week_day,"Sunday, ");
                break;
            case 1:
                strcat(week_day,"Monday, ");
                break;
            case 2:
                strcat(week_day,"Tuesday, ");
                break;
            case 3:
                strcat(week_day,"Wednesday, ");
                break;
            case 4:
                strcat(week_day,"Thursday, ");
                break;
            case 5:
                strcat(week_day,"Friday, ");
                break;
            case 6:
                strcat(week_day,"Saturday, ");
                break;
            default:
                break;
        }
        switch(local_time->tm_mon){
            case 0:
                strcat(week_day,"January ");
                break;
            case 1:
                strcat(week_day,"February ");
                break;
            case 2:
                strcat(week_day,"March ");
                break;
            case 3:
                strcat(week_day,"April ");
                break;
            case 4:
                strcat(week_day,"May ");
                break;
            case 5:
                strcat(week_day,"June ");
                break;
            case 6:
                strcat(week_day,"July ");
                break;
            case 7:
                strcat(week_day,"August ");
                break;
            case 8:
                strcat(week_day,"September ");
                break;
            case 9:
                strcat(week_day,"October ");
                break;
            case 10:
                strcat(week_day,"November ");
                break;
            case 11:
                strcat(week_day,"December ");
                break;
            default:
                break;
        }
        sprintf(day,"%d",local_time->tm_mday);
        strcat(week_day,day);
        clear_GUI(TIME_A);
    }

    final[0]='\0';
    final_sec[0]='\0';
    if(local_time->tm_hour < 10) strcat(final," ");
    strcat(final,hour);
    strcat(final,":");
    if(local_time->tm_min < 10) strcat(final,"0");
    strcat(final,minutes);

    sprintf(seconds, "%d", local_time->tm_sec);
    if(local_time->tm_sec < 10) strcat(final_sec,"0");
    strcat(final_sec,seconds);
    clear_GUI(TIME_S);

    text_print(final,3,28,28);
    text_print(final_sec,90,41,16);
    text_print(week_day,3,5,8);
    prev_min = local_time->tm_min;
    prev_hour = local_time->tm_hour;
    prev_wday = local_time->tm_wday;
    
}

void update_calendar()
{
    if(!next_calendar_event.is_completed) return;
    if(!next_calendar_event.needs_to_be_printed) return;
    next_calendar_event.needs_to_be_printed = false;
    clear_GUI(CALENDAR_G);
    text_print(next_calendar_event.data,8,2*IMGWIDTH+58,16);
}

void update_emails()
{
    if(!unread_emails.is_completed) return;
    if(!unread_emails.needs_to_be_printed) return;
    unread_emails.needs_to_be_printed = false;
    
    clear_GUI(MAIL_G);
    text_print(unread_emails.data,8,2*IMGWIDTH+31,16);
}

void update_work_eta()
{
    if(!work_eta.is_completed) return;
    if(!work_eta.needs_to_be_printed) return;
    work_eta.needs_to_be_printed = false;

    clear_GUI(WORK_ETA_G);
    text_print(work_eta.data,8,2*IMGWIDTH+7,16);
}

void update_hello()
{
    static char say_hello[48];
    if(!(name.is_completed && hello.is_completed && compliment.is_completed)) return;
    if(!(name.needs_to_be_printed || hello.needs_to_be_printed || compliment.needs_to_be_printed)) return;
    name.needs_to_be_printed = false;
    hello.needs_to_be_printed = false;
    compliment.needs_to_be_printed = false;
    name.is_completed = false;
    hello.is_completed = false;
    compliment.is_completed = false;
    say_hello[0]='\0';
    strcat(say_hello,hello.data);
    strcat(say_hello,", ");
    strcat(say_hello,name.data);
    clear_GUI(HELLO_G);
    text_print(say_hello,8,IMGWIDTH+20,16);
    text_print(compliment.data,8, IMGWIDTH+42,16);

}


