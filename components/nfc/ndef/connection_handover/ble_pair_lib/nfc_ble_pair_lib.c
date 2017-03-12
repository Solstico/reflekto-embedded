/* Copyright (c) 2016 Nordic Semiconductor. All Rights Reserved.
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
 
 #include "sdk_config.h"
 #if NFC_BLE_PAIR_LIB_ENABLED

#include "nfc_ble_pair_lib.h"
#include "sdk_macros.h"
#include "app_error.h"
#include "nrf_drv_rng.h"
#include "nfc_t2t_lib.h"
#include "nfc_ble_pair_msg.h"
#include "ble_advertising.h"
#include "peer_manager.h"
#include "ecc.h"

#define NRF_LOG_MODULE_NAME "NFC_BLE_PAIR"
#if NFC_BLE_PAIR_LIB_LOG_ENABLED
#define NRF_LOG_LEVEL       NFC_BLE_PAIR_LIB_LOG_LEVEL
#define NRF_LOG_INFO_COLOR  NFC_BLE_PAIR_LIB_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR NFC_BLE_PAIR_LIB_DEBUG_COLOR
#else // NFC_BLE_PAIR_LIB_LOG_ENABLED
#define NRF_LOG_LEVEL       0
#endif // NFC_BLE_PAIR_LIB_LOG_ENABLED
#include "nrf_log.h"

// Verify bonding and keys distribution settings.
#if ((BLE_NFC_SEC_PARAM_BOND)           &&  \
    !(BLE_NFC_SEC_PARAM_KDIST_OWN_ENC)  &&  \
    !(BLE_NFC_SEC_PARAM_KDIST_OWN_ID)   &&  \
    !(BLE_NFC_SEC_PARAM_KDIST_PEER_ENC) &&  \
    !(BLE_NFC_SEC_PARAM_KDIST_PEER_ID))
    #error "At least one of the BLE_NFC_SEC_PARAM_KDIST flags must be set to 1 when bonding is enabled."
#endif

// Macro for verifying if the pairing mode argument is valid
#define VERIFY_PAIRING_MODE(arg)         \
    if((arg) >= NFC_PAIRING_MODE_CNT)    \
    {                                    \
        return NRF_ERROR_INVALID_PARAM;  \
    }

#define BLE_GAP_LESC_P256_SK_LEN   32                                /**< GAP LE Secure Connections Elliptic Curve Diffie-Hellman P-256 Secret Key Length. */
#define TK_MAX_NUM                 1                                 /**< Maximal number of TK locations in NDEF message buffer. */
#define NDEF_MSG_BUFF_SIZE         256                               /**< Size of buffer for the NDEF pairing message */

#define BLE_NFC_SEC_PARAM_KEYPRESS 0                                 /**< Keypress notifications not enabled. */
#define BLE_NFC_SEC_PARAM_IO_CAPS  BLE_GAP_IO_CAPS_NONE              /**< No I/O capabilities. */

typedef struct
{
    uint8_t sk[BLE_GAP_LESC_P256_SK_LEN];                            /**< LE Secure Connections Elliptic Curve Diffie-Hellman P-256 Secret Key */
} ble_gap_lesc_p256_sk_t;

static uint8_t                   m_ndef_msg_buf[NDEF_MSG_BUFF_SIZE]; /**< NFC tag NDEF message buffer. */ 
static ble_advdata_tk_value_t    m_oob_auth_key;                     /**< Temporary Key buffer used in OOB legacy pairing mode. */ 
static uint8_t *                 m_tk_group[TK_MAX_NUM];             /**< Locations of TK in NDEF message. */                                     
static nfc_pairing_mode_t        m_pairing_mode;                     /**< Current pairing mode. */ 
static ble_gap_lesc_oob_data_t   m_ble_lesc_oob_data;                /**< LESC OOB data used in LESC OOB pairing mode */

static bool     m_connected         = false;                         /**< Indicates if device is connected */
static uint16_t m_conn_handle       = BLE_CONN_HANDLE_INVALID;       /**< Handle of the current connection */
static bool     m_pending_advertise = false;                         /**< Flag used to indicate pending advertising that will be start after disconnection */

__ALIGN(4) static ble_gap_lesc_p256_pk_t m_lesc_pk;                  /**< LESC ECC Public Key */
__ALIGN(4) static ble_gap_lesc_p256_sk_t m_lesc_sk;                  /**< LESC ECC Secret Key */
__ALIGN(4) static ble_gap_lesc_dhkey_t   m_lesc_dhkey;               /**< LESC ECC DH Key */

/**
 * @brief Generates random values to a given buffer
 *
 * @param[out] p_buff Buffer for random values
 * @param[in]  size   Number of bytes to generate
 *
 * @returns    Number of generated bytes
 */
static uint8_t random_vector_generate(uint8_t * p_buff, uint8_t size)
{
    uint8_t    available;
    ret_code_t err_code = NRF_SUCCESS;
    
    nrf_drv_rng_bytes_available(&available);
    
    uint8_t length = (size < available) ? size : available;
    err_code = nrf_drv_rng_rand(p_buff, length);
    APP_ERROR_CHECK(err_code);
    return length;
}

/**
 * @brief Prints generated key to the log console
 *
 * @param[in] lenght TK value length
 */
static void random_vector_log(uint8_t length)
{
    NRF_LOG_INFO("TK Random Value:");
    for (uint8_t i = 0; i < length; i++)
    {
        NRF_LOG_RAW_INFO(" %02X",(int)m_oob_auth_key.tk[i]);
    }
    NRF_LOG_RAW_INFO("\r\n");
}

/**
 * @brief Function handling NFC events
 *
 * @details Starts advertising and generates new OOB keys on the NFC_T2T_EVENT_FIELD_ON event.
 *
 * @param[in] p_context    Context for callback execution, not used in this callback implementation.
 * @param[in] event        Event generated by hal NFC lib.
 * @param[in] p_data       Received/transmitted data or NULL, not used in this callback implementation.
 * @param[in] data_length  Size of the received/transmitted packet, not used in this callback implementation.
 */
static void nfc_callback(void          * p_context, 
                         nfc_t2t_event_t event, 
                         const uint8_t * p_data, 
                         size_t          data_length)
{
    UNUSED_PARAMETER(p_context);
    UNUSED_PARAMETER(p_data);
    UNUSED_PARAMETER(data_length);
    
    ret_code_t         err_code = NRF_SUCCESS;
    nfc_pairing_mode_t pairing_mode;
    
    switch (event)
    {
        case NFC_T2T_EVENT_FIELD_ON:
            NRF_LOG_DEBUG("NFC_EVENT_FIELD_ON\r\n");
        
            pairing_mode = nfc_ble_pair_mode_get();
            
            if (pairing_mode == NFC_PAIRING_MODE_OOB)
            {
                // Generate Authentication OOB Key and update NDEF message content.
                uint8_t length = random_vector_generate(m_oob_auth_key.tk, BLE_GAP_SEC_KEY_LEN);
                random_vector_log(length);
                err_code = nfc_tk_group_modifier_update(&m_oob_auth_key);
                APP_ERROR_CHECK(err_code);
            }
            
            // If device is connected, terminate connection and start advertising on BLE_GAP_EVT_DISCONNECTED event.
            if(m_connected)
            {
                m_pending_advertise = true;
                
                err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
                APP_ERROR_CHECK(err_code);
            }
            else
            {
                // Start advertising when NFC field is sensed.
                err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }                
            
            break;

        case NFC_T2T_EVENT_FIELD_OFF:
            NRF_LOG_DEBUG("NFC_EVENT_FIELD_OFF\r\n");
            break;

        default:
            break;
    }
}

/**
 * @brief Function for setting Peer Manager secure mode used in device pairing
 *
 * @param[in] mode                  NFC pairing mode, this is value of the @ref nfc_pairing_mode_t enum
 *
 * @retval NRF_SUCCESS              If new secure mode has been set correctly.
 * @retval NRF_ERROR_INVALID_PARAM  If pairing mode is invalid.
 * @retval Other                    Other error codes might be returned depending on used modules.  
 */
static ret_code_t pm_secure_mode_set(nfc_pairing_mode_t mode)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code = NRF_SUCCESS;
    
    // Check if pairing mode is valid.
    VERIFY_PAIRING_MODE(mode);

    memset(&sec_param, 0x00, sizeof(sec_param));

    // Pairing mode specific security parameters.
    switch (mode)
    {
        case NFC_PAIRING_MODE_JUST_WORKS:
            // Disable pairing with OOB data.
            sec_param.mitm = 0;
            sec_param.oob  = 0;
            sec_param.lesc = 0;
            break;
        
        case NFC_PAIRING_MODE_OOB:
            // Enable legacy pairing with OOB data - TK value.
            sec_param.mitm  = 1;
            sec_param.oob   = 1;
            sec_param.lesc  = 0;
            break;
        
        case NFC_PAIRING_MODE_LESC_OOB:
        case NFC_PAIRING_MODE_LESC_JUST_WORKS:
            // Enable LESC pairing - OOB and MITM flags are cleared because it is central device 
            // who decides if connection will be authorized with LESC OOB data.
            sec_param.mitm  = 0;
            sec_param.oob   = 0;
            sec_param.lesc  = 1;
            break;
        
        default:
            return NRF_ERROR_INVALID_PARAM;
    }
    
    // Common security parameters to be used for all security procedures.
    sec_param.min_key_size   = BLE_NFC_SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = BLE_NFC_SEC_PARAM_MAX_KEY_SIZE;
    sec_param.keypress       = BLE_NFC_SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = BLE_NFC_SEC_PARAM_IO_CAPS;
    sec_param.bond           = BLE_NFC_SEC_PARAM_BOND;
    
#if (BLE_NFC_SEC_PARAM_BOND)
    // If bonding is enabled, set key distribution flags.
    sec_param.kdist_own.enc  = BLE_NFC_SEC_PARAM_KDIST_OWN_ENC;
    sec_param.kdist_own.id   = BLE_NFC_SEC_PARAM_KDIST_OWN_ID;
    sec_param.kdist_peer.enc = BLE_NFC_SEC_PARAM_KDIST_PEER_ENC;
    sec_param.kdist_peer.id  = BLE_NFC_SEC_PARAM_KDIST_PEER_ID;
#else
    // If bonding is not enabled, no keys can be distributed.
    sec_param.kdist_own.enc  = 0;
    sec_param.kdist_own.id   = 0;
    sec_param.kdist_peer.enc = 0;
    sec_param.kdist_peer.id  = 0;
#endif
    
    // Update Peer Manager security parameter settings.
    err_code = pm_sec_params_set(&sec_param);
    
    return err_code;
}


 /**@brief Function for preparing the BLE pairing data for the NFC tag.
 *
 * @details This function does not stop and start the NFC tag data emulation.
 *
 * @param[in] mode Pairing mode for which tag data should be prepared.
 *
 * @retval NRF_SUCCESS              If new tag pairing data has been set correctly.
 * @retval NRF_ERROR_INVALID_PARAM  If pairing mode is invalid.
 * @retval Other                    Other error codes might be returned depending on used modules.  
 */
ret_code_t nfc_ble_pair_data_set(nfc_pairing_mode_t mode)
{
    ret_code_t err_code = NRF_SUCCESS;

    // Check if pairing mode is valid
    VERIFY_PAIRING_MODE(mode);

    // Provide information about available buffer size to encoding function. 
    uint32_t ndef_msg_len = sizeof(m_ndef_msg_buf);
      
    switch (mode)
    {
        case NFC_PAIRING_MODE_OOB:
            // Encode NDEF message with Secure Simple Pairing OOB optional data - TK value. 
            err_code = nfc_ble_pair_msg_updatable_tk_encode(NFC_BLE_PAIR_MSG_BLUETOOTH_LE_SHORT,
                                                            &m_oob_auth_key,
                                                            NULL,
                                                            m_ndef_msg_buf,
                                                            &ndef_msg_len,
                                                            m_tk_group,
                                                            TK_MAX_NUM);
            break;
        
        case NFC_PAIRING_MODE_JUST_WORKS:
            // Encode NDEF message with Secure Simple Pairing OOB data. 
            err_code = nfc_ble_pair_default_msg_encode(NFC_BLE_PAIR_MSG_BLUETOOTH_LE_SHORT,
                                                       NULL,
                                                       NULL,
                                                       m_ndef_msg_buf,
                                                       &ndef_msg_len);
            break;
        
        case NFC_PAIRING_MODE_LESC_OOB:
            // Generate LESC OOB data 
            err_code = sd_ble_gap_lesc_oob_data_get(BLE_CONN_HANDLE_INVALID, 
                                                    &m_lesc_pk, 
                                                    &m_ble_lesc_oob_data);
            VERIFY_SUCCESS(err_code);   
        
            // Encode NDEF message with BLE LESC OOB pairing data -  LESC random and confirmation values. 
            err_code = nfc_ble_pair_default_msg_encode(NFC_BLE_PAIR_MSG_BLUETOOTH_LE_SHORT,
                                                       NULL,
                                                       &m_ble_lesc_oob_data,
                                                       m_ndef_msg_buf,
                                                       &ndef_msg_len);
            break;
        
        case NFC_PAIRING_MODE_LESC_JUST_WORKS:       
            err_code = nfc_ble_pair_default_msg_encode(NFC_BLE_PAIR_MSG_BLUETOOTH_LE_SHORT,
                                                       NULL,
                                                       NULL,
                                                       m_ndef_msg_buf,
                                                       &ndef_msg_len);
            break;
        
        default:
            return NRF_ERROR_INVALID_PARAM;
    }
    
    VERIFY_SUCCESS(err_code);
    
    // Update NFC tag data
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, ndef_msg_len);
    
    return err_code;
}

ret_code_t nfc_ble_pair_init(nfc_pairing_mode_t mode)
{
    ret_code_t err_code = NRF_SUCCESS;

    // Check if pairing mode is valid
    VERIFY_PAIRING_MODE(mode);
    
    m_pairing_mode = mode;
    
    // Initialize RNG peripheral for authentication OOB data generation
    err_code = nrf_drv_rng_init(NULL);
    if (err_code != NRF_ERROR_INVALID_STATE)
    {
        VERIFY_SUCCESS(err_code);
    }
    
    // Initialize encryption module with random number generator use
    ecc_init(true);
    
    // Start NFC
    err_code = nfc_t2t_setup(nfc_callback, NULL);
    VERIFY_SUCCESS(err_code);
        
    // Set Peer Manager pairing mode 
    err_code = pm_secure_mode_set(mode);
    VERIFY_SUCCESS(err_code);

    if ((mode == NFC_PAIRING_MODE_LESC_OOB) || (mode == NFC_PAIRING_MODE_LESC_JUST_WORKS))
    {
        // Generate new LESC keys
        err_code = ecc_p256_keypair_gen(m_lesc_sk.sk, m_lesc_pk.pk);
        VERIFY_SUCCESS(err_code);
    
        // Update Peer Manager with new LESC Public Key
        err_code = pm_lesc_public_key_set(&m_lesc_pk);
        VERIFY_SUCCESS(err_code);
    }
    
    // Set proper NFC data according to the pairing mode 
    err_code = nfc_ble_pair_data_set(mode);
    VERIFY_SUCCESS(err_code);
    
    // Turn on tag emulation
    err_code = nfc_t2t_emulation_start();

    return err_code;
}

ret_code_t nfc_ble_pair_mode_set(nfc_pairing_mode_t mode)
{
    ret_code_t err_code = NRF_SUCCESS;

    // Check if pairing mode is valid
    VERIFY_PAIRING_MODE(mode);
    
    if (mode != m_pairing_mode)
    {
        m_pairing_mode = mode;
        
        if ((mode == NFC_PAIRING_MODE_LESC_OOB) || (mode == NFC_PAIRING_MODE_LESC_JUST_WORKS))
        {
            // Generate new LESC keys
            err_code = ecc_p256_keypair_gen(m_lesc_sk.sk, m_lesc_pk.pk);
            VERIFY_SUCCESS(err_code);
    
            // Update Peer Manager with new LESC Public Key
            err_code = pm_lesc_public_key_set(&m_lesc_pk);
            VERIFY_SUCCESS(err_code);
        }
        
        // Update Peer Manager settings according to the new pairing mode
        err_code = pm_secure_mode_set(mode);
        VERIFY_SUCCESS(err_code);

        // NFC tag emulation must be turned off during changes in payload
        err_code = nfc_t2t_emulation_stop();
        VERIFY_SUCCESS(err_code);

        // Update NFC tag data
        err_code = nfc_ble_pair_data_set(mode);
        VERIFY_SUCCESS(err_code);

        // Turn on tag emulation after changes
        err_code = nfc_t2t_emulation_start();
        VERIFY_SUCCESS(err_code);
    }
    
    return NRF_SUCCESS;
}

nfc_pairing_mode_t nfc_ble_pair_mode_get(void)
{
    return m_pairing_mode;
}

/**
 * @brief   Generates new key pair for LESC pairing.
 *
 * @details If device is in @ref NFC_PAIRING_MODE_LESC_OOB mode,
 *          NFC Connection Handover message is also updated with
 *          newly generated LESC OOB data.
 *
 * @retval  NRF_SUCCESS If new tag pairing data has been set correctly.
 * @retval  Other       Other error codes might be returned depending on used modules.   
 */
static ret_code_t generate_lesc_keys(void)
{
    ret_code_t err_code = NRF_SUCCESS;
    
    // Generate new LESC keys
    err_code = ecc_p256_keypair_gen(m_lesc_sk.sk, m_lesc_pk.pk);
    VERIFY_SUCCESS(err_code);

    // Update Peer Manager with new LESC Public Key
    err_code = pm_lesc_public_key_set(&m_lesc_pk);
    VERIFY_SUCCESS(err_code);
                
    if (m_pairing_mode == NFC_PAIRING_MODE_LESC_OOB)
    {
        // Generate LESC OOB data.
        err_code = sd_ble_gap_lesc_oob_data_get(BLE_CONN_HANDLE_INVALID, 
                                                &m_lesc_pk, 
                                                &m_ble_lesc_oob_data);
        VERIFY_SUCCESS(err_code);

        // Update NDEF message with new LESC OOB data.
        err_code = nfc_lesc_data_update(&m_ble_lesc_oob_data);
        VERIFY_SUCCESS(err_code);
    }    
    
    return NRF_SUCCESS;
}

void on_nfc_ble_pair_evt(const ble_evt_t * const p_ble_evt)
{
    ret_code_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        // Upon authorization key request, reply with Temporary Key readen from the NFC tag
        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            NRF_LOG_DEBUG("BLE_GAP_EVT_AUTH_KEY_REQUEST\r\n");
        
            err_code = sd_ble_gap_auth_key_reply(p_ble_evt->evt.gap_evt.conn_handle, 
                                                 BLE_GAP_AUTH_KEY_TYPE_OOB, 
                                                 m_oob_auth_key.tk);
            APP_ERROR_CHECK(err_code);
            break;

        // Upon LESC Diffie_Hellman key request, reply with key computed from device secret key and peer public key      
        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            NRF_LOG_INFO("BLE_GAP_EVT_LESC_DHKEY_REQUEST\r\n");
        
            // If LESC OOB pairing is on, perform authentication with OOB data
            if (p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.oobd_req)
            {
                err_code = sd_ble_gap_lesc_oob_data_set(p_ble_evt->evt.gap_evt.conn_handle,
                                                        &m_ble_lesc_oob_data,
                                                        NULL);
                APP_ERROR_CHECK(err_code);
            }

            // Compute D-H key
            err_code = ecc_p256_shared_secret_compute(
                            &m_lesc_sk.sk[0], 
                            &p_ble_evt->evt.gap_evt.params.lesc_dhkey_request.p_pk_peer->pk[0],
                            &m_lesc_dhkey.key[0]);
            APP_ERROR_CHECK(err_code);
            
            // Reply with obtained result
            err_code = sd_ble_gap_lesc_dhkey_reply(p_ble_evt->evt.gap_evt.conn_handle, 
                                                   &m_lesc_dhkey);
            APP_ERROR_CHECK(err_code);            
            break;

        case BLE_GAP_EVT_CONNECTED:
            // Stop advertising if device is connected
            err_code = ble_advertising_start(BLE_ADV_MODE_IDLE);
            APP_ERROR_CHECK(err_code);
       
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            m_connected = true;
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            // Generate new LESC key pair and OOB data 
            if ((m_pairing_mode == NFC_PAIRING_MODE_LESC_OOB) || 
                (m_pairing_mode == NFC_PAIRING_MODE_LESC_JUST_WORKS))
            {
                err_code = generate_lesc_keys();
                APP_ERROR_CHECK(err_code);
            }
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            // Start directed advertising if connection timeout occured
            if (p_ble_evt->evt.gap_evt.params.disconnected.reason == BLE_HCI_CONNECTION_TIMEOUT)
            {
                err_code = ble_advertising_start(BLE_ADV_MODE_DIRECTED);
                APP_ERROR_CHECK(err_code);
            }
            
            m_connected   = false;
            m_conn_handle = BLE_CONN_HANDLE_INVALID;
            
            if(m_pending_advertise)
            {
                m_pending_advertise = false;

                // Start advertising when NFC field is sensed.
                err_code = ble_advertising_start(BLE_ADV_MODE_FAST);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            else
            {
                // Generate new LESC key pair and OOB data only when connection is not terminated because of new tag reading
                if ((m_pairing_mode == NFC_PAIRING_MODE_LESC_OOB) || 
                    (m_pairing_mode == NFC_PAIRING_MODE_LESC_JUST_WORKS))
                {
                    err_code = generate_lesc_keys();
                    APP_ERROR_CHECK(err_code);
                }
            }
            break;

        default:
            break;
    }
}

#endif // NFC_BLE_PAIR_LIB_ENABLED
