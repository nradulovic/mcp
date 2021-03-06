/**
 * @brief          : Usb device for Virtual Com Port.
 */

#include <stdint.h>

#include "app_usbd_cdc.h"
#include "app_usbd_cdc__desc.h"
#include "config_usbd_cdc.h"

#include "main.h"

#include "usbd_cdc.h"
#include "usbd_def.h"

static int8_t init(void);
static int8_t teardown(void);
static int8_t control(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t receive(uint8_t *pbuf, uint32_t *length);
static int8_t transmit_complete(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USB Device Core handle declaration. */
static USBD_HandleTypeDef handle;

/* Create buffer for reception and transmission           */
/** Received data over USB are stored in this buffer      */
static uint8_t rx_buffer[CONFIG_USBD_CDC__RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
static uint8_t tx_buffer[CONFIG_USBD_CDC__TX_DATA_SIZE];
static USBD_CDC_ItfTypeDef app_usbd_cdc_if__fops =
    {
      init,
      teardown,
      control,
      receive,
      transmit_complete };
static const struct app_usbd_cdc__context * g_context;

/**
 * @brief  Initializes the CDC media low layer over the FS USB IP
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t init(void)
{
    /* Set Application Buffers */
    USBD_CDC_SetTxBuffer(&handle, tx_buffer, 0);
    USBD_CDC_SetRxBuffer(&handle, rx_buffer);
    return (USBD_OK);
}

/**
 * @brief  DeInitializes the CDC media low layer
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t teardown(void)
{
    return (USBD_OK);
}

/**
 * @brief  Manage the CDC class requests
 * @param  cmd: Command code
 * @param  pbuf: Buffer containing command data (request parameters)
 * @param  length: Number of data to be sent (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t control(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
    UNUSED(pbuf);
    UNUSED(length);

    switch (cmd) {
    case CDC_SEND_ENCAPSULATED_COMMAND:
        break;
    case CDC_GET_ENCAPSULATED_RESPONSE:
        break;
    case CDC_SET_COMM_FEATURE:
        break;
    case CDC_GET_COMM_FEATURE:
        break;
    case CDC_CLEAR_COMM_FEATURE:
        break;
        /*******************************************************************************/
        /* Line Coding Structure                                                       */
        /*-----------------------------------------------------------------------------*/
        /* Offset | Field       | Size | Value  | Description                          */
        /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
        /* 4      | bCharFormat |   1  | Number | Stop bits                            */
        /*                                        0 - 1 Stop bit                       */
        /*                                        1 - 1.5 Stop bits                    */
        /*                                        2 - 2 Stop bits                      */
        /* 5      | bParityType |  1   | Number | Parity                               */
        /*                                        0 - None                             */
        /*                                        1 - Odd                              */
        /*                                        2 - Even                             */
        /*                                        3 - Mark                             */
        /*                                        4 - Space                            */
        /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
        /*******************************************************************************/
    case CDC_SET_LINE_CODING:
        break;
    case CDC_GET_LINE_CODING:
        break;
    case CDC_SET_CONTROL_LINE_STATE:
        break;
    case CDC_SEND_BREAK:
        break;
    default:
        break;
    }

    return (USBD_OK);
}

/**
 * @brief  Data received over USB OUT endpoint are sent over CDC interface
 *         through this function.
 *
 *         @note
 *         This function will issue a NAK packet on any OUT packet received on
 *         USB endpoint until exiting this function. If you exit this function
 *         before transfer is complete on CDC interface (ie. using DMA controller)
 *         it will result in receiving more data while previous ones are still
 *         not sent.
 *
 * @param  Buf: Buffer of data to be received
 * @param  Len: Number of data received (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t receive(uint8_t *buffer, uint32_t *length)
{
    g_context->receive(g_context->arg, buffer, (uint16_t)*length);
    USBD_CDC_SetRxBuffer(&handle, &buffer[0]);
    USBD_CDC_ReceivePacket(&handle);
    return (USBD_OK);
}

/**
 * @brief  transmit_complete
 *         Data transmited callback
 *
 *         @note
 *         This function is IN transfer complete callback used to inform user that
 *         the submitted Data is successfully sent over USB.
 *
 * @param  Buf: Buffer of data to be received
 * @param  Len: Number of data received (in bytes)
 * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t transmit_complete(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
    UNUSED(Buf);
    UNUSED(Len);
    int8_t retval;

    switch (epnum) {
    case CDC_IN_EP:
        if (g_context->complete) {
            g_context->complete(g_context->arg);
        }
        retval = USBD_OK;
        break;
    default:
        retval = USBD_FAIL;
        break;
    }
    return retval;
}

/**
 * Init USB device Library, add supported class and start the library
 */
void app_usbd_cdc__init(const struct app_usbd_cdc__context * context)
{
    g_context = context;
    /* Init Device Library, add supported class and start the library. */
    if (USBD_Init(&handle, &usbd_cdc__desc__fs_desc, USBD_CONF__DEV_MODE__FULL_SPEED) != USBD_OK) {
        Error_Handler();
    }
    if (USBD_RegisterClass(&handle, &USBD_CDC) != USBD_OK) {
        Error_Handler();
    }
    if (USBD_CDC_RegisterInterface(&handle, &app_usbd_cdc_if__fops) != USBD_OK) {
        Error_Handler();
    }
    if (USBD_Start(&handle) != USBD_OK) {
        Error_Handler();
    }
}

/**
 * @brief  app_usbd_cdc_if__transmit
 *         Data to send over USB IN endpoint are sent over CDC interface
 *         through this function.
 *         @note
 *
 *
 * @param  Buf: Buffer of data to be sent
 * @param  Len: Number of data to be sent (in bytes)
 * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
 */
enum app_usbd_cdc__error app_usbd_cdc__transmit(const void * buffer, uint16_t length)
{
    USBD_StatusTypeDef result;
    enum app_usbd_cdc__error error;

    USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*) handle.pClassData;
    if (hcdc->TxState != 0) {
        return APP_USBD_CDC__ERROR__BUSY;
    }
    /* Cast away const qualifier. We don't want to modify the whole USB library for this */
    USBD_CDC_SetTxBuffer(&handle, (uint8_t *)buffer, length);
    result = USBD_CDC_TransmitPacket(&handle);

    switch (result) {
    case USBD_OK:
        error = APP_USBD_CDC__ERROR__OK;
        break;
    case USBD_BUSY:
        error = APP_USBD_CDC__ERROR__BUSY;
        break;
    default:
        error = APP_USBD_CDC__ERROR__FAIL;
        break;
    }
    return error;
}

