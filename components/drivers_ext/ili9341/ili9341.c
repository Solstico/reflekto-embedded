/* Copyright (c) 2017 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 */

#include "ili9341.h"
#include "nrf_drv_spi.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(ILI9341_SPI_INSTANCE);

typedef struct
{
    uint16_t width;
    uint16_t height;
}ili9341_t;

static ili9341_t m_ili9341;

__STATIC_INLINE void spi_write(uint8_t byte)
{
    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, &byte, sizeof(byte), NULL, 0));
}

__STATIC_INLINE void write_command(uint8_t c)
{
    nrf_gpio_pin_clear(ILI9341_DC_PIN);
    spi_write(c);
}

__STATIC_INLINE void write_data(uint8_t c)
{
    nrf_gpio_pin_set(ILI9341_DC_PIN);
    spi_write(c);
}

__STATIC_INLINE void set_addr_window(uint16_t x_0, uint16_t y_0, uint16_t x_1, uint16_t y_1)
{
    ASSERT(x_0 < x_1);
    ASSERT(y_0 < y_1);

    write_command(ILI9341_CASET);
    write_data(x_0 >> 8);
    write_data(x_0);
    write_data(x_1 >> 8);
    write_data(x_1);
    write_command(ILI9341_PASET);
    write_data(y_0 >> 8);
    write_data(y_0);
    write_data(y_1 >> 8);
    write_data(y_1);
    write_command(ILI9341_RAMWR);
}

__STATIC_INLINE void command_list(void)
{
    write_command(ILI9341_PWCTRB);
    write_data(0x00);
    write_data(0XC1);
    write_data(0X30);

    write_command(ILI9341_TIMCTRA);
    write_data(0x85);
    write_data(0x00);
    write_data(0x78);

    write_command(ILI9341_PWCTRSEQ);
    write_data(0x39);
    write_data(0x2C);
    write_data(0x00);
    write_data(0x34);
    write_data(0x02);

    write_command(ILI9341_PUMP);
    write_data(0x20);

    write_command(ILI9341_TIMCTRB);
    write_data(0x00);
    write_data(0x00);

    write_command(ILI9341_PWCTR1);
    write_data(0x23);

    write_command(ILI9341_PWCTR2);
    write_data(0x10);

    write_command(ILI9341_VMCTR1);
    write_data(0x3e);
    write_data(0x28);

    write_command(ILI9341_VMCTR2);
    write_data(0x86);

    write_command(ILI9341_MADCTL);
    write_data(0x48);

    write_command(ILI9341_PIXFMT);
    write_data(0x55);

    write_command(ILI9341_FRMCTR1);
    write_data(0x00);
    write_data(0x18);

    write_command(ILI9341_DFUNCTR);
    write_data(0x08);
    write_data(0x82);
    write_data(0x27);

    write_command(ILI9341_ENGMCTR);
    write_data(0x00);

    write_command(ILI9341_GAMMASET);
    write_data(0x01);

    write_command(ILI9341_GMCTRP1);
    write_data(0x0F);
    write_data(0x31);
    write_data(0x2B);
    write_data(0x0C);
    write_data(0x0E);
    write_data(0x08);
    write_data(0x4E);
    write_data(0xF1);
    write_data(0x37);
    write_data(0x07);
    write_data(0x10);
    write_data(0x03);
    write_data(0x0E);
    write_data(0x09);
    write_data(0x00);

    write_command(ILI9341_GMCTRN1);
    write_data(0x00);
    write_data(0x0E);
    write_data(0x14);
    write_data(0x03);
    write_data(0x11);
    write_data(0x07);
    write_data(0x31);
    write_data(0xC1);
    write_data(0x48);
    write_data(0x08);
    write_data(0x0F);
    write_data(0x0C);
    write_data(0x31);
    write_data(0x36);
    write_data(0x0F);

    write_command(ILI9341_SLPOUT);
    nrf_delay_ms(120);
    write_command(ILI9341_DISPON);
}

ret_code_t ili9341_init(void)
{
    ret_code_t err_code = NRF_SUCCESS;

    m_ili9341.width = ILI9341_TFTWIDTH;
    m_ili9341.height = ILI9341_TFTHEIGHT;

    nrf_gpio_cfg_output(ILI9341_DC_PIN);

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;

    spi_config.sck_pin  = ILI9341_SCK_PIN;
    spi_config.miso_pin = ILI9341_MISO_PIN;
    spi_config.mosi_pin = ILI9341_MOSI_PIN;
    spi_config.ss_pin   = ILI9341_SS_PIN;

    err_code = nrf_drv_spi_init(&spi, &spi_config, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    command_list();

    return err_code;
}

void ili9341_uninit(void)
{
    nrf_drv_spi_uninit(&spi);
}

void ili9341_pixel_draw(int16_t x, int16_t y, uint16_t color)
{
    set_addr_window(x,y,x,y);

    nrf_gpio_pin_set(ILI9341_DC_PIN);

    spi_write(color >> 8);
    spi_write(color);

    nrf_gpio_pin_clear(ILI9341_DC_PIN);
}

void ili9341_buffer_draw(const uint16_t * buffer, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    uint16_t pixel;
    uint8_t hi;
    uint8_t lo;
    int32_t i;
    uint32_t j;

    set_addr_window(x, y, x + width - 1, y + height - 1);

    nrf_gpio_pin_set(ILI9341_DC_PIN);

    for (i = 0; i < height; i++)
    {
        for (j = 0; j < width; j++)
        {
            pixel = *(buffer + (uint32_t)(i * width + j));
            hi = pixel >> 8;
            lo = pixel;
            spi_write(hi);
            spi_write(lo);
        }
    }

    nrf_gpio_pin_clear(ILI9341_DC_PIN);
}

void ili9341_rect_draw(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color)
{
    uint8_t hi;
    uint8_t lo;

    set_addr_window(x, y, x + width - 1, y + height - 1);

    hi = color >> 8;
    lo = color;

    nrf_gpio_pin_set(ILI9341_DC_PIN);

    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            spi_write(hi);
            spi_write(lo);
        }
    }

    nrf_gpio_pin_clear(ILI9341_DC_PIN);
}

void ili9341_rotation_set(uint8_t rotation)
{
    uint32_t rot;
    write_command(ILI9341_MADCTL);
    rot = rotation % 4;                   // Can't be higher than 3.
    switch (rot) {
        case 0:
            write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR);
            m_ili9341.width  = ILI9341_TFTWIDTH;
            m_ili9341.height = ILI9341_TFTHEIGHT;
            break;
        case 1:
            write_data(ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
            m_ili9341.width  = ILI9341_TFTHEIGHT;
            m_ili9341.height = ILI9341_TFTWIDTH;
            break;
        case 2:
            write_data(ILI9341_MADCTL_MY | ILI9341_MADCTL_BGR);
            m_ili9341.width  = ILI9341_TFTWIDTH;
            m_ili9341.height = ILI9341_TFTHEIGHT;
            break;
        case 3:
            write_data(ILI9341_MADCTL_MX | ILI9341_MADCTL_MY | ILI9341_MADCTL_MV | ILI9341_MADCTL_BGR);
            m_ili9341.width  = ILI9341_TFTHEIGHT;
            m_ili9341.height = ILI9341_TFTWIDTH;
            break;
        default:
            break;
    }
}

void ili9341_display_invert(bool invert)
{
    write_command(invert ? ILI9341_INVON : ILI9341_INVOFF);
}
