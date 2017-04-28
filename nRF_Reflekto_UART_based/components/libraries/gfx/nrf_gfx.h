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

#ifndef NRF_GFX_H__
#define NRF_GFX_H__

#include <stdint.h>
#include <stdbool.h>
#include "sdk_errors.h"

/**
 * @brief GFX point object structure.
 */
typedef struct
{
    uint16_t x;                 /**< Horizontal coordinate of point where to start drawing object. */
    uint16_t y;                 /**< Vertical coordinate of point where to start drawing object. */
}nrf_gfx_point_t;

/**
 * @brief GFX line object structure.
 */
typedef struct
{
    uint16_t x_start;           /**< Horizontal coordinate of point where to start drawing object. */
    uint16_t y_start;           /**< Vertical coordinate of point where to start drawing object. */
    uint16_t x_end;             /**< Horizontal coordinate of point where to end drawing object. */
    uint16_t y_end;             /**< Vertical coordinate of point where to end drawing object. */
    uint16_t thickness;         /**< Thickness of the border of the object. */
}nrf_gfx_line_t;

/**
 * @brief GFX circle object structure.
 */
typedef struct
{
    uint16_t x;                 /**< Horizontal coordinate of the centre of the object. */
    uint16_t y;                 /**< Vertical coordinate of the centre of the object. */
    uint16_t r;                 /**< Radius of the object. */
}nrf_gfx_circle_t;

/**
 * @brief GFX rectangle object structure.
 */
typedef struct
{
    uint16_t x;                 /**< Horizontal coordinate of point where to start drawing object. */
    uint16_t y;                 /**< Vertical coordinate of point where to start drawing object. */
    uint16_t width;             /**< Width of the object. */
    uint16_t height;            /**< Height of the object. */
}nrf_gfx_rect_t;

/**
 * @defgroup nrf_gfx_macros Set of macros helping defining new graphic objects.
 * @{
 */
#define NRF_GFX_POINT(_x, _y)   \
    {                           \
        .x = _x,                \
        .y = _y                 \
    }

#define NRF_GFX_LINE(_x_0, _y_0, _x_1, _y_1, _thickness)    \
    {                                               \
        .x_start = _x_0,                            \
        .y_start = _y_0,                            \
        .x_end = _x_1,                              \
        .y_end = _y_1,                              \
        .thickness = _thickness                     \
    }

#define NRF_GFX_CIRCLE(_x, _y, _radius)     \
    {                                       \
        .x = _x,                            \
        .y = _y,                            \
        .r = _radius                        \
    }

#define NRF_GFX_RECT(_x, _y, _width, _height)  \
    {                                       \
        .x = _x,                            \
        .y = _y,                            \
        .width = _width,                    \
        .height = _height                   \
    }
/* @} */

/**
 * @brief Enumarator defining font types.
 */
typedef enum
{  
    FONT5X7 = 0,        /**< Font of size 5x7 pixels. */
    FONT10X16           /**< Font of size 10x16 pixels. */
}nrf_gfx_font_t;

/**
 * @brief Structure holding font description.
 */ 
typedef struct
{
    uint8_t ascii_offset;           /**< Offset between first sign defined in array 
                                            between first sign in ascii tables. */
    uint16_t width;                 /**< Font width in pixels. */
    uint16_t height;                /**< Font height in pixels. */
    uint16_t space_size;            /**< Length of break between next signs in pixels. */
    uint16_t line_break_size;       /**< Length of break between next lines in pixels. */
    nrf_gfx_font_t font_type;       /**< Enumarator defining font types. */
    const unsigned char * p_data;   /**< Pointer to array with font. */
}nrf_gfx_font_desc_t;

/**
 * @brief GFX lcd initialization function type.
 */
typedef ret_code_t (* lcd_init_t)(void);

/**
 * @brief GFX point draw function type.
 */
typedef void (* point_draw_t)(uint16_t x,
                             uint16_t y,
                             uint32_t color);

/**
 * @brief GFX rectangle draw function type.
 */
typedef void (* rect_draw_t)(uint16_t x,
                             uint16_t y,
                             uint16_t width,
                             uint16_t height,
                             uint32_t color);

/**
 * @brief GFX initialization structure.
 */
typedef struct
{
    lcd_init_t lcd_init;            /**< GFX lcd initialization function type. */
    point_draw_t point_draw;        /**< GFX point draw function type. */
    rect_draw_t rect_draw;          /**< GFX rectangle draw function type. */
    uint16_t screen_width;          /**< Screen width. */
    uint16_t screen_height;         /**< SCreen height. */
}nrf_gfx_config_t;

/**
 * @brief Function for initialzing GFX library.
 *
 * @param[in] p_config          Pointer to library configuration.
 */
ret_code_t nrf_gfx_init(nrf_gfx_config_t * p_config);

/**
 * @brief Function for drawing a point.
 *
 * @param[in] p_point           Pointer to point object.
 * @param[in] color             Color of the object in the display accepted format.
 */
void nrf_gfx_point_draw(nrf_gfx_point_t * p_point, uint32_t color);

/**
 * @brief Function for drawing a line.
 *
 * @param[in] p_line            Pointer to line object.
 * @param[in] color             Color of the object in the display accepted format.
 */
void nrf_gfx_line_draw(nrf_gfx_line_t * p_line, uint32_t color);

/**
 * @brief Function for drawing a circle.
 *
 * @param[in] p_circle          Pointer to circle object.
 * @param[in] color             Color of the object in the display accepted format.
 * @param[in] fill              If true, cricle will be filled.
 */
void nrf_gfx_circle_draw(nrf_gfx_circle_t * p_circle,
                         uint32_t color,
                         bool fill);

/**
 * @brief Function for drawing a rectangle.
 *
 * @param[in] p_rect            Pointer to rectangle object.
 * @param[in] thickness         Thickness of the rectangle border.
 * @param[in] color             Color of the border in the display accepted format.
 * @param[in] fill              If true, rectangle will be filled.
 * @param[in] color             Color of the filling in RGB-565 format.
 */
ret_code_t nrf_gfx_rect_draw(nrf_gfx_rect_t * p_rect,
                             uint16_t thickness,
                             uint32_t color,
                             bool fill,
                             uint32_t fill_color);

/**
 * @brief Function for filling screen with selected color.
 * 
 * @param[in] color             Color of in the display accepted format.
 */
void nrf_gfx_screen_fill(uint32_t color);

/**
 * @brief Function for drawing an image from bmp file.
 *
 * Data in img_buf is expected to be stored in 2-bytes samples, little endianness, RGB-565 format.
 * Pointer should skip header and point to first byte of data.
 *
 * @param[in] p_rect            Pointer to rectangle object.
 * @param[in] img_buf           Pointer to data from bmp file.
 *
 * @note Only compatibile with display accspting pixels in RGB-565 format.
 */
void nrf_gfx_bmp565_draw(nrf_gfx_rect_t * p_rect, const uint16_t * img_buf);

/**
 * @brief Function for drawing an image from bmp file.
 *
 * Data in img_buf is expected to be stored in 2-bytes samples, little endianness, RGB-565 format.
 * Pointer should skip header and point to first byte of data.
 *
 * @param[in] img_buf           Pointer to data from bmp file.
 *
 * @note Only compatibile with display accspting pixels in RGB-565 format.
 */
void nrf_gfx_background_set(const uint16_t * img_buf);

/**
 * @brief Function for printing string to the screen.
 *
 * @param[in] p_point           Pointer to point where to start drawing object.
 * @param[in] font_color        Color of the font in the display accepted format.
 * @param[in] p_string          Pointer to string.
 * @param[in] p_font            Pointer to font descriptor.
 * @param[in] wrap              If true, string will be wraped to the new line.
 */
void nrf_gfx_print(nrf_gfx_point_t * p_point,
                   uint16_t font_color,
                   const char * p_string,
                   const nrf_gfx_font_desc_t * p_font,
                   bool wrap);

#endif //NRF_GFX_H__
