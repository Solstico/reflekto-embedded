#include <stdint.h>
#include <string.h>

#include "reflekto_ble_services.h"
#include "SEGGER_RTT.h"
#include "boards.h"
#include "app_error.h"
#include "ble_srv_common.h"
#include "app_timer.h"
#include "reflekto_timers.h"
#include "reflekto_data_collector.h"


static void on_time_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    SEGGER_RTT_WriteString(0, "Time was written\n"); // Print message to RTT to the application flow
    if(p_evt_write->len > 0)
    {
	memset(&(p_evt_write->data[p_evt_write->len]),0,(20-p_evt_write->len)*sizeof(p_evt_write->data[0]));
	SEGGER_RTT_printf(0, "%d\n",p_evt_write->len);
	SEGGER_RTT_printf(0, "%\n",p_evt_write->data);
	SEGGER_RTT_printf(0, "UUID: %x\n", p_evt_write->uuid.uuid);

        time_t received_time = 0;
        received_time |= p_evt_write->data[0];
        received_time <<= 8;
        received_time |= p_evt_write->data[1];
        received_time <<= 8;
        received_time |= p_evt_write->data[2];
        received_time <<= 8;
        received_time |= p_evt_write->data[3];
        SEGGER_RTT_printf(0, "Received time: %d", received_time);
        if(received_time > 1494000000) //check if time is after 5.5.2017
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
		case BLE_GATTS_EVT_WRITE:
                        if(p_ble_evt->evt.gatts_evt.params.write.uuid.uuid == BLE_UUID_CURRENT_TIME_CHAR)
                            on_time_char_write(p_our_service,p_ble_evt);
			break;
		default:
			break;
	}
}
static void on_weather_char_write(ble_os_t *p_nus, ble_evt_t *p_ble_evt)
{
    ble_gatts_evt_write_t * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    switch (p_evt_write->uuid.uuid)
    {
        case BLE_UUID_WEATHER_CHAR_CITY:
            update_weather(p_evt_write->data,p_evt_write->len,CITY);
            break;
        case BLE_UUID_WEATHER_CHAR_WIND:
            update_weather(p_evt_write->data,p_evt_write->len,WIND);
            break;
        case BLE_UUID_WEATHER_CHAR_ADV:
            update_weather(p_evt_write->data,p_evt_write->len,ADVICE);
            break;
        default:
            break;
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
		case BLE_GATTS_EVT_WRITE:
                        on_weather_char_write(p_our_service,p_ble_evt);
			break;
		default:
			break;
	}
}

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

    // Print messages to Segger Real Time Terminal
    // UNCOMMENT THE FOUR LINES BELOW AFTER INITIALIZING THE SERVICE OR THE EXAMPLE WILL NOT COMPILE.
    SEGGER_RTT_WriteString(0, "Exectuing time_service_init()\n"); // Print message to RTT to the application flow
    SEGGER_RTT_printf(0, "Service UUID: 0x%#04x\n", service_uuid.uuid); // Print service UUID should match definition BLE_UUID_OUR_SERVICE
    SEGGER_RTT_printf(0, "Service UUID type: 0x%#02x\n", service_uuid.type); // Print UUID type. Should match BLE_UUID_TYPE_VENDOR_BEGIN. Search for BLE_UUID_TYPES in ble_types.h for more info
    SEGGER_RTT_printf(0, "Service handle: 0x%#04x\n", p_our_service->service_handle); // Print out the service handle. Should match service handle shown in MCP under Attribute values
    SEGGER_RTT_WriteString(0,"End of time service init\n");
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

    // Print messages to Segger Real Time Terminal
    // UNCOMMENT THE FOUR LINES BELOW AFTER INITIALIZING THE SERVICE OR THE EXAMPLE WILL NOT COMPILE.
    SEGGER_RTT_WriteString(0, "Exectuing weather_service_init()\n"); // Print message to RTT to the application flow
    SEGGER_RTT_printf(0, "Service UUID: 0x%#04x\n", service_uuid.uuid); // Print service UUID should match definition BLE_UUID_OUR_SERVICE
    SEGGER_RTT_printf(0, "Service UUID type: 0x%#02x\n", service_uuid.type); // Print UUID type. Should match BLE_UUID_TYPE_VENDOR_BEGIN. Search for BLE_UUID_TYPES in ble_types.h for more info
    SEGGER_RTT_printf(0, "Service handle: 0x%#04x\n", p_our_service->service_handle); // Print out the service handle. Should match service handle shown in MCP under Attribute values
    SEGGER_RTT_WriteString(0,"End of time service init\n");
    ble_uuid_t char_uuid;
    char_uuid.uuid = BLE_UUID_WEATHER_CHAR_CITY;
    weather_char_add(p_our_service,char_uuid);
    char_uuid.uuid = BLE_UUID_WEATHER_CHAR_WIND;
    weather_char_add(p_our_service,char_uuid);
    char_uuid.uuid = BLE_UUID_WEATHER_CHAR_ADV;
    weather_char_add(p_our_service,char_uuid);
}

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
    SEGGER_RTT_WriteString(0, "Characteristic was added to time service\n");
}

static void weather_char_add(ble_os_t * p_our_service, ble_uuid_t char_uuid)
{
    // 2A - UUID
    uint32_t err_code;
    ble_uuid128_t base_uuid = BLE_UUID_OUR_BASE_UUID;
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
    attr_char_value.max_len     = 20;
    attr_char_value.init_len    = 5;
    uint8_t value[5]            = {0x65,0x6D,0x70,0x74,0x79};
    attr_char_value.p_value     = value;

    // 2E - Add characteristic to the service
    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                   &char_md,
                                   &attr_char_value,
                                   &p_our_service->char_handles);
    APP_ERROR_CHECK(err_code);
    SEGGER_RTT_WriteString(0, "Characteristic was added to weather service\n");
}