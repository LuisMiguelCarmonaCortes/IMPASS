#ifndef UTILS_H
#define UTILS_H

#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "definitions.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/spi_master.h"

esp_err_t i2c_write_reg_8(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t reg, uint8_t value);
esp_err_t i2c_read_reg_8(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t reg, uint8_t *value);

esp_err_t i2c_write_reg_16(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t reg, uint16_t value);
esp_err_t i2c_read_reg_16(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t reg, uint16_t *value);

esp_err_t i2c_init(uint8_t i2c_controller, uint32_t i2c_freq);
esp_err_t spi_bus_init(spi_host_device_t spi_host);

#endif