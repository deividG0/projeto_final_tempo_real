// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"

// #define ARRAY_LENGTH 5

// QueueHandle_t arrayQueue;

// void producerTask(void* pvParameters) {
//     int array[ARRAY_LENGTH] = {1, 2, 3, 4, 5};

//     while (1) {
//         // Send the array to the queue
//         if (xQueueSend(arrayQueue, array, portMAX_DELAY) != pdPASS) {
//             printf("Failed to send array to the queue\n");
//         }

//         // Delay before sending the array again
//         vTaskDelay(pdMS_TO_TICKS(2000));
//     }
// }

// void consumerTask(void* pvParameters) {
//     int receivedArray[ARRAY_LENGTH];

//     while (1) {
//         // Receive the array from the queue
//         if (xQueueReceive(arrayQueue, receivedArray, portMAX_DELAY) == pdPASS) {
//             printf("Received array: ");
//             for (int i = 0; i < ARRAY_LENGTH; i++) {
//                 printf("%d ", receivedArray[i]);
//             }
//             printf("\n");
//         } else {
//             printf("Failed to receive array from the queue\n");
//         }
//     }
// }

// void app_main() {
//     // Create the array queue
//     arrayQueue = xQueueCreate(1, sizeof(int) * ARRAY_LENGTH);

//     // Create the producer task
//     xTaskCreate(producerTask, "ProducerTask", 2048, NULL, 1, NULL);

//     // Create the consumer task
//     xTaskCreate(consumerTask, "ConsumerTask", 2048, NULL, 1, NULL);
// }