
#include "config_peripherals.h"
#include "stm32f4xx_hal.h"
#include "error_handler.h"
#include "mdrv_application.h"
#include "poll_timer.h"

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
    poll_timer__isr();
}

void MDRV_TIME_BASE_CONFIG__TIM_IRQ_HANDLER(void)
{
    mdrv__application__tim__isr();
}

void OTG_FS_IRQHandler(void)
{
    HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS);
}
