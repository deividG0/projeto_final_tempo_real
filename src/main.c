// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/i2s.h"

// #define SAMPLE_RATE (16000)
// #define BUFFER_LENGTH (1024)

// #define I2S_PORT I2S_NUM_0 // I2S port number
// #define I2S_WS 15
// #define I2S_SD 13
// #define I2S_SCK 2

// void i2sMicrophoneTask(void *pvParameters)
// {
//   uint8_t *audioBuffer = (uint8_t *)malloc(BUFFER_LENGTH);
//   size_t bytesRead = 0;

//   while (1)
//   {
//     // Read audio data from I2S
//     i2s_read(I2S_NUM_0, audioBuffer, BUFFER_LENGTH, &bytesRead, portMAX_DELAY);

//     // Print some debug information
//     printf("Read %d bytes from I2S   |   And the first the sample is %d\n", bytesRead, audioBuffer[0]);
//   }

//   free(audioBuffer);
//   vTaskDelete(NULL);
// }

// void init_microfone()
// {
//   // Initialize the I2S interface
//   i2s_config_t i2s_config = {
//       .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
//       .sample_rate = SAMPLE_RATE,
//       .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
//       .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
//       // .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
//       .communication_format = I2S_COMM_FORMAT_I2S_MSB,
//       .intr_alloc_flags = 0,
//       .dma_buf_count = 8,
//       .dma_buf_len = BUFFER_LENGTH,
//       .use_apll = false,
//       // .tx_desc_auto_clear = false,
//       // .fixed_mclk = 0
//   };
//   i2s_pin_config_t pin_config = {
//       .bck_io_num = I2S_SCK,
//       .ws_io_num = I2S_WS,
//       .data_out_num = -1,
//       .data_in_num = I2S_SD};
//   i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
//   i2s_set_pin(I2S_PORT, &pin_config);
//   i2s_zero_dma_buffer(I2S_PORT);
// }

// void app_main()
// {
//   init_microfone();
//   // Set up a task for capturing audio from the microphone
//   xTaskCreatePinnedToCore(i2sMicrophoneTask, "i2sMicrophoneTask", 2048, NULL, 1, NULL, 0);
// }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/queue.h"
// #include "driver/i2s.h"

// #define SAMPLE_RATE (16000)
// #define BUFFER_LENGTH (1024)

// #define I2S_PORT I2S_NUM_0 // I2S port number
// #define I2S_WS 15
// #define I2S_SD 13
// #define I2S_SCK 2

// QueueHandle_t arrayQueue;

// void i2sMicrophoneTask(void *pvParameters)
// {
//   uint8_t *audioBuffer = (uint8_t *)malloc(BUFFER_LENGTH);
//   size_t bytesRead = 0;

//   while (1)
//   {
//     // Read audio data from I2S
//     i2s_read(I2S_NUM_0, audioBuffer, BUFFER_LENGTH, &bytesRead, portMAX_DELAY);

//     xQueueSend(arrayQueue, &audioBuffer, portMAX_DELAY);

//     // Process the audio data here

//     // Print some debug information
//     printf("Read %d bytes from I2S   |   And the first the sample is %d\n", bytesRead, audioBuffer[0]);
//   }

//   free(audioBuffer);
//   vTaskDelete(NULL);
// }

// void TaskAudio1(void *pvParameters)
// {
//   int receivedArray[BUFFER_LENGTH];
//   while (1)
//   {
//     if (xQueueReceive(arrayQueue, &receivedArray, portMAX_DELAY) == pdPASS)
//     {
//       printf("Received array: ");
//       float mean = 0;
//       for (int16_t i = 0; i < BUFFER_LENGTH; ++i)
//       {
//         mean += (receivedArray[i]);
//       }

//       mean /= BUFFER_LENGTH;

//       printf("Mean: %f\n", mean);
//     }
//     else
//     {
//       printf("Failed to receive array from the queue\n");
//     }
//   }
// }

// void init_microfone()
// {
//   // Initialize the I2S interface
//   i2s_config_t i2s_config = {
//       .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
//       .sample_rate = SAMPLE_RATE,
//       .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
//       .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
//       // .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
//       .communication_format = I2S_COMM_FORMAT_I2S_MSB,
//       .intr_alloc_flags = 0,
//       .dma_buf_count = 8,
//       .dma_buf_len = BUFFER_LENGTH,
//       .use_apll = false,
//       // .tx_desc_auto_clear = false,
//       // .fixed_mclk = 0
//   };
//   i2s_pin_config_t pin_config = {
//       .bck_io_num = I2S_SCK,
//       .ws_io_num = I2S_WS,
//       .data_out_num = -1,
//       .data_in_num = I2S_SD};
//   i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
//   i2s_set_pin(I2S_PORT, &pin_config);
//   i2s_zero_dma_buffer(I2S_PORT);
// }

// void app_main()
// {
//   init_microfone();
//   // Create the array queue
//   arrayQueue = xQueueCreate(BUFFER_LENGTH, sizeof(int));
//   // Set up a task for capturing audio from the microphone
//   xTaskCreatePinnedToCore(i2sMicrophoneTask, "i2sMicrophoneTask", 2048, NULL, 1, NULL, 0);
//   xTaskCreate(TaskAudio1, "AudioTask1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
// }

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "freertos/semphr.h"
// #include "freertos/queue.h"
// #include "driver/gpio.h"

// // Define the data buffer size
// #define BUFFER_SIZE 1024

// // Declare three data buffers
// uint8_t buffer1[BUFFER_SIZE];
// uint8_t buffer2[BUFFER_SIZE];
// uint8_t buffer3[BUFFER_SIZE];

// // Declare semaphore to synchronize access to the buffers
// SemaphoreHandle_t bufferSemaphore;

// // Task function for consuming audio data from buffer 1
// void TaskAudio1(void* pvParameters)
// {
//   while (1)
//   {
//     // Wait for access to buffer 1
//     xSemaphoreTake(bufferSemaphore, portMAX_DELAY);

//     // Process audio data from buffer 1
//     // ...

//     // Release the buffer semaphore
//     xSemaphoreGive(bufferSemaphore);
//   }
// }

// // Task function for consuming audio data from buffer 2
// void TaskAudio2(void* pvParameters)
// {
//   while (1)
//   {
//     // Wait for access to buffer 2
//     xSemaphoreTake(bufferSemaphore, portMAX_DELAY);

//     // Process audio data from buffer 2
//     // ...

//     // Release the buffer semaphore
//     xSemaphoreGive(bufferSemaphore);
//   }
// }

// // Task function for consuming audio data from buffer 3
// void TaskAudio3(void* pvParameters)
// {
//   while (1)
//   {
//     // Wait for access to buffer 3
//     xSemaphoreTake(bufferSemaphore, portMAX_DELAY);

//     // Process audio data from buffer 3
//     // ...

//     // Release the buffer semaphore
//     xSemaphoreGive(bufferSemaphore);
//   }
// }

// // I2S interrupt handler
// void I2S_InterruptHandler(void)
// {
//   // Read audio data from the I2S data register

//   // Determine which buffer to store the data in
//   // (e.g., buffer1, buffer2, or buffer3)

//   // Wait for access to the buffer
//   xSemaphoreTake(bufferSemaphore, portMAX_DELAY);

//   // Store the audio data in the buffer

//   // Release the buffer semaphore
//   xSemaphoreGive(bufferSemaphore);

//   // Clear the interrupt flag
// }

// void app_main()
// {
//   // Initialize FreeRTOS and other peripherals

//   // Create the semaphore to synchronize buffer access
//   bufferSemaphore = xSemaphoreCreateBinary();

//   // Create the tasks
//   xTaskCreate(TaskAudio1, "AudioTask1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
//   xTaskCreate(TaskAudio2, "AudioTask2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
//   xTaskCreate(TaskAudio3, "AudioTask3", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

//   // Start the scheduler
//   vTaskStartScheduler();

//   // Will only reach here if there is an error starting the scheduler

//   // return 0;
// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////// TESTE FUNCIONAL DO FREE RTOS ////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include <stdio.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "driver/gpio.h"

// #define LED_PIN GPIO_NUM_2

// bool LED_on = true;

// // Task function for Task1
// void Task1(void *pvParameters)
// {
//   while (1)
//   {
//     printf("Task 1 is running %d\n", gpio_get_level(LED_PIN));
//     if(LED_on){
//       gpio_set_level(LED_PIN, 1);
//       LED_on = false;
//     }else{
//       gpio_set_level(LED_PIN, 0);
//       LED_on = true;
//     }

//     vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
//   }
// }

// // Task function for Task2
// void Task2(void *pvParameters)
// {
//   while (1)
//   {
//     printf("Task 2 is running\n");
//     vTaskDelay(pdMS_TO_TICKS(2000)); // Delay for 2 seconds
//   }
// }

// void app_main()
// {
//   gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

//   // Create Task1
//   xTaskCreate(Task1, "Task1", 2048, NULL, 1, NULL);

//   // Create Task2
//   xTaskCreate(Task2, "Task2", 2048, NULL, 2, NULL);
// }
