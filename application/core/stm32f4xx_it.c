
#include "main.h"
#include "stm32f4xx_it.h"
#include "error_handler.h"
#include "config_mdrv_time_base.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

void NMI_Handler(void)
{
    error_handler__handle();
}

void HardFault_Handler(void)
{
    error_handler__handle();
}

void MemManage_Handler(void)
{
    error_handler__handle();
}

void BusFault_Handler(void)
{
    error_handler__handle();
}

void UsageFault_Handler(void)
{
    error_handler__handle();
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
    HAL_IncTick();
}

void MDRV_TIME_BASE_CONFIG__TIM_IRQ_HANDLER(void)
{
    mdrv__time_base__it();
}

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}
