#include "mcp3008.h"
#include "esp_log.h"
#include "definitions.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t spi_mutex;

static const char *TAG = "MCP3008";

mcp3008_Status_t mcp3008_init(mcp3008_t *dev) {
    
    // Se indica el pin, velocidad y modo del SPI
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1000000, 
        .mode = 0,
        .spics_io_num = CS_ADC,
        .queue_size = 7,
    };

    // Se agrega el dispositivo al bus SPI
    if(spi_bus_add_device(SPI2_HOST, &dev_cfg, &dev->spi_handle) != ESP_OK) {
        ESP_LOGE(TAG, "No se pudo agregar el ADC al bus SPI");
        return MCP3008_ERR_SPI;
    }

    return MCP3008_OK;
}

mcp3008_Status_t mcp3008_read_channel (mcp3008_t *dev, uint8_t channel, uint16_t *result)
{
    if (spi_mutex == NULL) 
    {
        return MCP3008_ERROR;
    }

    if (xSemaphoreTake(spi_mutex, portMAX_DELAY))
    {
        uint8_t tx_data[3] = {START_BYTE, channel, DUMMY_BYTE};
        uint8_t rx_data[3] = {0};

        // Se prepara el comando
        spi_transaction_t t = {0};
        t.length = 8 * 3;
        t.tx_buffer = tx_data;
        t.rx_buffer = rx_data;

        // Se manda el comando por SPI
        esp_err_t ret = spi_device_transmit(dev->spi_handle, &t);

        xSemaphoreGive(spi_mutex);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Fallo al enviar el comando");
            return MCP3008_ERR_SPI;
        }

        // El resultado esta en los ultimos 10 bits
        *result = ((rx_data[1] & 0x03) << 8) | rx_data[2];
        
        return MCP3008_OK;
    }
    return MCP3008_ERROR;
}

mcp3008_Status_t mcp3008_raw_to_mv(mcp3008_t *dev, uint8_t channel, uint16_t *vref_mv)
{
    uint16_t result;

    // Se llama a la funcion "mcp3008_read_channel"
    if(mcp3008_read_channel(dev, channel, &result) != MCP3008_OK)
    {
        ESP_LOGE(TAG, "Fallo al obtener raw");
        return MCP3008_ERR_SPI;
    }

    // Se convierte de raw a mV, teniendo en cuenta VREF
    *vref_mv = (uint16_t)(((uint32_t)result * VREF) / 1024);
    return MCP3008_OK;
}

mcp3008_Status_t mcp3008_amperios(mcp3008_t *dev, uint8_t channel, float *amp)
{
    uint16_t vref_mv;

    // Se llama a la funcion "mcp3008_raw_to_mv"
    if(mcp3008_raw_to_mv(dev, channel, &vref_mv) != MCP3008_OK)
    {
        return MCP3008_ERROR;
    }

    // Se convierte de mV a amperios, teniendo en cuenta el offset y la sensibilidad del ACS712
    *amp = ((float)vref_mv - ACS712_OFFSET_MV) / ACS712_SENSITIVITY;

    return MCP3008_OK;
}