#include "reflekto_display.h"
#include "reflekto_ble_services.h"
#include "reflekto_timers.h"
#include "nrf_gfx.h"

#define GRAY            0xC618
#define RED             0xF800
#define BLUE            0x001F

#define LINE_STEP       10

#define CIRCLE_RADIUS   10
#define CIRCLE_STEP     ((2 * CIRCLE_RADIUS) + 1)

#define BORDER          2

static const char * test_text = "This is nRF52.";

extern const nrf_gfx_font_desc_t microsoftSansSerif_8ptFontInfo;
extern const nrf_gfx_font_desc_t microsoftSansSerif_16ptFontInfo;
extern const nrf_lcd_t nrf_lcd_ili9341;
extern const nrf_lcd_t nrf_lcd_st7735;

static const nrf_gfx_font_desc_t * p_font = &microsoftSansSerif_16ptFontInfo;
static const nrf_lcd_t * p_lcd = &nrf_lcd_ili9341;

void gfx_initialization(void)
{
    APP_ERROR_CHECK(nrf_gfx_init(p_lcd));
}

static void brackground_set(void)
{
    nrf_gfx_invert(p_lcd, true);
    //nrf_gfx_background_set(p_lcd, nrf52);
    nrf_gfx_invert(p_lcd, false);
}

static void text_print(void)
{
    nrf_gfx_point_t text_start = NRF_GFX_POINT(10, nrf_gfx_height_get(p_lcd) - 30);
    APP_ERROR_CHECK(nrf_gfx_print(p_lcd, &text_start, 0, test_text, p_font, true));
}

void screen_clear(void)
{
    nrf_gfx_screen_fill(p_lcd, GRAY);
}

static void line_draw(void)
{
    nrf_gfx_line_t my_line = NRF_GFX_LINE(0, 0, 0, nrf_gfx_height_get(p_lcd), 2);
    nrf_gfx_line_t my_line_2 = NRF_GFX_LINE(nrf_gfx_width_get(p_lcd), nrf_gfx_height_get(p_lcd), 0, nrf_gfx_height_get(p_lcd), 1);

    for (uint16_t i = 0; i <= nrf_gfx_width_get(p_lcd); i += LINE_STEP)
    {
        my_line.x_end = i;
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line, RED));
    }

    my_line.x_end = nrf_gfx_width_get(p_lcd);

    for (uint16_t i = 0; i <= nrf_gfx_height_get(p_lcd); i += LINE_STEP)
    {
        my_line.y_end = (nrf_gfx_height_get(p_lcd) - i);
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line, RED));
    }

    for (uint16_t i = 0; i <= nrf_gfx_height_get(p_lcd); i += LINE_STEP)
    {
        my_line_2.y_end = (nrf_gfx_height_get(p_lcd) - i);
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line_2, BLUE));
    }

    my_line_2.y_end = 0;

    for (uint16_t i = 0; i <= nrf_gfx_width_get(p_lcd); i += LINE_STEP)
    {
        my_line_2.x_end = i;
        APP_ERROR_CHECK(nrf_gfx_line_draw(p_lcd, &my_line_2, BLUE));
    }
}

static void circle_draw(void)
{
    nrf_gfx_circle_t my_circle = NRF_GFX_CIRCLE(0, 0, CIRCLE_RADIUS);

    for (uint16_t j = 0; j <= nrf_gfx_height_get(p_lcd); j += CIRCLE_STEP)
    {
        my_circle.y = j;
        for (uint16_t i = 0; i <= nrf_gfx_width_get(p_lcd); i += CIRCLE_STEP)
        {
            my_circle.x = i;
            APP_ERROR_CHECK(nrf_gfx_circle_draw(p_lcd, &my_circle, BLUE, true));
        }
    }

    for (uint16_t j = CIRCLE_RADIUS; j <= nrf_gfx_height_get(p_lcd) + CIRCLE_RADIUS; j += CIRCLE_STEP)
    {
        my_circle.y = j;
        for (uint16_t i = CIRCLE_RADIUS; i <= nrf_gfx_width_get(p_lcd) + CIRCLE_RADIUS; i += CIRCLE_STEP)
        {
            my_circle.x = i;
            APP_ERROR_CHECK(nrf_gfx_circle_draw(p_lcd, &my_circle, RED, false));
        }
    }
}

static void rect_draw(void)
{
    nrf_gfx_rect_t my_rect = NRF_GFX_RECT(nrf_gfx_width_get(p_lcd) / 2,
                             nrf_gfx_height_get(p_lcd) / nrf_gfx_width_get(p_lcd),
                             nrf_gfx_height_get(p_lcd),
                             BORDER);
    nrf_gfx_rect_t my_rect_fill = NRF_GFX_RECT(nrf_gfx_width_get(p_lcd) / 2,
                                  nrf_gfx_height_get(p_lcd) / nrf_gfx_width_get(p_lcd),
                                  nrf_gfx_height_get(p_lcd),
                                  BORDER);

    nrf_gfx_rotation_set(p_lcd, NRF_LCD_ROTATE_90);

    for (uint16_t i = 0, j = 0;
        i <= (nrf_gfx_width_get(p_lcd) - (2 * BORDER)) / 2 &&
        j <= (nrf_gfx_height_get(p_lcd) - (2 * BORDER)) / 2;
        i += 6, j += 8)
    {
        my_rect.x = i;
        my_rect.y = j;
        my_rect_fill.x = i + BORDER;
        my_rect_fill.y = j + BORDER;
        my_rect.width = nrf_gfx_width_get(p_lcd) - i * 2;
        my_rect.height = nrf_gfx_height_get(p_lcd) - j * 2;
        my_rect_fill.width = nrf_gfx_width_get(p_lcd) - i * 2 - (2 * BORDER);
        my_rect_fill.height = nrf_gfx_height_get(p_lcd) - j * 2 - (2 * BORDER);

        // Draw using pseudo-random colors.
        APP_ERROR_CHECK(nrf_gfx_rect_draw(p_lcd, &my_rect, 2, ((i + j) * 10), false));
        APP_ERROR_CHECK(nrf_gfx_rect_draw(p_lcd, &my_rect_fill, 2, (UINT16_MAX - (i + j) * 10), true));
    }

    nrf_gfx_rotation_set(p_lcd, NRF_LCD_ROTATE_0);

}