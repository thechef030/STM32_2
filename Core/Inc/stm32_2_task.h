#ifndef STM32_2_TASK_H
#define STM32_2_TASK_H

/* 빌드 프리셋에서 과제 5/6/7을 선택합니다. 별도 지정이 없으면 과제 5. */
#ifndef STM32_2_TASK
#define STM32_2_TASK 5
#endif
#if STM32_2_TASK != 5 && STM32_2_TASK != 6 && STM32_2_TASK != 7
#error "STM32_2_TASK must be 5, 6 or 7"
#endif

#endif
