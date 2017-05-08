#ifndef  REFLEKTO_BLE_SERVICES_H__
#define  REFLEKTO_BLE_SERVICES_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"
#include <time.h>


#define CLOCK_INTERVAL APP_TIMER_TICKS(1000)
#define BLE_UUID_TIME_SERVICE                0x1805
#define BLE_UUID_WEATHER_SERVICE             0x0010
#define BLE_UUID_WEATHER_CHAR_CITY           0x0011
#define BLE_UUID_WEATHER_CHAR_WIND           0x0012
#define BLE_UUID_WEATHER_CHAR_ADV            0x0013

#define BLE_UUID_OUR_BASE_UUID              {0x6F, 0x74, 0x6B, 0x65, 0x6C, 0x66, 0x65, 0x72, 0x20, 0x73, 0x69, 0x20, 0x00, 0x00, 0x68, 0x74}

typedef  struct
{
   uint16_t service_handle;
   uint16_t conn_handle;
   ble_gatts_char_handles_t char_handles;
} ble_os_t;


void time_service_init(ble_os_t * p_our_service);
void weather_service_init(ble_os_t * p_our_service);
static void weather_char_add(ble_os_t * p_our_service, ble_uuid_t char_uuid);
static void time_char_add(ble_os_t * p_our_service);
void time_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt);
void weather_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt);
static void on_time_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt);
static void on_weather_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt);

#endif