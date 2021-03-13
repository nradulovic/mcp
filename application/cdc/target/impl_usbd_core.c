/**
 * @brief          : This file implements the board support package for the USB device library
 */

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "usbd_core.h"
#include "usbd_def.h"
#include "error_handler.h"

static USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status);

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/**
 * @brief  Initializes the low level portion of the device driver.
 * @param  pdev: Device handle
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_Init(USBD_HandleTypeDef *pdev)
{
    /* Init USB Ip. */
    if (pdev->id == USBD_CONF__DEV_MODE__FULL_SPEED) {
        /* Link the driver to the stack. */
        hpcd_USB_OTG_FS.pData = pdev;
        pdev->pData = &hpcd_USB_OTG_FS;

        hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
        hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
        hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
        hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
        hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.vbus_sensing_enable = DISABLE;
        hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
        if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK) {
            error_handler__handle();
        }

#if (USE_HAL_PCD_REGISTER_CALLBACKS == 1U)
  /* Register USB PCD CallBacks */
  HAL_PCD_RegisterCallback(&hpcd_USB_OTG_FS, HAL_PCD_SOF_CB_ID, PCD_SOFCallback);
  HAL_PCD_RegisterCallback(&hpcd_USB_OTG_FS, HAL_PCD_SETUPSTAGE_CB_ID, PCD_SetupStageCallback);
  HAL_PCD_RegisterCallback(&hpcd_USB_OTG_FS, HAL_PCD_RESET_CB_ID, PCD_ResetCallback);
  HAL_PCD_RegisterCallback(&hpcd_USB_OTG_FS, HAL_PCD_SUSPEND_CB_ID, PCD_SuspendCallback);
  HAL_PCD_RegisterCallback(&hpcd_USB_OTG_FS, HAL_PCD_RESUME_CB_ID, PCD_ResumeCallback);
  HAL_PCD_RegisterCallback(&hpcd_USB_OTG_FS, HAL_PCD_CONNECT_CB_ID, PCD_ConnectCallback);
  HAL_PCD_RegisterCallback(&hpcd_USB_OTG_FS, HAL_PCD_DISCONNECT_CB_ID, PCD_DisconnectCallback);

  HAL_PCD_RegisterDataOutStageCallback(&hpcd_USB_OTG_FS, PCD_DataOutStageCallback);
  HAL_PCD_RegisterDataInStageCallback(&hpcd_USB_OTG_FS, PCD_DataInStageCallback);
  HAL_PCD_RegisterIsoOutIncpltCallback(&hpcd_USB_OTG_FS, PCD_ISOOUTIncompleteCallback);
  HAL_PCD_RegisterIsoInIncpltCallback(&hpcd_USB_OTG_FS, PCD_ISOINIncompleteCallback);
#endif /* USE_HAL_PCD_REGISTER_CALLBACKS */
        HAL_PCDEx_SetRxFiFo(&hpcd_USB_OTG_FS, 0x80);
        HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_FS, 0, 0x40);
        HAL_PCDEx_SetTxFiFo(&hpcd_USB_OTG_FS, 1, 0x80);
    }
    return USBD_OK;
}

/**
 * @brief  De-Initializes the low level portion of the device driver.
 * @param  pdev: Device handle
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_DeInit(pdev->pData);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Starts the low level portion of the device driver.
 * @param  pdev: Device handle
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_Start(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Start(pdev->pData);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Stops the low level portion of the device driver.
 * @param  pdev: Device handle
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_Stop(pdev->pData);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Opens an endpoint of the low level driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint number
 * @param  ep_type: Endpoint type
 * @param  ep_mps: Endpoint max packet size
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t ep_type,
                uint16_t ep_mps)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Open(pdev->pData, ep_addr, ep_mps, ep_type);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Closes an endpoint of the low level driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint number
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Close(pdev->pData, ep_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Flushes an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint number
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Flush(pdev->pData, ep_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Sets a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint number
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_SetStall(pdev->pData, ep_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Clears a Stall condition on an endpoint of the Low Level Driver.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint number
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_ClrStall(pdev->pData, ep_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Returns Stall condition.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint number
 * @retval Stall (1: Yes, 0: No)
 */
uint8_t USBD_LL_IsStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    PCD_HandleTypeDef *hpcd = (PCD_HandleTypeDef*) pdev->pData;

    if ((ep_addr & 0x80) == 0x80) {
        return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
    } else {
        return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
    }
}

/**
 * @brief  Assigns a USB address to the device.
 * @param  pdev: Device handle
 * @param  dev_addr: Device address
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_SetAddress(pdev->pData, dev_addr);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Transmits data over an endpoint.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint number
 * @param  pbuf: Pointer to data to be sent
 * @param  size: Data size
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf,
                uint32_t size)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Transmit(pdev->pData, ep_addr, pbuf, size);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Prepares an endpoint for reception.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint number
 * @param  pbuf: Pointer to data to be received
 * @param  size: Data size
 * @retval USBD status
 */
USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf,
                uint32_t size)
{
    HAL_StatusTypeDef hal_status = HAL_OK;
    USBD_StatusTypeDef usb_status = USBD_OK;

    hal_status = HAL_PCD_EP_Receive(pdev->pData, ep_addr, pbuf, size);

    usb_status = USBD_Get_USB_Status(hal_status);

    return usb_status;
}

/**
 * @brief  Returns the last transfered packet size.
 * @param  pdev: Device handle
 * @param  ep_addr: Endpoint number
 * @retval Recived Data Size
 */
uint32_t USBD_LL_GetRxDataSize(USBD_HandleTypeDef *pdev, uint8_t ep_addr)
{
    return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*) pdev->pData, ep_addr);
}

/**
 * @brief  Delays routine for the USB Device Library.
 * @param  Delay: Delay in ms
 * @retval None
 */
void USBD_LL_Delay(uint32_t Delay)
{
    HAL_Delay(Delay);
}

/**
 * @brief  Retuns the USB status depending on the HAL status:
 * @param  hal_status: HAL status
 * @retval USB status
 */
static USBD_StatusTypeDef USBD_Get_USB_Status(HAL_StatusTypeDef hal_status)
{
    USBD_StatusTypeDef usb_status = USBD_OK;

    switch (hal_status) {
    case HAL_OK:
        usb_status = USBD_OK;
        break;
    case HAL_ERROR:
        usb_status = USBD_FAIL;
        break;
    case HAL_BUSY:
        usb_status = USBD_BUSY;
        break;
    case HAL_TIMEOUT:
        usb_status = USBD_FAIL;
        break;
    default:
        usb_status = USBD_FAIL;
        break;
    }
    return usb_status;
}
