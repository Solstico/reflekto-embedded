/* Copyright (c) 2017 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 */

#include "nrf_gfx.h"
#include <stdlib.h>
#include "app_util_platform.h"
#define NRF_LOG_MODULE_NAME "GFX"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

typedef struct
{
    point_draw_t point_draw;
    rect_draw_t rect_draw;
    uint16_t screen_width;
    uint16_t screen_height;
    int32_t min_x;
    int32_t max_x;
    int32_t min_y;
    int32_t max_y;
}nrf_gfx_t;

nrf_gfx_t m_gfx;

__STATIC_INLINE void pixel_draw(uint16_t x, uint16_t y, uint32_t color)
{
    if ((x > m_gfx.screen_width) || (y > m_gfx.screen_height))
    {
        return;
    }

    m_gfx.point_draw(x, y, color);
}

__STATIC_INLINE void rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color)
{
    if ((x > m_gfx.screen_width) || (y > m_gfx.screen_height))
    {
        return;
    }

    if (m_gfx.rect_draw == NULL)
    {
        for (uint16_t i = 0; i < height; i++)
        {
            for (uint16_t j = 0; j < width; j++)
            {
                pixel_draw(x + j, y + i, color);
            }
        }
    }
    else
    {
        m_gfx.rect_draw(x, y, width, height, color);
    }
}

__STATIC_INLINE void draw_line(uint16_t x_0, uint16_t y_0, uint16_t x_1, uint16_t y_1, uint32_t color)
{
    uint16_t x = x_0;
    uint16_t y = y_0;
    int16_t d;
    int16_t d_1;
    int16_t d_2;
    int16_t ai;
    int16_t bi;
    int16_t xi = (x_0 < x_1) ? 1 : (-1);
    int16_t yi = (y_0 < y_1) ? 1 : (-1);
    bool swapped = false;

    d_1 = abs(x_1 - x_0);
    d_2 = abs(y_1 - y_0);

    pixel_draw(x, y, color);

    if (d_1 < d_2)
    {
        d_1 = d_1 ^ d_2;
        d_2 = d_1 ^ d_2;
        d_1 = d_2 ^ d_1;
        swapped = true;
    }

    ai = (d_2 - d_1) * 2;
    bi = d_2 * 2;
    d = bi - d_1;

    while ((y != y_1) || (x != x_1))
    { 
        if (d >= 0)
        { 
            x += xi;
            y += yi;
            d += ai;
        } 
        else
        {
            d += bi;
            if (swapped)
            {
                y += yi;
            }
            else
            {
                x += xi;
            }
        }
        pixel_draw(x, y, color);
    }
}

__STATIC_INLINE void write_character(const nrf_gfx_font_desc_t * p_font,
                                    uint8_t character, 
                                    uint16_t x,
                                    uint16_t y,
                                    uint16_t font_color)
{
    switch (p_font->font_type)
    {
        case FONT5X7:
            for (uint8_t i = 0; i < p_font->width; i++)
            {
                for (uint8_t j = 0; j < p_font->height; j++)
                {
                    if (p_font->p_data[(character - p_font->ascii_offset) * p_font->width + i] & 1 << j)
                    {
                        pixel_draw(x + i, y + j, font_color);
                    }
                }
            }
            break;
        case FONT10X16:
            for (uint8_t i = 0; i < p_font->width; i++)
            {
                for (uint8_t j = 0; j < (p_font->height / 2); j++)
                {
                    if (p_font->p_data[(character - p_font->ascii_offset) * p_font->width * 2 + i] & 1 << j)
                    {
                        pixel_draw(x + i, y + j, font_color);
                    }
                }
            }

            for (uint8_t i = p_font->width; i < p_font->width * 2; i++)
            {
                for (uint8_t j = 0; j < (p_font->height / 2); j++)
                {
                    if (p_font->p_data[(character - p_font->ascii_offset) * p_font->width * 2 + i] & 1 << j)
                    {
                        pixel_draw(x + i - p_font->width, y + j + (p_font->height / 2), font_color);
                    }
                }
            }
            break;
        default:
            break;
    }
}

ret_code_t nrf_gfx_init(nrf_gfx_config_t * p_config)
{
    ret_code_t err_code = NRF_SUCCESS;
    m_gfx.point_draw = p_config->point_draw;
    m_gfx.rect_draw = p_config->rect_draw;
    m_gfx.screen_width = p_config->screen_width;
    m_gfx.screen_height = p_config->screen_height;
    err_code = p_config->lcd_init();
    return err_code;
}

void nrf_gfx_point_draw(nrf_gfx_point_t * p_point, uint32_t color)
{
    pixel_draw(p_point->x, p_point->y, color);
}

void nrf_gfx_line_draw(nrf_gfx_line_t * p_line, uint32_t color)
{
    uint16_t x_thick = 0;
    uint16_t y_thick = 0;

    if (abs(p_line->x_start - p_line->x_end) > abs(p_line->y_start - p_line->y_end))
    {
        y_thick = p_line->thickness;
    } 
    else
    {
        x_thick = p_line->thickness;
    }

    if ((p_line->x_start == p_line->x_end) || (p_line->y_start == p_line->y_end))
    {
        rect_draw(p_line->x_start, p_line->y_start, abs(p_line->x_end - p_line->x_start) + x_thick, abs(p_line->y_end - p_line->y_start) + y_thick, color);
    }
    else
    {
        if (x_thick > 0)
        {
            for (uint16_t i = 0; i < p_line->thickness; i++)
            {
                draw_line(p_line->x_start + i, p_line->y_start, p_line->x_end + i, p_line->y_end, color);
            }
        }
        else if (y_thick > 0)
        {
            for (uint16_t i = 0; i < p_line->thickness; i++)
            {
                draw_line(p_line->x_start, p_line->y_start + i, p_line->x_end, p_line->y_end + i, color);
            }
        }
        else
        {
            draw_line(p_line->x_start + x_thick, p_line->y_start + y_thick, p_line->x_end + x_thick, p_line->y_end + y_thick, color); 
        }
    }
}

void nrf_gfx_circle_draw(nrf_gfx_circle_t * p_circle,
                         uint32_t color,
                         bool fill)
{
    int16_t y = 0;
    int16_t err = 0;
    int16_t x = fill ? (p_circle->r + 1) : p_circle->r;

    while (x >= y)
    {
        if (fill)
        {
            if ((-y + p_circle->x < 0) || (-x + p_circle->x < 0))
            {
                rect_draw(0, -x + p_circle->y, (y + p_circle->x), 1, color);
                rect_draw(0, -y + p_circle->y, (x + p_circle->x), 1, color);
                rect_draw(0, y + p_circle->y, (x + p_circle->x), 1, color);
                rect_draw(0, x + p_circle->y, (y + p_circle->x), 1, color);
            }
            else
            {
                rect_draw(-y + p_circle->x, -x + p_circle->y, 2 * y, 1, color);
                rect_draw(-x + p_circle->x, -y + p_circle->y, 2 * x, 1, color);
                rect_draw(-x + p_circle->x, y + p_circle->y, 2 * x, 1, color);
                rect_draw(-y + p_circle->x, x + p_circle->y, 2 * y, 1, color);
            }
        }
        else
        {
            pixel_draw(y + p_circle->x, x + p_circle->y, color);
            pixel_draw(-y + p_circle->x, x + p_circle->y, color);
            pixel_draw(x + p_circle->x, y + p_circle->y, color);
            pixel_draw(-x + p_circle->x, y + p_circle->y, color);
            pixel_draw(-y + p_circle->x, -x + p_circle->y, color);
            pixel_draw(y + p_circle->x, -x + p_circle->y, color);
            pixel_draw(-x + p_circle->x, -y + p_circle->y, color);
            pixel_draw(x + p_circle->x, -y + p_circle->y, color);
        }

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

ret_code_t nrf_gfx_rect_draw(nrf_gfx_rect_t * p_rect,
                             uint16_t thickness,
                             uint32_t color,
                             bool fill,
                             uint32_t fill_color)
{
    uint16_t rect_width = p_rect->width - thickness;
    uint16_t rect_height = p_rect->height - thickness;
    
    if ((p_rect->width == 1) || (p_rect->height == 1) || (thickness * 2 > p_rect->width) || (thickness * 2 > p_rect->height))
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    if (thickness > 0)
    {
        nrf_gfx_line_t line;

        // Top horizontal line.
        line.x_start = p_rect->x;
        line.y_start = p_rect->y;
        line.x_end = p_rect->x + p_rect->width;
        line.y_end = p_rect->y;
        line.thickness = thickness;
        nrf_gfx_line_draw(&line, color);
        // Bottom horizontal line.
        line.x_start = p_rect->x;
        line.y_start = p_rect->y + rect_height;
        line.x_end = p_rect->x + p_rect->width;
        line.y_end = p_rect->y + rect_height;
        nrf_gfx_line_draw(&line, color);
        // Left vertical line.
        line.x_start = p_rect->x;
        line.y_start = p_rect->y + thickness;
        line.x_end = p_rect->x;
        line.y_end = p_rect->y + rect_height;
        nrf_gfx_line_draw(&line, color);
        // Right vertical line.
        line.x_start = p_rect->x + rect_width;
        line.y_start = p_rect->y + thickness;
        line.x_end = p_rect->x + rect_width;
        line.y_end = p_rect->y + rect_height;
        nrf_gfx_line_draw(&line, color);
    }
    if (fill)
    {
        rect_draw(p_rect->x + thickness, p_rect->y + thickness, (p_rect->width - 2 * thickness), (p_rect->height - 2 * thickness), fill_color);
    }

    return NRF_SUCCESS;
}

void nrf_gfx_screen_fill(uint32_t color)
{
    rect_draw(0, 0, m_gfx.screen_width, m_gfx.screen_height, color);
}

void nrf_gfx_bmp565_draw(nrf_gfx_rect_t * p_rect, const uint16_t * img_buf)
{
    uint16_t pixel;
    int32_t i;
    uint32_t j;

    for (i = p_rect->height - 1; i >= 0; i--)
    {
        for (j = 0; j < p_rect->width; j++) 
        {
            pixel = (*(img_buf + (uint32_t)(i * p_rect->width + j)) >> 8) | (*(img_buf + (uint32_t)(i * p_rect->width + j)) << 8);
            pixel_draw(p_rect->x + j, p_rect->y + (p_rect->height - i), pixel);
        }
    }
}

void nrf_gfx_background_set(const uint16_t * img_buf)
{
    nrf_gfx_rect_t rectangle = 
    {
        .x = 0,
        .y = 0,
        .width = m_gfx.screen_width,
        .height = m_gfx.screen_height
    };
    nrf_gfx_bmp565_draw(&rectangle, img_buf);
}

void nrf_gfx_print(nrf_gfx_point_t * p_point,
                   uint16_t font_color,
                   const char * string,
                   const nrf_gfx_font_desc_t * p_font,
                   bool wrap)
{
    uint16_t x = p_point->x;
    uint16_t y = p_point->y;
    
    if ((x > (m_gfx.screen_width - p_font->width)) || (y > (m_gfx.screen_height - p_font->height)))
    {
        // Not enough space to write even single char.
        return;
    }

    for (uint8_t i = 0; ; i++)
    {
        if (string[i] == '\0')
        {
            return;
        }
        write_character(p_font, (uint8_t)string[i], x, y, font_color);
        x += p_font->width + p_font->space_size;
        if (x > (m_gfx.screen_width - p_font->width))
        {
            if (wrap)
            {
                y += p_font->height + p_font->line_break_size;
                x = p_point->x;
            }
            else
            {
                return;
            }
            
            if (y > (m_gfx.screen_height - p_font->height))
            {
                return;
            }
        }
    }
}
