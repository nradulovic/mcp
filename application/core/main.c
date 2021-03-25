#include "main.h"
#include "config_peripherals.h"
#include "usbd_cdc_terminal.h"
#include "mdrv.h"
#include "error_handler.h"
#include "mdrv_time_base.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_ll_gpio.h"

static void system_clock_init(void);
static void gpio_init(void);
static void pin_init_output(void *context, bool value);
static void pin_init_input(void *context);
static void pin_init_trigger(void *context);
static void pin_write(void *context, bool value);
static bool pin_read(void *context);

static const struct mdrv__ll mdrv__ll = {
    .pin_init_input = pin_init_input,
    .pin_init_output = pin_init_output,
    .pin_init_trigger = pin_init_trigger,
    .pin_write = pin_write,
    .pin_read = pin_read,
    .tim_start = mdrv__time_base__start,
    .tim_stop = mdrv__time_base__stop,
    .tim_start_on_trigger = mdrv__time_base__start_on_trigger};

static struct mdrv__context mdrv__context;

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    system_clock_init();
    DBGMCU->APB2FZ |= (DBGMCU_APB2_FZ_DBG_TIM9_STOP);

    /* Initialize all configured peripherals */
    gpio_init();
    mdrv__time_base__init(&mdrv__context);
    mdrv__init(&mdrv__context, &mdrv__ll, NULL);
    usbd_cdc_terminal__init();
    usbd_cdc_terminal__set_terminal_context(&mdrv__context);

    /* Infinite loop */
    while (1) {
        usbd_cdc_terminal__loop();
    }
}

void system_clock_init(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {
        0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {
        0};

    /** Configure the main internal regulator output voltage
     */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /**
     * Initializes the RCC Oscillators according to the specified parameters
     * in the RCC_OscInitTypeDef structure.
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 4;
    RCC_OscInitStruct.PLL.PLLN = 192;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 8;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        error_handler__handle();
    }
    /** Initializes the CPU, AHB and APB buses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1
        | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
        error_handler__handle();
    }
    SystemCoreClockUpdate();
}

static void gpio_init(void)
{
    GPIO_InitTypeDef gpio_init_struct = {
        0};

    /* Initialize DATA pin */
    MCP_DATA_CLK_ENABLE();
    gpio_init_struct.Pin = MCP_DATA_PIN;
    gpio_init_struct.Mode = GPIO_MODE_INPUT;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    gpio_init_struct.Alternate = 0;
    HAL_GPIO_Init(MCP_DATA_GPIO_PORT, &gpio_init_struct);

    /* Initialize STATUS pin */
    MCP_STATUS_CLK_ENABLE();
    gpio_init_struct.Pin = MCP_STATUS_PIN;
    gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
    gpio_init_struct.Alternate = 0;
    HAL_GPIO_Init(MCP_STATUS_GPIO_PORT, &gpio_init_struct);
}

static void pin_init_output(void *context, bool value)
{
    pin_write(context, value);
    LL_GPIO_SetPinMode(MCP_DATA_GPIO_PORT, MCP_DATA_PIN, LL_GPIO_MODE_OUTPUT);
}

static void pin_init_input(void *context)
{
    (void) context;
    LL_GPIO_SetPinMode(MCP_DATA_GPIO_PORT, MCP_DATA_PIN, LL_GPIO_MODE_INPUT);
}

static void pin_init_trigger(void *context)
{
    (void) context;

    GPIO_InitTypeDef input_pin = {
          .Pin = GPIO_PIN_2,
          .Mode = GPIO_MODE_AF_PP,
          .Alternate = GPIO_AF3_TIM9,
          .Pull = GPIO_NOPULL,
          .Speed = GPIO_SPEED_FAST
      };

      HAL_GPIO_Init(GPIOA, &input_pin);
}

static void pin_write(void *context, bool state)
{
    (void) context;
    if (state) {
        LL_GPIO_SetOutputPin(MCP_DATA_GPIO_PORT, MCP_DATA_PIN);
    } else {
        LL_GPIO_ResetOutputPin(MCP_DATA_GPIO_PORT, MCP_DATA_PIN);
    }
}

static bool pin_read(void *context)
{
    (void) context;
    return LL_GPIO_ReadInputPort(MCP_DATA_GPIO_PORT) & MCP_DATA_PIN;
}
