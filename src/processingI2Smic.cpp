#include <stdio.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <math.h>
#include "driver/i2s.h"

#include <string.h>
#include <arduinoFFT.h>

#define SAMPLE_RATE (16000)
#define BUFFER_LENGTH (1024)

#define I2S_PORT I2S_NUM_0 // I2S port number
#define I2S_WS 15
#define I2S_SD 13
#define I2S_SCK 2

float noiseFloor = 0.0;
const float noiseThreshold = 1.0;
bool voiceDetected = false;

// Define global variables
// uint16_t microphoneData[BUFFER_LENGTH];
int16_t microphoneData[BUFFER_LENGTH];
SemaphoreHandle_t microphoneMutex;

// Threshold for mean
#define MEAN_THRESHOLD 60

// Parte da media móvel
#define MOVING_AVERAGE_SIZE 20
double movingAverage[MOVING_AVERAGE_SIZE];
int movingAverageIndex = 0;
double movingAverageSum = 0;

double movingAverageFilter(double inputValue)
{
    // remove o valor mais antigo da média móvel
    movingAverageSum -= movingAverage[movingAverageIndex];
    // adiciona o novo valor na média móvel
    movingAverageSum += inputValue;
    // armazena o novo valor no buffer da média móvel
    movingAverage[movingAverageIndex] = inputValue;
    // atualiza o índice do buffer
    movingAverageIndex = (movingAverageIndex + 1) % MOVING_AVERAGE_SIZE;
    // calcula a média móvel
    double movingAverageValue = movingAverageSum / MOVING_AVERAGE_SIZE;
    return movingAverageValue;
}

void i2sMicrophoneTask(void *pvParameters)
{
    // uint16_t *audioBuffer = (uint16_t *)malloc(BUFFER_LENGTH);
    int16_t *audioBuffer = (int16_t *)malloc(BUFFER_LENGTH);
    size_t bytesRead = 0;

    while (1)
    {
        // Read audio data from I2S
        esp_err_t result = i2s_read(I2S_NUM_0, audioBuffer, BUFFER_LENGTH, &bytesRead, portMAX_DELAY); // no timeout
        if (result == ESP_OK && bytesRead > 0)
        {
            // Acquire the microphone mutex
            xSemaphoreTake(microphoneMutex, portMAX_DELAY);
            // Update the global variable with the new data
            memcpy(microphoneData, audioBuffer, BUFFER_LENGTH);
            // Release the microphone mutex
            xSemaphoreGive(microphoneMutex);
            // Delay or yield to other tasks if necessary
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Example delay of 10 milliseconds
    }
    free(audioBuffer);
    vTaskDelete(NULL);
}

// Define task function
void CalculateFFT(void *pvParameters)
{
    arduinoFFT FFT = arduinoFFT(); // Create FFT object
    const double samplingFrequency = SAMPLE_RATE;
    double *vReal = new double[BUFFER_LENGTH];
    double *vImag = new double[BUFFER_LENGTH];
    double freq;

    while (1)
    {
        if (xSemaphoreTake(microphoneMutex, portMAX_DELAY) == pdTRUE)
        {
            for (size_t i = 0; i < BUFFER_LENGTH; i++)
            {
                vReal[i] = microphoneData[i] / 1.0;
                vImag[i] = 0.0; // Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
            }
            FFT.Windowing(vReal, BUFFER_LENGTH, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
            FFT.Compute(vReal, vImag, BUFFER_LENGTH, FFT_FORWARD);
            FFT.ComplexToMagnitude(vReal, vImag, BUFFER_LENGTH);
            double x = FFT.MajorPeak(vReal, BUFFER_LENGTH, samplingFrequency);
            freq = movingAverageFilter(x);
            printf(" | Frequency from FFT %f | ", freq);

            xSemaphoreGive(microphoneMutex);
        }
        else
        {
            printf("Failed to take semaphore in task 1");
        }
        // Delay or yield to other tasks if necessary
        vTaskDelay(pdMS_TO_TICKS(10)); // Example delay of 100 milliseconds
    }
}

// Define task function
void DetectAbnormalSignal(void *pvParameters)
{
    while (1)
    {
        // Acquire the microphone mutex
        if (xSemaphoreTake(microphoneMutex, portMAX_DELAY) == pdTRUE)
        {
            // Calculate the mean of the audio buffer
            float mean = 0.0;
            for (int16_t i = 0; i < BUFFER_LENGTH / 8; ++i)
            {
                mean = mean + microphoneData[i] / 1.0;
            }
            // Check if the mean exceeds the threshold
            mean = mean / (BUFFER_LENGTH / 8);
            if (fabs(mean) > MEAN_THRESHOLD)
            {
                printf(" ## Mean %f exceeded the threshold! ## \n", mean);
            }
            else
            {
                printf(" | Mean %f | \n", mean);
            }

            // Release the microphone mutex
            xSemaphoreGive(microphoneMutex);
        }
        else
        {
            printf("Failed to take semaphore in task 2");
        }

        // Delay or yield to other tasks if necessary
        vTaskDelay(pdMS_TO_TICKS(10));
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
        // printf("TaskAudio3 reading microfone %d %d %d %d %d\n", microphoneData[0], microphoneData[1], microphoneData[2], microphoneData[3], microphoneData[4]);
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

extern "C" void app_main()
{
    init_microfone();

    // Create the microphone mutex
    microphoneMutex = xSemaphoreCreateMutex();

    // Set up a task for capturing audio from the microphone
    xTaskCreatePinnedToCore(i2sMicrophoneTask, "i2sMicrophoneTask", 2048, NULL, 1, NULL, 0);

    xTaskCreate(CalculateFFT, "CalculateFFT", 2048, NULL, 1, NULL);
    xTaskCreate(DetectAbnormalSignal, "DetectAbnormalSignal", 2048, NULL, 1, NULL);
    // xTaskCreate(TaskAudio3, "AudioTask3", 2048, NULL, 1, NULL);
}
