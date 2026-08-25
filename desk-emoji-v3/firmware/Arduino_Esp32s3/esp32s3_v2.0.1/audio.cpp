#include "audio.h"

i2s_config_t i2sIn_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMA_BUF_COUNT,
    .dma_buf_len = DMA_BUF_LEN
};

const i2s_pin_config_t i2sIn_pin_config = {
    .bck_io_num = INMP441_SCK,
    .ws_io_num = INMP441_WS,
    .data_out_num = -1,
    .data_in_num = INMP441_SD
};

i2s_config_t i2sOut_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = DMA_BUF_COUNT,
    .dma_buf_len = DMA_BUF_LEN
};

const i2s_pin_config_t i2sOut_pin_config = {
    .bck_io_num = MAX98357_BCLK,
    .ws_io_num = MAX98357_LRC,
    .data_out_num = MAX98357_DIN,
    .data_in_num = -1
};

void setup_audio() {
    pinMode(MAX98357_GAIN, INPUT);
    pinMode(MAX98357_SD, OUTPUT);
    digitalWrite(MAX98357_SD, HIGH);

    i2s_driver_install(I2S_NUM_0, &i2sIn_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &i2sIn_pin_config);

    i2s_driver_install(I2S_NUM_1, &i2sOut_config, 0, NULL);
    i2s_set_pin(I2S_NUM_1, &i2sOut_pin_config);

    log_info("Audio is Ready.");
}

void record(int16_t *data, size_t length) {
  size_t bytes_read;
  i2s_read(I2S_NUM_0, data, length * sizeof(int16_t), &bytes_read, portMAX_DELAY);
}

void enhanceVoice(int16_t *data, size_t length) {
  const float a0 = 0.2;
  const float a1 = 0.8;
  static int16_t prev_sample = 0;
  
  for (int i = 0; i < length; i++) {
    int16_t filtered = a0 * data[i] + a1 * prev_sample;
    prev_sample = filtered;
    data[i] = constrain(filtered * 5, -32768, 32767);
  }
}

void play(const int16_t *data, size_t length, float volume_ratio) {
  size_t bytes_written;
  for(size_t i = 0; i < length; i++) {
    int16_t scaled_sample = data[i] * volume_ratio;
    i2s_write(I2S_NUM_1, &scaled_sample, sizeof(int16_t), &bytes_written, portMAX_DELAY);
  }
}

void stop_play() {
  i2s_zero_dma_buffer(I2S_NUM_1);
}

size_t calculate_mean(const int16_t *data, size_t length) {
  size_t mean = 0;
  size_t count = 0;
  for (size_t i = 0; i < length; i += 1000) {
    mean += abs(data[i]);
    count++;
  }
  return mean / count;
}
