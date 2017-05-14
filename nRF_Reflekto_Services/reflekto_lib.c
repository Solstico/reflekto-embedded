#include "reflekto_lib.h"

extern const nrf_gfx_font_desc_t Font10x16_desc;
extern const nrf_gfx_font_desc_t Font5x7_desc;

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
