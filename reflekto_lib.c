#include "reflekto_lib.h"

extern const nrf_gfx_font_desc_t Font10x16_desc;

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

void text_print(char text_in[], uint16_t posX, uint16_t posY)
{
    nrf_gfx_point_t text_start = NRF_GFX_POINT(posX, posY);
    nrf_gfx_print(&text_start, 0xFFFF, text_in, &Font10x16_desc, true);
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

void print_actual_time(char string_to_print[18])
{
	text_print(string_to_print,90,30);
}

void print_actual_weather(char string_to_print[18])
{
	text_print(string_to_print,10,60);
}

void print_name(char string_to_print[18])
{
	text_print("Name: ",10,90);
	text_print(string_to_print,90,90);
}
