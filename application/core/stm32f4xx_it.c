
#include "main.h"
#include "stm32f4xx_it.h"
#include "error_handler.h"

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

void TIM5_IRQHandler(void)
{
    mdrv__time_base__it();
}

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}
