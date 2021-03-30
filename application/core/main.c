
#include <stdbool.h>

#include "mdrv.h"
#include "mdrv_application.h"
#include "error_handler.h"
#include "stm32f4xx_hal.h"
#include "usbd_cdc_terminal.h"
#include "config_peripherals.h"
#include "error_handler.h"
#include "mcp_config.h"

static void system_clock_init(void);
static void gpio_init(void);

static const struct mdrv__ll mdrv__ll = {
    .pin_init_input = mdrv__application__pin__init_input,
    .pin_init_output = mdrv__application__pin__init_output,
    .pin_init_trigger = mdrv__application__pin__init_trigger,
    .pin_write = mdrv__application__pin__write,
    .pin_read = mdrv__application__pin__read,
    .tim_start = mdrv__application__tim__start,
    .tim_stop = mdrv__application__tim__stop,
    .tim_start_on_trigger = mdrv__application__tim__start_on_trigger};

static struct mdrv__context g__mdrv__context;
static struct mcp_config__context g__mcp_config__context;

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    error_handler__init();
    system_clock_init();
    gpio_init();
    /* Stop timer 9 (which is used by mdrv__application) while in debug mode */
    DBGMCU->APB2FZ |= (DBGMCU_APB2_FZ_DBG_TIM9_STOP);

    mdrv__application__init(&g__mdrv__context);
    mdrv__init(&g__mdrv__context, &mdrv__ll, NULL);
    usbd_cdc_terminal__init();
    usbd_cdc_terminal__set_terminal_context(&g__mdrv__context);

    /* Infinite loop */
    while (true) {
        usbd_cdc_terminal__loop();
    }
}

static void gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {
        0};

    /* Initialize STATUS pin */
    MCP_STATUS_CLK_ENABLE();
    gpio_init_struct.Pin = MCP_STATUS_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_struct.Alternate = 0;
    HAL_GPIO_Init(MCP_STATUS_GPIO_PORT, &gpio_init_struct);
}

void system_clock_init(void)
{
    RCC_OscInitTypeDef rcc_osc_init = {
        0};
    RCC_ClkInitTypeDef rcc_clk_init = {
        0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /**
     * Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    rcc_osc_init.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    rcc_osc_init.HSEState = RCC_HSE_ON;
    rcc_osc_init.PLL.PLLState = RCC_PLL_ON;
    rcc_osc_init.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    rcc_osc_init.PLL.PLLM = 4;
    rcc_osc_init.PLL.PLLN = 192;
    rcc_osc_init.PLL.PLLP = RCC_PLLP_DIV4;
    rcc_osc_init.PLL.PLLQ = 8;
    if (HAL_RCC_OscConfig(&rcc_osc_init) != HAL_OK) {
        error_handler__handle();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    rcc_clk_init.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1
        | RCC_CLOCKTYPE_PCLK2;
    rcc_clk_init.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    rcc_clk_init.AHBCLKDivider = RCC_SYSCLK_DIV1;
    rcc_clk_init.APB1CLKDivider = RCC_HCLK_DIV2;
    rcc_clk_init.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&rcc_clk_init, FLASH_LATENCY_3) != HAL_OK) {
        error_handler__handle();
    }
    SystemCoreClockUpdate();
}
