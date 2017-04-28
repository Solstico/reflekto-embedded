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

#ifndef ILI9341_H__
#define ILI9341_H__

#include <stdint.h>
#include <stdbool.h>
#include "app_util_platform.h"
#include "sdk_errors.h"

#define ILI9341_TFTWIDTH    240
#define ILI9341_TFTHEIGHT   320

/**
 * @defgroup ili9341_defines Set of commands described in ili9341 datasheet.
 * @{
 */
#define ILI9341_NOP         0x00
#define ILI9341_SWRESET     0x01
#define ILI9341_RDDID       0x04
#define ILI9341_RDDST       0x09

#define ILI9341_SLPIN       0x10
#define ILI9341_SLPOUT      0x11
#define ILI9341_PTLON       0x12
#define ILI9341_NORON       0x13

#define ILI9341_RDMODE      0x0A
#define ILI9341_RDMADCTL    0x0B
#define ILI9341_RDPIXFMT    0x0C
#define ILI9341_RDIMGFMT    0x0D
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF      0x20
#define ILI9341_INVON       0x21
#define ILI9341_GAMMASET    0x26
#define ILI9341_DISPOFF     0x28
#define ILI9341_DISPON      0x29

#define ILI9341_CASET       0x2A
#define ILI9341_PASET       0x2B
#define ILI9341_RAMWR       0x2C
#define ILI9341_RAMRD       0x2E

#define ILI9341_PTLAR       0x30
#define ILI9341_MADCTL      0x36
#define ILI9341_PIXFMT      0x3A

#define ILI9341_FRMCTR1     0xB1
#define ILI9341_FRMCTR2     0xB2
#define ILI9341_FRMCTR3     0xB3
#define ILI9341_INVCTR      0xB4
#define ILI9341_DFUNCTR     0xB6

#define ILI9341_PWCTR1      0xC0
#define ILI9341_PWCTR2      0xC1
#define ILI9341_PWCTR3      0xC2
#define ILI9341_PWCTR4      0xC3
#define ILI9341_PWCTR5      0xC4
#define ILI9341_VMCTR1      0xC5
#define ILI9341_VMCTR2      0xC7
#define ILI9341_PWCTRSEQ    0xCB
#define ILI9341_PWCTRA      0xCD
#define ILI9341_PWCTRB      0xCF

#define ILI9341_RDID1       0xDA
#define ILI9341_RDID2       0xDB
#define ILI9341_RDID3       0xDC
#define ILI9341_RDID4       0xDD

#define ILI9341_GMCTRP1     0xE0
#define ILI9341_GMCTRN1     0xE1
#define ILI9341_DGMCTR1     0xE2
#define ILI9341_DGMCTR2     0xE3
#define ILI9341_TIMCTRA     0xE8
#define ILI9341_TIMCTRB     0xEA

#define ILI9341_ENGMCTR     0xF2
#define ILI9341_INCTR       0xF6
#define ILI9341_PUMP        0xF7
/* @} */

/**
 * @brief Enumarator with available rotations.
 */
typedef enum{
    ILI9341_ROTATE_90 = 0,       /**< Rotate 90 deegres, clockwise. */
    ILI9341_ROTATE_180,          /**< Rotate 180 deegres, clockwise. */
    ILI9341_ROTATE_270           /**< Rotate 270 deegres, clockwise. */
}ili9341_rotation_t;

/**
 * @brief Function for converting 24-bit pixel into RGB-565 pixel.
 */ 
__STATIC_INLINE uint16_t ili9341_24bit_to_color565(uint8_t r, uint8_t g, uint8_t b) 
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/**
 * @brief Function for initalizing ili9341 controller.
 */
ret_code_t ili9341_init(void);

/**
 * @brief Function for uninitalizing ili9341 controller.
 */
void ili9341_uninit(void);

/**
 * @brief Function for drawing a single pixel.
 *
 * @param[in] x             Horizontal coordinate of pixel.
 * @param[in] y             Vertical coordinate of pixel.
 * @param[in] color         Color of the pixel in RGB-565 format.
 */
void ili9341_pixel_draw(int16_t x, int16_t y, uint16_t color);

/**
 * @brief Function for drawing rectangle.
 * 
 * @param[in] x             Horizontal coordinate of point where to start drawing rectangle.
 * @param[in] y             Vertical coordinate of point where to start drawing rectangle.
 * @param[in] width         Width of image.
 * @param[in] height        Height of image.
 * @param[in] color         Color to fill rectangle in RGB-565 format.
 */
void ili9341_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) ;

/**
 * @brief Function for drawing rectangled shape objects from buffer.
 *
 * @param[in] buffer        Pointer to buffer with RGB-565 pixels in it.
 * @param[in] x             Horizontal coordinate of point where to start drawing image.
 * @param[in] y             Vertical coordinate of point where to start drawing image.
 * @param[in] width         Width of image.
 * @param[in] height        Height of image.
 */
void ili9341_buffer_draw(const uint16_t * buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height);

/**
 * @brief Function for rotating screen.
 *
 * @param[in] rotation      Rotation as enumared value.
 */
void ili9341_rotation_set(ili9341_rotation_t rotation);

/**
 * @brief Function for setting inversion of colors on the screen.
 *
 * @param[in] invert        If true, inversion will be set.
 */
void ili9341_display_invert(bool invert);

#endif // ILI9341_H__
