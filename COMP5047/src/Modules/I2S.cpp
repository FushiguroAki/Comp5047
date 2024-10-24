#include "I2S.h"
#define SAMPLE_RATE (16000)
#define PIN_I2S_BCLK 14 // SCK
#define PIN_I2S_LRC 15 // WS
#define PIN_I2S_DIN 13  //SD
#define PIN_I2S_DOUT    I2S_PIN_NO_CHANGE


I2S::I2S() {
  BITS_PER_SAMPLE = I2S_BITS_PER_SAMPLE_32BIT;

  // I2S configuration specifically for the INMP441 microphone
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = 0,
    .dma_buf_count = 16,
    .dma_buf_len = 60
  };
  
  i2s_pin_config_t pin_config;
  pin_config.bck_io_num = PIN_I2S_BCLK;
  pin_config.ws_io_num = PIN_I2S_LRC;
  pin_config.data_out_num = I2S_PIN_NO_CHANGE;
  pin_config.data_in_num = PIN_I2S_DIN;

  // Install and configure I2S driver
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  i2s_set_clk(I2S_NUM_0, SAMPLE_RATE, BITS_PER_SAMPLE, I2S_CHANNEL_STEREO);
}

int I2S::Read(char* data, int numData) {
  return i2s_read_bytes(I2S_NUM_0, (char *)data, numData, portMAX_DELAY);
}

int I2S::GetBitPerSample() {
  return (int)BITS_PER_SAMPLE;
}