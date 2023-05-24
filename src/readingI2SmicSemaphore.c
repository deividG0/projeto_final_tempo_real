#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include <string.h>

#define SAMPLE_RATE (16000)
#define BUFFER_LENGTH (1024)

#define I2S_PORT I2S_NUM_0 // I2S port number
#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2

// Define global variables
uint16_t microphoneData[BUFFER_LENGTH];
SemaphoreHandle_t microphoneMutex;

void i2sMicrophoneTask(void *pvParameters)
{
    uint16_t *audioBuffer = (uint16_t *)malloc(BUFFER_LENGTH);
    size_t bytesRead = 0;

    while (1)
    {
        // Read audio data from I2S
        i2s_read(I2S_NUM_0, audioBuffer, BUFFER_LENGTH, &bytesRead, portMAX_DELAY);

        // Acquire the microphone mutex
        xSemaphoreTake(microphoneMutex, portMAX_DELAY);
        // Update the global variable with the new data
        memcpy(microphoneData, audioBuffer, BUFFER_LENGTH);
        // Release the microphone mutex
        xSemaphoreGive(microphoneMutex);
        // Delay or yield to other tasks if necessary
        vTaskDelay(pdMS_TO_TICKS(10)); // Example delay of 10 milliseconds

        // // Print some debug information
        // printf("Read %d bytes from I2S   |   And the first the sample is %d\n", bytesRead, audioBuffer[0]);
    }

    free(audioBuffer);
    vTaskDelete(NULL);
}

// Define task function
void TaskAudio1(void *pvParameters)
{
    while (1)
    {
        // Acquire the microphone mutex
        xSemaphoreTake(microphoneMutex, portMAX_DELAY);

        // Print some debug information
        printf("TaskAudio1 reading microfone %d %d %d %d %d\n", microphoneData[0], microphoneData[1], microphoneData[2], microphoneData[3], microphoneData[4]);

        // Release the microphone mutex
        xSemaphoreGive(microphoneMutex);

        // Delay or yield to other tasks if necessary
        vTaskDelay(pdMS_TO_TICKS(100)); // Example delay of 100 milliseconds
    }
}

// Define task function
void TaskAudio2(void *pvParameters)
{
    while (1)
    {
        // Acquire the microphone mutex
        xSemaphoreTake(microphoneMutex, portMAX_DELAY);

        // Print some debug information
        printf("TaskAudio2 reading microfone %d %d %d %d %d\n", microphoneData[0], microphoneData[1], microphoneData[2], microphoneData[3], microphoneData[4]);

        // Release the microphone mutex
        xSemaphoreGive(microphoneMutex);

        // Delay or yield to other tasks if necessary
        vTaskDelay(pdMS_TO_TICKS(100)); // Example delay of 100 milliseconds
    }
}

// Define task function
void TaskAudio3(void *pvParameters)
{
    while (1)
    {
        // Acquire the microphone mutex
        xSemaphoreTake(microphoneMutex, portMAX_DELAY);

        // Print some debug information
        printf("TaskAudio3 reading microfone %d %d %d %d %d\n", microphoneData[0], microphoneData[1], microphoneData[2], microphoneData[3], microphoneData[4]);

        // Release the microphone mutex
        xSemaphoreGive(microphoneMutex);

        // Delay or yield to other tasks if necessary
        vTaskDelay(pdMS_TO_TICKS(100)); // Example delay of 100 milliseconds
    }
}

void init_microfone()
{
    // Initialize the I2S interface
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        // .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,
        .intr_alloc_flags = 0,
        .dma_buf_count = 8,
        .dma_buf_len = BUFFER_LENGTH,
        .use_apll = false,
        // .tx_desc_auto_clear = false,
        // .fixed_mclk = 0
    };
    i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1,
        .data_in_num = I2S_SD};
    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
    i2s_zero_dma_buffer(I2S_PORT);
}

void app_main()
{
    init_microfone();

    // Create the microphone mutex
    microphoneMutex = xSemaphoreCreateMutex();

    // Set up a task for capturing audio from the microphone
    xTaskCreatePinnedToCore(i2sMicrophoneTask, "i2sMicrophoneTask", 2048, NULL, 1, NULL, 0);

    xTaskCreate(TaskAudio1, "AudioTask1", 2048, NULL, 1, NULL);
    xTaskCreate(TaskAudio2, "AudioTask2", 2048, NULL, 1, NULL);
    xTaskCreate(TaskAudio3, "AudioTask3", 2048, NULL, 1, NULL);

    // Start the FreeRTOS scheduler
    // vTaskStartScheduler();
}
