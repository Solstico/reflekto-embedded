#include "reflekto_lib.h"

extern const nrf_gfx_font_desc_t Font10x16_desc;
extern const nrf_gfx_font_desc_t Font5x7_desc;

static char welcome_string[36];
static char sex_string[36];
static uint8_t received_parts[9];

void gfx_initialization(void)
{
    ret_code_t err_code;

    nrf_gfx_config_t nrf_gfx_config_ili = 
    {
        .lcd_init = ili9341_init,
        .point_draw = (point_draw_t)ili9341_pixel_draw,
        .rect_draw = (rect_draw_t)ili9341_rect_draw,
        .screen_width = ILI9341_TFTWIDTH,
        .screen_height = ILI9341_TFTHEIGHT
    };

    err_code = nrf_gfx_init(&nrf_gfx_config_ili);
    APP_ERROR_CHECK(err_code);
}

void brackground_set(void)
{
    //nrf_gfx_background_set(nrf52);
}

void text_print(char text_in[], uint16_t posX, uint16_t posY, uint8_t size)
{		
		nrf_gfx_point_t text_start = NRF_GFX_POINT(posX, posY);
		if (size == 10) nrf_gfx_print(&text_start, 0xFFFF, text_in, &Font10x16_desc, true);
		else nrf_gfx_print(&text_start, 0xFFFF, text_in, &Font5x7_desc, true);
}

void screen_clear(void)
{
    nrf_gfx_screen_fill(0x0000);
}

void line_draw(void)
{
    nrf_gfx_line_t my_line = NRF_GFX_LINE(0, 0, 0, 320, 2);
    nrf_gfx_line_t my_line_2 = NRF_GFX_LINE(240, 320, 0, 320, 1);

    for (uint16_t i = 0; i <= ILI9341_TFTWIDTH; i += 10)
    {
        my_line.x_end = i;
        nrf_gfx_line_draw(&my_line, 0xF800);
    }

    for (uint16_t i = 0; i <= ILI9341_TFTHEIGHT; i += 10)
    {
        my_line.y_end = (ILI9341_TFTHEIGHT - i);
        nrf_gfx_line_draw(&my_line, 0xF800);
    }

    for (uint16_t i = 0; i <= ILI9341_TFTHEIGHT; i += 10)
    {
        my_line_2.y_end = (ILI9341_TFTHEIGHT - i);
        nrf_gfx_line_draw(&my_line_2, 0x001F);
    }

    for (uint16_t i = 0; i <= ILI9341_TFTWIDTH; i += 10)
    {
        my_line_2.x_end = i;
        nrf_gfx_line_draw(&my_line_2, 0x001F);
    }
}

void circle_draw(void)
{
    nrf_gfx_circle_t my_circle = NRF_GFX_CIRCLE(0, 0, 10);

    for (uint16_t j = 0; j <= ILI9341_TFTHEIGHT; j += 20)
    {
        my_circle.y = j;
        for (uint16_t i = 0; i <= ILI9341_TFTWIDTH; i += 20)
        {
            my_circle.x = i;
            nrf_gfx_circle_draw(&my_circle, 0x1415, true);
        }
    }

    for (uint16_t j = 10; j <= ILI9341_TFTHEIGHT; j += 20)
    {
        my_circle.y = j;
        for (uint16_t i = 10; i <= ILI9341_TFTWIDTH; i += 20)
        {
            my_circle.x = i;
            nrf_gfx_circle_draw(&my_circle, 0x0005, false);
        }
    }
}

void rect_draw(void)
{
    nrf_gfx_rect_t my_rect = NRF_GFX_RECT(ILI9341_TFTWIDTH / 2, ILI9341_TFTHEIGHT / 2, 2, 2);
    for (uint16_t i = 0, j = 0; i <= ILI9341_TFTWIDTH / 2; i += 6, j+= 8)
    {
        my_rect.x = i;
        my_rect.y = j;
        my_rect.width = ILI9341_TFTWIDTH - i * 2;
        my_rect.height = ILI9341_TFTHEIGHT - j * 2;
        nrf_gfx_rect_draw(&my_rect, 2, (i + j) * 10, true, UINT16_MAX - (i + j) * 10);
    }

}

void clear_GUI(enum DataType type)
{
		nrf_gfx_rect_t my_rect = NRF_GFX_RECT(0,0,ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT);
		switch (type){
			case TIME:
				my_rect.x=0; my_rect.y=0;
				my_rect.width = ILI9341_TFTWIDTH/2; my_rect.height = ILI9341_TFTHEIGHT/5; 
				nrf_gfx_rect_draw(&my_rect, 1, 0xFFFF, true, 0x0000);
				break;
			case WEATHER1:
				my_rect.x=ILI9341_TFTWIDTH/2; my_rect.y=0;
				my_rect.width = ILI9341_TFTWIDTH/2; my_rect.height = ILI9341_TFTHEIGHT/5; 
				nrf_gfx_rect_draw(&my_rect, 1, 0xFFFF, true, 0x0000);
				break;
			case HELLO:
				break;
			case NAME:
				my_rect.x=0; my_rect.y=ILI9341_TFTHEIGHT/5;
				my_rect.width = ILI9341_TFTWIDTH; my_rect.height = ILI9341_TFTHEIGHT/5; 
				nrf_gfx_rect_draw(&my_rect, 1, 0xFFFF, true, 0x0000);
				break;
			case SEX:
				my_rect.x=0; my_rect.y=2*ILI9341_TFTHEIGHT/5;
				my_rect.width = ILI9341_TFTWIDTH; my_rect.height = ILI9341_TFTHEIGHT/5-10; 
				nrf_gfx_rect_draw(&my_rect, 0, 0xFFFF, true, 0x0000);
				break;
			case COMPLIMENT:
				my_rect.x=0; my_rect.y=3*ILI9341_TFTHEIGHT/5-10;
				my_rect.width = ILI9341_TFTWIDTH; my_rect.height = ILI9341_TFTHEIGHT/10+10; 
				nrf_gfx_rect_draw(&my_rect, 0, 0xFFFF, true, 0x0000);
				break;
			case WEATHER2:
				break;
			case WEATHER3:
				break;
			case CALENDAR:
				my_rect.x=0; my_rect.y=7*ILI9341_TFTHEIGHT/10;
				my_rect.width = ILI9341_TFTWIDTH; my_rect.height = 3*ILI9341_TFTHEIGHT/10; 
				nrf_gfx_rect_draw(&my_rect, 1, 0xFFFF, true, 0x0000);
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
	clear_GUI(TIME);
	text_print(string_to_print,28,26,10);
}

void print_actual_weather(char string_to_print[18])
{
	clear_GUI(WEATHER1);
	text_print(string_to_print,ILI9341_TFTWIDTH/2+10,10,5);
}

void print_hello(char string_to_print[18])
{
		memset(&welcome_string, 0, sizeof(welcome_string));
		received_parts[HELLO]=1;
		uint8_t i=0;
		for(i=0; i<18; i++)
		{
				welcome_string[i]=string_to_print[i];
		}
}

void print_name(char string_to_print[18])
{
		uint8_t i=0;
		for(i=0; i<18; i++)
		{
				if(welcome_string[i]==0) 
				{
						welcome_string[i]=',';
						i++;
						break;
				}
		}
		for(; i<18; i++) //fill first line
		{
				welcome_string[i]=' ';
		}
		for(uint8_t j=0; j<18; j++)
		{
				welcome_string[i+j]=string_to_print[j];
		}
		clear_GUI(NAME);
		if(received_parts[HELLO]==1) text_print(welcome_string,10,ILI9341_TFTHEIGHT/5+10,10);
		received_parts[HELLO]=0;
}

void print_additional_weather(char string_to_print[18])
{
		text_print(string_to_print,ILI9341_TFTWIDTH/2+10,20,5);
}

void print_weather_advise(char string_to_print[18])
{
		text_print(string_to_print,ILI9341_TFTWIDTH/2+10,30,5);
}

void print_sex(char string_to_print[18])
{		
		clear_GUI(SEX);
		memset(&sex_string, 0, sizeof(sex_string));
		received_parts[SEX]=1;
		uint8_t i=0;
		char* my_text="You are";
		for(i=0; i<strlen(my_text); i++) sex_string[i]=my_text[i];
		sex_string[i]=' '; i++;
		for(uint8_t j=0; j<18;j++)	sex_string[i+j]=string_to_print[j];
		text_print(sex_string,10,2*ILI9341_TFTHEIGHT/5+10,10);
		text_print("I am sure!",10,2*ILI9341_TFTHEIGHT/5+35,10);
}

void print_compliment(char string_to_print[18])
{
		clear_GUI(COMPLIMENT);
		text_print(string_to_print,10,3*ILI9341_TFTHEIGHT/5-10,10);
}

void print_next_event(char string_to_print[18])
{
		clear_GUI(CALENDAR);
		text_print("Your next event:",10,7*ILI9341_TFTHEIGHT/10+10,10);
		text_print(string_to_print,10,7*ILI9341_TFTHEIGHT/10+35,10);
}
