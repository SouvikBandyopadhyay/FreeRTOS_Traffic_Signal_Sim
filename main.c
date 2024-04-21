#include <stdlib.h> 
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define NUM_QUEUES 4
#define NUM_MUTEXES 4

QueueHandle_t queues[NUM_QUEUES];
SemaphoreHandle_t mutexes[NUM_MUTEXES];
QueueHandle_t emergencyQueue;


void addItemsToQueuesTask(void* pvParameters) {
    while (1) {
        // Selecting a lane to add traffic
        int queueIndex = rand() % NUM_QUEUES;
        // number of traffic to add
        int numItemsToAdd = rand() % 6;

        if(numItemsToAdd)
            printf("%d Cars Arrived at Lane %d\n", numItemsToAdd, queueIndex +1);

        // Lock the respective mutex to add traffic to respective lane 
        if (xSemaphoreTake(mutexes[queueIndex], (TickType_t)10) == pdTRUE) {
            for (int i = 0; i < numItemsToAdd; i++) {
                int item = 1;
                xQueueSend(queues[queueIndex], &item, portMAX_DELAY);
            }
            // Unlock the mutex
            xSemaphoreGive(mutexes[queueIndex]);
        }
        // randomly delay for some time
        vTaskDelay(pdMS_TO_TICKS( rand() % 5000));
    }
}

void emergency(void* pvParameters) {
    while (1) {
        // randomly delay some time
        TickType_t xDelay = pdMS_TO_TICKS((rand() % 50000));
        vTaskDelay(xDelay);

        // Choose a lane to add emergency vehicle
        int k = (rand() % NUM_QUEUES) + 1;

        // Lock the corresponding mutex
        if (xSemaphoreTake(mutexes[k - 1], (TickType_t)10) == pdTRUE) {
            // Add emergency vehicle to the selected lane
            int item = k;
            xQueueSend(queues[k - 1], &item, portMAX_DELAY);

            // Unlock the mutex
            xSemaphoreGive(mutexes[k - 1]);
        }

        // Add lane number to the emergency queue
        xQueueSend(emergencyQueue, &k, portMAX_DELAY);
        printf("Emergency Vehicle Waiting at Lane %d\n", k);
    }
}


void clearQueueTask(void* pvParameters) {
    vTaskDelay(500);
    while (1) {
        int k;
        int emergencyItem;

        // Check if there is an item in the emergency queue
        if (xQueueReceive(emergencyQueue, &emergencyItem, (TickType_t)0) == pdTRUE) {
            k = emergencyItem;
            int length1,length = 0;
            // set green to lane with emergency vehicle, waiting for the longest time
            printf("\t\t\t\tGreen %d : ", k );
            // Lock the corresponding mutex
            if (xSemaphoreTake(mutexes[k - 1], (TickType_t)10) == pdTRUE) {
                // Check traffic length of that lane
                length = uxQueueMessagesWaiting(queues[k - 1]);
                length1 = length;

                // Release the lane
                while (length > 0) {
                    int item;
                    xQueueReceive(queues[k - 1], &item, portMAX_DELAY);
                    length--;
                }

                // Unlock the mutex
                xSemaphoreGive(mutexes[k - 1]);
                printf("%d Cars released, Due to Emergency at Lane %d\n", length1, k);
            }

            // Wait for L seconds for traffic to pass
            if (length1 == 0)
                length1 = 1;
            vTaskDelay(pdMS_TO_TICKS(length1 * 1000));
        }
        // If there is no emergency vehicle
        else {
            int maxQueueIndex = 0;
            int maxLength = uxQueueMessagesWaiting(queues[0]);
            // Find the lane with most traffic
            for (int i = 0; i < NUM_QUEUES; i++) {
                int length = uxQueueMessagesWaiting(queues[i]);
                printf("\t\t\t\t%d Cars : Waiting - Lane %d\n", length, i + 1);
                if (length > maxLength) {
                    maxLength = length;
                    maxQueueIndex = i;
                }
            }
            if (maxLength) {

            printf("\t\t\t\tGreen %d : ", maxQueueIndex + 1);
            // Lock the corresponding mutex
            if (xSemaphoreTake(mutexes[maxQueueIndex], (TickType_t)10) == pdTRUE) {
                // Clear the lane with highest traffic
                for (int i = 0; i < maxLength; i++) {
                    int item;
                    xQueueReceive(queues[maxQueueIndex], &item, portMAX_DELAY);
                }

                // Unlock the mutex
                xSemaphoreGive(mutexes[maxQueueIndex]);
                printf("%d Cars Released from Lane %d\n", maxLength, maxQueueIndex +1);
            }
            }
            if (maxLength == 0)
                maxLength = 1;
            // Wait for L seconds for the traffic to pass
            vTaskDelay(pdMS_TO_TICKS(maxLength * 1000));

        }
    }
}


int main(void) {
    // Create queues
    for (int i = 0; i < NUM_QUEUES; i++) {
        queues[i] = xQueueCreate(30, sizeof(int));
    }
    emergencyQueue = xQueueCreate(10, sizeof(int));

    // Create mutexes
    for (int i = 0; i < NUM_MUTEXES; i++) {
        mutexes[i] = xSemaphoreCreateMutex();
        xSemaphoreGive(mutexes[i]);
    }

    // Create tasks
    xTaskCreate(addItemsToQueuesTask, "Task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1 , NULL);
    xTaskCreate(emergency, "Task2", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY , NULL);
    xTaskCreate(clearQueueTask, "Task3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    // Should never reach here
    while (1);
    return 0;
}
