/*Copyright (c) 2017, Michal Wojcik, email: michal.t.wojcik@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdint.h>
#include <string.h>

#include "reflekto_ble_services.h"
#include "boards.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "app_timer.h"
#include "reflekto_timers.h"
#include "reflekto_data_collector.h"
#include "nrf_log.h"

// EVENT HANDLING
static void on_time_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt)
{
    if(!has_permission_to_write)
    {
        disconnect_peripheral();
        return;
    }
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    NRF_LOG_INFO("Time was written\n"); // Print message to RTT to the application flow
    if(p_evt_write->len > 0)
    {
	NRF_LOG_INFO("%d\n",p_evt_write->len);
	NRF_LOG_INFO("UUID: %x\n", p_evt_write->uuid.uuid);

        time_t received_time = 0;
        received_time |= p_evt_write->data[3];
        received_time <<= 8;
        received_time |= p_evt_write->data[2];
        received_time <<= 8;
        received_time |= p_evt_write->data[1];
        received_time <<= 8;
        received_time |= p_evt_write->data[0];
        NRF_LOG_INFO("Received time: %d\r\n", received_time);
        set_current_time(received_time);
    }
}

void time_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			break;
		case BLE_GAP_EVT_DISCONNECTED:
			p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
                        break;
		case BLE_GATTS_EVT_WRITE:
                        if(p_ble_evt->evt.gatts_evt.params.write.uuid.uuid == BLE_UUID_CURRENT_TIME_CHAR)
                            on_time_char_write(p_our_service,p_ble_evt);
			break;
		default:
			break;
	}
}
static void on_string_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    switch (p_evt_write->uuid.uuid)
    {
        case BLE_UUID_WEATHER_CHAR_CITY:
            update_collected_string(p_evt_write->data,p_evt_write->len,CITY);
            break;
        case BLE_UUID_WEATHER_CHAR_WIND:
            update_collected_string(p_evt_write->data,p_evt_write->len,WIND);
            break;
        case BLE_UUID_WEATHER_CHAR_ADV:
            update_collected_string(p_evt_write->data,p_evt_write->len,ADVICE);
            break;
        case BLE_UUID_PERSONAL_CALENDAR_CHAR:
            update_collected_string(p_evt_write->data,p_evt_write->len,CALENDAR);
            break;
        case BLE_UUID_PERSONAL_EMAIL_CHAR:
            update_collected_string(p_evt_write->data,p_evt_write->len,EMAIL);
            break;
        case BLE_UUID_PERSONAL_HELLO_CHAR:
            update_collected_string(p_evt_write->data,p_evt_write->len,HELLO);
            break;
        case BLE_UUID_PERSONAL_NAME_CHAR:
            update_collected_string(p_evt_write->data,p_evt_write->len,NAME);
            break;
        case BLE_UUID_PERSONAL_WORK_ETA_CHAR:
            update_collected_string(p_evt_write->data,p_evt_write->len,WORK_ETA);
            break;
        case BLE_UUID_PERSONAL_COMPLIMENT:
            update_collected_string(p_evt_write->data,p_evt_write->len,COMPLIMENT);
            break;
        default:
            break;
    }
}

static void configuration_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    if( p_evt_write->data[0] == 0 &&
        p_evt_write->data[1] == 0 &&
        p_evt_write->data[2] == 1 &&
        p_evt_write->data[3] == 2 &&
        p_evt_write->data[4] == 2 &&
        p_evt_write->data[5] == 0
    ) // password that allows writing to Reflekto is: 001220
        {
            has_permission_to_write = true;
            disconnect_timer_stop();
            return;
        }
    // command used to disconnect from peripheral is 666
    if( p_evt_write->data[0] == 6 && p_evt_write->data[1] == 6 && p_evt_write->data[2] == 6)
    {
        has_permission_to_write = false;
        disconnect_peripheral();
    }
}

void weather_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			break;
		case BLE_GAP_EVT_DISCONNECTED:
			p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
                        break;
		case BLE_GATTS_EVT_WRITE:
                        on_string_char_write(p_our_service,p_ble_evt); // Here happens the magic for handling string writes
			break;
		default:
			break;
	}
}

void personal_info_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
			p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			break;
		case BLE_GAP_EVT_DISCONNECTED:
			p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
                        break;
		default:
			break;
	}
}

void configuration_service_on_ble_evt(ble_os_t *p_our_service, ble_evt_t *p_ble_evt)
{
	switch (p_ble_evt->header.evt_id)
	{
		case BLE_GAP_EVT_CONNECTED:
                    p_our_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
                    break;
		case BLE_GAP_EVT_DISCONNECTED:
                    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
                    break;
                case BLE_GATTS_EVT_WRITE:
                    if(p_ble_evt->evt.gatts_evt.params.write.uuid.uuid == BLE_UUID_CONFIGURATION_CHAR){
                        configuration_char_write(p_our_service,p_ble_evt);
                    }
                    break;
		default:
                    break;
	}
}

// SERVICES INIT
void time_service_init(ble_os_t * p_our_service)
{
    uint32_t err_code;
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
    service_uuid.uuid = BLE_UUID_TIME_SERVICE;

    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&service_uuid,&p_our_service->service_handle);
    APP_ERROR_CHECK(err_code);

    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    NRF_LOG_INFO("Exectuing time_service_init()\r\n"); // Print message to RTT to the application flow
    NRF_LOG_INFO("Service UUID: 0x%#04x\r\n", service_uuid.uuid); // Print service UUID should match definition BLE_UUID_OUR_SERVICE
    NRF_LOG_INFO("Service UUID type: 0x%#02x\r\n", service_uuid.type); // Print UUID type. Should match BLE_UUID_TYPE_VENDOR_BEGIN. Search for BLE_UUID_TYPES in ble_types.h for more info
    NRF_LOG_INFO("Service handle: 0x%#04x\r\n", p_our_service->service_handle); // Print out the service handle. Should match service handle shown in MCP under Attribute values
    NRF_LOG_INFO("End of time service init\r\n");
    time_char_add(p_our_service);
}

void weather_service_init(ble_os_t * p_our_service)
{
    uint32_t err_code;
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
    service_uuid.uuid = BLE_UUID_WEATHER_SERVICE;

    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&service_uuid,&p_our_service->service_handle);
    APP_ERROR_CHECK(err_code);

    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    NRF_LOG_INFO("Exectuing weather_service_init()\n"); // Print message to RTT to the application flow
    NRF_LOG_INFO("Service UUID: 0x%#04x\n", service_uuid.uuid); // Print service UUID should match definition BLE_UUID_OUR_SERVICE
    NRF_LOG_INFO("Service UUID type: 0x%#02x\n", service_uuid.type); // Print UUID type. Should match BLE_UUID_TYPE_VENDOR_BEGIN. Search for BLE_UUID_TYPES in ble_types.h for more info
    NRF_LOG_INFO("Service handle: 0x%#04x\n", p_our_service->service_handle); // Print out the service handle. Should match service handle shown in MCP under Attribute values
    NRF_LOG_INFO("End of time service init\n");
    ble_uuid_t char_uuid;
    char_uuid.uuid = BLE_UUID_WEATHER_CHAR_CITY;
    string_char_add(p_our_service,char_uuid);
    char_uuid.uuid = BLE_UUID_WEATHER_CHAR_WIND;
    string_char_add(p_our_service,char_uuid);
    char_uuid.uuid = BLE_UUID_WEATHER_CHAR_ADV;
    string_char_add(p_our_service,char_uuid);
}

void personal_info_service_init(ble_os_t * p_our_service)
{
    uint32_t err_code;
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
    service_uuid.uuid = BLE_UUID_PERSONAL_INFO_SERVICE;

    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&service_uuid,&p_our_service->service_handle);
    APP_ERROR_CHECK(err_code);

    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    NRF_LOG_INFO("Exectuing personal_service_init()\n"); // Print message to RTT to the application flow
    NRF_LOG_INFO("Service UUID: 0x%#04x\n", service_uuid.uuid); // Print service UUID should match definition BLE_UUID_OUR_SERVICE
    NRF_LOG_INFO("Service UUID type: 0x%#02x\n", service_uuid.type); // Print UUID type. Should match BLE_UUID_TYPE_VENDOR_BEGIN. Search for BLE_UUID_TYPES in ble_types.h for more info
    NRF_LOG_INFO("Service handle: 0x%#04x\n", p_our_service->service_handle); // Print out the service handle. Should match service handle shown in MCP under Attribute values
    NRF_LOG_INFO("End of time service init\n");

    ble_uuid_t char_uuid;
    char_uuid.uuid = BLE_UUID_PERSONAL_CALENDAR_CHAR;
    string_char_add(p_our_service,char_uuid);
    char_uuid.uuid = BLE_UUID_PERSONAL_EMAIL_CHAR;
    string_char_add(p_our_service,char_uuid);
    char_uuid.uuid = BLE_UUID_PERSONAL_WORK_ETA_CHAR;
    string_char_add(p_our_service,char_uuid);
    char_uuid.uuid = BLE_UUID_PERSONAL_NAME_CHAR;
    string_char_add(p_our_service,char_uuid);
    char_uuid.uuid = BLE_UUID_PERSONAL_HELLO_CHAR;
    string_char_add(p_our_service,char_uuid);
    char_uuid.uuid = BLE_UUID_PERSONAL_COMPLIMENT;
    string_char_add(p_our_service,char_uuid);

}

void configuration_service_init(ble_os_t * p_our_service)
{
    uint32_t err_code;
    ble_uuid_t service_uuid;
    ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
    service_uuid.uuid = BLE_UUID_CONFIGURATION_SERVICE;

    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&service_uuid,&p_our_service->service_handle);
    APP_ERROR_CHECK(err_code);

    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
    NRF_LOG_INFO("Exectuing time_service_init()\r\n");
    NRF_LOG_INFO("Service UUID: 0x%#04x\r\n", service_uuid.uuid);
    NRF_LOG_INFO("Service UUID type: 0x%#02x\n", service_uuid.type);
    NRF_LOG_INFO("End of cofniguration service init\r\n");
    configuration_char_add(p_our_service);
}

// CHARACTERISTICS ADD
static void time_char_add(ble_os_t * p_our_service)
{
    // 2A - UUID
    uint32_t err_code;
    ble_uuid_t char_uuid;
    ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
    char_uuid.uuid = BLE_UUID_CURRENT_TIME_CHAR;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);

    // 2F - read/write properties
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;

    // 2B - Attribute Metadata
    ble_gatts_attr_md_t attr_md;
    memset (&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    // 2G - read/write permissions
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    // 2C - Characteristic Value Attribue
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;

    // 2H - Characteristic length
    attr_char_value.max_len     = 4;
    attr_char_value.init_len    = 4;
    uint8_t value[4]            = {0x00,0x00,0x00,0x00};
    attr_char_value.p_value     = value;

    // 2E - Add characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                   &char_md,
                                   &attr_char_value,
                                   &p_our_service->char_handles);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Characteristic was added to time service\n");
}

static void string_char_add(ble_os_t * p_our_service, ble_uuid_t char_uuid)
{
    // 2A - UUID
    uint32_t err_code;
    ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);

    // 2F - read/write properties
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 0;
    char_md.char_props.write = 1;

    // 2B - Attribute Metadata
    ble_gatts_attr_md_t attr_md;
    memset (&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    // 2G - read/write permissions
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    // 2C - Characteristic Value Attribue
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;

    // 2H - Characteristic length
    attr_char_value.max_len     = 20;
    attr_char_value.init_len    = 0;

    // 2E - Add characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                   &char_md,
                                   &attr_char_value,
                                   &p_our_service->char_handles);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Characteristic was added to string service\n");
}

static void configuration_char_add(ble_os_t * p_our_service)
{
    // 2A - UUID
    uint32_t err_code;
    ble_uuid_t char_uuid;
    ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
    char_uuid.uuid = BLE_UUID_CONFIGURATION_CHAR;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code);

    // 2F - read/write properties
    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 0;
    char_md.char_props.write = 1;

    // 2B - Attribute Metadata
    ble_gatts_attr_md_t attr_md;
    memset (&attr_md, 0, sizeof(attr_md));
    attr_md.vloc = BLE_GATTS_VLOC_STACK;

    // 2G - read/write permissions
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    // 2C - Characteristic Value Attribue
    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;

    // 2H - Characteristic length
    attr_char_value.max_len     = 6;
    attr_char_value.init_len    = 6;
    uint8_t value[6]            = {0x00,0x00,0x00,0x00,0x00,0x00};
    attr_char_value.p_value     = value;

    // 2E - Add characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                   &char_md,
                                   &attr_char_value,
                                   &p_our_service->char_handles);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("Characteristic was added to configuration service\n");
}
