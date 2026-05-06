#include "main.h"
#include "ssd1306.h"
#include "fonts.h"
#include <stdio.h>
#include <string.h>

/* HANDLES */
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
UART_HandleTypeDef huart2;
I2C_HandleTypeDef hi2c1;

/* VARIABLES */
uint8_t temperature = 0;
uint8_t humidity = 0;

/* DHT11 */
#define DHT11_PORT GPIOA
#define DHT11_PIN  GPIO_PIN_1

/* FUNCTION PROTOTYPES */
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);

/* MICROSECOND DELAY */
void delay_us(uint16_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);
    while(__HAL_TIM_GET_COUNTER(&htim2) < us);
}

/* DHT11 START */
void DHT11_Start(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = DHT11_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET);
    HAL_Delay(18);

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET);
    delay_us(20);

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT11_PORT, &GPIO_InitStruct);
}

/* DHT11 READ */
uint8_t DHT11_Read(void)
{
    uint8_t i = 0, j;

    for(j = 0; j < 8; j++)
    {
        while(!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));
        delay_us(40);

        if(!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))
            i &= ~(1 << (7 - j));
        else
        {
            i |= (1 << (7 - j));
            while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));
        }
    }
    return i;
}

/* MAIN */
int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_USART2_UART_Init();
    MX_I2C1_Init();

    HAL_TIM_Base_Start(&htim2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);

    /* OLED INIT */
    ssd1306_Init(&hi2c1);

    /* ===== WELCOME ===== */
    ssd1306_Fill(Black);
    ssd1306_SetCursor(20,10);
    ssd1306_WriteString("WELCOME", Font_11x18, White);
    ssd1306_SetCursor(40,35);
    ssd1306_WriteString("^_^", Font_11x18, White);
    ssd1306_UpdateScreen(&hi2c1);
    HAL_Delay(1500);

    /* ===== TITLE ===== */
    ssd1306_Fill(Black);
    ssd1306_SetCursor(10,10);
    ssd1306_WriteString("STM32", Font_11x18, White);
    ssd1306_SetCursor(0,35);
    ssd1306_WriteString("SMART FAN", Font_11x18, White);
    ssd1306_UpdateScreen(&hi2c1);
    HAL_Delay(1500);

    /* ===== STARTING ===== */
    for(int i=0;i<3;i++)
    {
        ssd1306_Fill(Black);
        ssd1306_SetCursor(10,20);
        ssd1306_WriteString("Starting", Font_11x18, White);
        if(i==1) ssd1306_WriteString(".", Font_11x18, White);
        if(i==2) ssd1306_WriteString("..", Font_11x18, White);
        ssd1306_UpdateScreen(&hi2c1);
        HAL_Delay(500);
    }

    while(1)
    {
        uint8_t Rh1,Rh2,T1,T2,sum;
        uint16_t duty=0;
        char line[20];

        /* DHT11 */
        DHT11_Start();
        if(!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN))
        {
            while(!HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));
            while(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN));

            Rh1=DHT11_Read();
            Rh2=DHT11_Read();
            T1=DHT11_Read();
            T2=DHT11_Read();
            sum=DHT11_Read();

            if((Rh1+Rh2+T1+T2)==sum)
            {
                temperature=T1;
                humidity=Rh1;
            }
        }

        /* PWM */
        if(temperature >= 27) duty = 999;
        else if(temperature > 25) duty = 700;
        else if(temperature >= 23) duty = 600;
        else if(temperature >= 20) duty = 400;
        else duty = 0;

        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, duty);

        /* MOTOR */
        if(duty==0)
        {
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);
        }

        /* OLED DISPLAY */
        ssd1306_Fill(Black);

        ssd1306_SetCursor(0,0);
        ssd1306_WriteString("Temp:", Font_7x10, White);
        sprintf(line,"%dC",temperature);
        ssd1306_SetCursor(70,0);
        ssd1306_WriteString(line, Font_7x10, White);

        ssd1306_SetCursor(0,20);
        ssd1306_WriteString("Hum :", Font_7x10, White);
        sprintf(line,"%d%%",humidity);
        ssd1306_SetCursor(70,20);
        ssd1306_WriteString(line, Font_7x10, White);

        ssd1306_SetCursor(0,40);
        ssd1306_WriteString("Fan :", Font_7x10, White);

        if(duty==0) sprintf(line,"OFF");
        else if(duty<=600) sprintf(line,"LOW");
        else if(duty<=800) sprintf(line,"MED");
        else sprintf(line,"HIGH");

        ssd1306_SetCursor(70,40);
        ssd1306_WriteString(line, Font_7x10, White);

        ssd1306_UpdateScreen(&hi2c1);

        HAL_Delay(1000);
    }
}

/* ================= INIT FUNCTIONS ================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_Osc={0};
    RCC_ClkInitTypeDef RCC_Clk={0};

    RCC_Osc.OscillatorType=RCC_OSCILLATORTYPE_HSI;
    RCC_Osc.HSIState=RCC_HSI_ON;
    HAL_RCC_OscConfig(&RCC_Osc);

    RCC_Clk.ClockType=RCC_CLOCKTYPE_SYSCLK;
    RCC_Clk.SYSCLKSource=RCC_SYSCLKSOURCE_HSI;
    HAL_RCC_ClockConfig(&RCC_Clk,FLASH_LATENCY_0);
}

static void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct={0};

    GPIO_InitStruct.Pin=GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_8;
    GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

    GPIO_InitStruct.Pin=GPIO_PIN_6;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_PP;
    GPIO_InitStruct.Alternate=GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStruct);

    GPIO_InitStruct.Pin=GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode=GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull=GPIO_PULLUP;
    GPIO_InitStruct.Alternate=GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB,&GPIO_InitStruct);
}

static void MX_TIM2_Init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();
    htim2.Instance=TIM2;
    htim2.Init.Prescaler=16-1;
    htim2.Init.Period=0xFFFFFFFF;
    HAL_TIM_Base_Init(&htim2);
}

static void MX_TIM3_Init(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();

    TIM_OC_InitTypeDef sConfig={0};

    htim3.Instance=TIM3;
    htim3.Init.Prescaler=16-1;
    htim3.Init.Period=1000-1;
    HAL_TIM_PWM_Init(&htim3);

    sConfig.OCMode=TIM_OCMODE_PWM1;
    sConfig.Pulse=0;
    HAL_TIM_PWM_ConfigChannel(&htim3,&sConfig,TIM_CHANNEL_1);
}

static void MX_USART2_UART_Init(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();

    huart2.Instance=USART2;
    huart2.Init.BaudRate=115200;
    HAL_UART_Init(&huart2);
}

static void MX_I2C1_Init(void)
{
    __HAL_RCC_I2C1_CLK_ENABLE();

    hi2c1.Instance=I2C1;
    hi2c1.Init.ClockSpeed=100000;
    hi2c1.Init.AddressingMode=I2C_ADDRESSINGMODE_7BIT;
    HAL_I2C_Init(&hi2c1);
}
