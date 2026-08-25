#ifndef Audio_h
#define Audio_h

#include <Arduino.h>
#include <driver/i2s.h>
#include <FFat.h>
#include "common.h"

// microphone and speaker
#define INMP441_WS GPIO_NUM_46
#define INMP441_SCK GPIO_NUM_21
#define INMP441_SD GPIO_NUM_14

#define MAX98357_LRC GPIO_NUM_18
#define MAX98357_BCLK GPIO_NUM_17
#define MAX98357_DIN GPIO_NUM_19
#define MAX98357_SD GPIO_NUM_3
#define MAX98357_GAIN GPIO_NUM_20

// parameters
#define SAMPLE_RATE 16000
#define DMA_BUF_COUNT 8
#define DMA_BUF_LEN 1024
#define SOUND_THRESHOLD 180

// i2s
extern i2s_config_t i2sIn_config;
extern i2s_config_t i2sOut_config;
extern const i2s_pin_config_t i2sIn_pin_config;
extern const i2s_pin_config_t i2sOut_pin_config;

void setup_audio();
void record(int16_t *data, size_t length = DMA_BUF_LEN);
void enhanceVoice(int16_t *data, size_t length = DMA_BUF_LEN);
void play(const int16_t *data, size_t length = DMA_BUF_LEN, float volume_ratio = 1.0);
void stop_play();
size_t calculate_mean(const int16_t *data, size_t length);

#endif