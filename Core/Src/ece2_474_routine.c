/*
 * STM32_2 과제 5/6/7: UART2 = PC, UART3 = FPGA, 9600 8N1.
 * getchar와 printf는 메인 루프에서만 사용합니다.
 * 같은 UART2에서 HAL_UART_Receive_IT를 함께 시작하지 않습니다.
 */
#include "ece2_474.h"
#include "main.h"
#include "stm32_2_task.h"

/* Watch용 변수. HAL_OK는 MCU 송신 완료이며 FPGA 수신 확인은 아닙니다. */
volatile uint32_t pc_rx_count = 0U;
volatile uint8_t pc_last_byte = 0U;
volatile uint32_t fpga_tx_count = 0U;
volatile uint32_t fpga_tx_error_count = 0U;
volatile HAL_StatusTypeDef fpga_last_tx_status = HAL_OK;

void ece2_474_init(void)
{
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("\r\nSTM32_2 Task %d | UART2/UART3: 9600 8N1\r\n", STM32_2_TASK);
#if STM32_2_TASK == 5
    printf("Send one character (None line ending): 1, A, a\r\n");
#elif STM32_2_TASK == 6
    printf("UART2 -> UART3 bridge. Check received data on FPGA COM.\r\n");
#else
    printf("FPGA commands: 0=OFF, 1=ON, 2=TOGGLE; others=HOLD.\r\n");
#endif
}

void polling_routine(void)
{
    /* 입력이 없으면 getchar에서 기다리는 것이 정상입니다. */
    int ch = getchar();
    if (ch == EOF)
    {
        clearerr(stdin);
        return;
    }
    uint8_t data = (uint8_t)ch;
    pc_last_byte = data;
    ++pc_rx_count;

#if STM32_2_TASK != 5
    /* 과제 6과 7의 MCU 전달 동작은 같습니다.
     * 문자 '1'(0x31)을 숫자 1(0x01)로 변환하지 않고 그대로 보냅니다.
     * 기타 문자도 전달하며, LED 상태 유지 여부는 FPGA에서 처리합니다. */
    HAL_StatusTypeDef result = HAL_UART_Transmit(&huart3, &data, 1U, 100U);
    fpga_last_tx_status = result;
    if (result == HAL_OK)
    {
        ++fpga_tx_count;
    }
    else
    {
        ++fpga_tx_error_count;
    }
#endif

    /* 제어 문자가 화면을 흐트러뜨리지 않도록 바이트 값으로 표시합니다. */
    if (data >= 0x20U && data <= 0x7EU)
    {
        printf("RX: '%c' (0x%02X)", (int)data, (unsigned int)data);
    }
    else
    {
        printf("RX: [control/non-ASCII] (0x%02X)", (unsigned int)data);
    }
#if STM32_2_TASK != 5
    printf(" | UART3 TX result=%d", (int)result);
#endif
    printf("\r\n");
}

/* 1. 사용자 버튼 PC13을 눌렀을 때 호출됩니다. 우선순위: 7.
 * 버튼의 접점 때문에 한 번 눌러도 여러 번 호출될 수 있습니다.
 */
static void button_isr_p7(void)
{
    /* 실습 코드 작성: 버튼 이벤트를 기록하거나 LED 상태를 변경합니다. */
}

/* 2. TIM6를 인터럽트 방식으로 시작하면 100 ms마다 호출됩니다. 우선순위: 4.
 * 시작 호출: HAL_TIM_Base_Start_IT(&htim6);
 */
static void tim6_isr_p4(void)
{
    /* 실습 코드 작성: 횟수를 세거나 메인 루프에서 처리할 표시를 남깁니다. */
}

/* 3. PC 통신 UART2의 요청한 수신이 완료되면 호출됩니다. 우선순위: 6.
 * 먼저 수신 버퍼를 준비하고 HAL_UART_Receive_IT로 수신을 요청해야 합니다.
 * 다음 입력도 받으려면 처리 후 수신을 다시 요청합니다.
 * 현재 표준 입력은 기다리는 방식입니다. 같은 UART의 인터럽트 수신과 함께 쓰지 않습니다.
 */
static void uart2_isr_p6(void)
{
    /* 실습 코드 작성: PC 수신 버퍼를 처리하고 다음 수신을 요청합니다. */
}

/* 4. FPGA 통신 UART3의 요청한 수신이 완료되면 호출됩니다. 우선순위: 5.
 * 먼저 수신 버퍼를 준비하고 HAL_UART_Receive_IT로 수신을 요청해야 합니다.
 * 다음 입력도 받으려면 처리 후 수신을 다시 요청합니다.
 */
static void uart3_isr_p5(void)
{
    /* 실습 코드 작성: FPGA 수신 버퍼를 처리하고 다음 수신을 요청합니다. */
}

/* PA0: 풀업 입력의 HIGH -> LOW 변화. 우선순위: 8. */
static void gpio_in_pullup_isr_p8_ece2(void)
{
    /* 실습 코드 작성 */
}

/* PA1: 풀다운 입력의 LOW -> HIGH 변화. 우선순위: 9. */
static void gpio_in_pulldown_isr_p9_ece2(void)
{
    /* 실습 코드 작성 */
}

/* 아래는 HAL 콜백과 실습 함수를 연결하는 코드입니다. */

/* CubeMX의 EXTI 처리 코드에서 버튼 실습 함수로 연결합니다. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == BUTTON_ece2_Pin)
    {
        button_isr_p7();
    }
    else if (GPIO_Pin == GPIO_IN_PULLUP_ece2_Pin)
    {
        gpio_in_pullup_isr_p8_ece2();
    }
    else if (GPIO_Pin == GPIO_IN_PULLDOWN_ece2_Pin)
    {
        gpio_in_pulldown_isr_p9_ece2();
    }
}

/* CubeMX의 타이머 처리 코드에서 TIM6 실습 함수로 연결합니다. */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        tim6_isr_p4();
    }
}

/* UART 수신 완료 시 PC 통신과 FPGA 통신을 구분합니다. */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2)
    {
        uart2_isr_p6();
    }
    else if (huart->Instance == USART3)
    {
        uart3_isr_p5();
    }
}
