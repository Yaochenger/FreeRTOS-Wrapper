/*
 * Copyright (c) 2006-2022, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-08-24     yangjie      the first version
 */

/*
 * Demo: semaphore
 * This demo creates one binary semaphore statically
 * It creates two tasks:
 *    1) task #1: give the semaphore
 *    2) task #2: take the semaphore
 *
 */

#include <FreeRTOS.h>
#include <semphr.h>
#include <task.h>

#define TASK_PRIORITY         25

/* Semaphore handle */
static SemaphoreHandle_t static_sem = NULL;
/* Buffer to store semaphore structure */
static StaticSemaphore_t xMutexBuffer;
static TaskHandle_t TaskHandle1 = NULL;
static TaskHandle_t TaskHandle2 = NULL;

static void vTask1Code(void *pvParameters)
{
    static BaseType_t result;
    static BaseType_t count = 0;

    while (1)
    {
        if (count <= 100)
        {
            count++;
        }
        else
            return;

        /* Release the semaphore when count is incremented by 10 */
        if (0 == (count % 10))
        {
            rt_kprintf("task1 release a static semaphore.\n");
            result = xSemaphoreGive(static_sem);
            if (result != pdPASS)
            {
                rt_kprintf("task1 release a static semaphore, failed.\n");
                return;
            }
        }
    }
}

static void vTask2Code(void * pvParameters)
{
    static BaseType_t result;
    static BaseType_t number = 0;
    while (1)
    {
        /* Block on the semaphore indefinitely. Increment number after taking the semaphore */
        result = xSemaphoreTake(static_sem, portMAX_DELAY);
        if (result != pdPASS)
        {
            rt_kprintf("task2 take a static semaphore, failed.\n");
            return;
        }
        else
        {
            number++;
            rt_kprintf("task2 take a static semaphore. number = %d\n", number);
        }
    }
}

int semaphore_binary_static()
{
    /* Create a binary semaphore statically */
    static_sem = xSemaphoreCreateBinaryStatic(&xMutexBuffer);
    if (static_sem == NULL)
    {
        rt_kprintf("create static semaphore failed.\n");
        return -1;
    }
    xTaskCreate( vTask1Code, "Task1", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY, &TaskHandle1 );
    if (TaskHandle1 == NULL)
    {
        rt_kprintf("Create task 1 failed\n");
        return -1;
    }
    xTaskCreate( vTask2Code, "Task2", configMINIMAL_STACK_SIZE, NULL, TASK_PRIORITY - 1, &TaskHandle2 );
    if (TaskHandle2 == NULL)
    {
        rt_kprintf("Create task 2 failed\n");
        return -1;
    }

    return 0;
}

MSH_CMD_EXPORT(semaphore_binary_static, semaphore sample);
