/*
Copyright (c) 2017, Michal Wojcik, email: michal.t.wojcik@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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

#define BLE_UUID_PERSONAL_INFO_SERVICE       0x0020
#define BLE_UUID_PERSONAL_CALENDAR_CHAR      0x0021
#define BLE_UUID_PERSONAL_EMAIL_CHAR         0x0022
#define BLE_UUID_PERSONAL_WORK_ETA_CHAR      0x0023
#define BLE_UUID_PERSONAL_NAME_CHAR          0x0024
#define BLE_UUID_PERSONAL_HELLO_CHAR         0x0025
#define BLE_UUID_PERSONAL_COMPLIMENT         0x0026

#define BLE_UUID_CONFIGURATION_SERVICE       0xDEAD
#define BLE_UUID_CONFIGURATION_CHAR          0xBEEF

#define BLE_UUID_OUR_BASE_UUID              {0x6F, 0x74, 0x6B, 0x65, 0x6C, 0x66, 0x65, 0x72, 0x20, 0x73, 0x69, 0x20, 0x00, 0x00, 0x68, 0x74}


typedef  struct
{
   uint16_t service_handle;
   uint16_t conn_handle;
   ble_gatts_char_handles_t char_handles;
} ble_os_t;

extern bool has_permission_to_write;

void disconnect_peripheral(); // from main.c

void time_service_init(ble_os_t * p_our_service);
void weather_service_init(ble_os_t * p_our_service);
void personal_info_service_init(ble_os_t * p_our_service);
void configuration_service_init(ble_os_t * p_our_service);

static void string_char_add(ble_os_t * p_our_service, ble_uuid_t char_uuid);
static void time_char_add(ble_os_t * p_our_service);
static void configuration_char_add(ble_os_t * p_our_service);

void time_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt);
void weather_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt);
void personal_info_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt);
void configuration_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt);

static void on_time_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt);
static void on_string_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt);
static void configuration_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt);

#endif