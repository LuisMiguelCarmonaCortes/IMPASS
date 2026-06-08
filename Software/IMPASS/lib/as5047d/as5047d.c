#include "as5047d.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

extern SemaphoreHandle_t spi_mutex;

static const char *TAG = "AS5047D";

as5047d_Status_t as5047d_init(as5047d_t *dev)
{
    // Se indica el pin, velocidad y modo del SPI
    spi_device_interface_config_t dev_cfg = {
        .clock_speed_hz = 1000000, 
        .mode = 0,
        .spics_io_num = CS_INC,
        .queue_size = 7,
    };

    // Se agrega el dispositivo al bus SPI
    if(spi_bus_add_device(SPI2_HOST, &dev_cfg, &dev->spi_handle) != ESP_OK) {
        ESP_LOGE(TAG, "No se pudo agregar el inclinometro al bus SPI");
        return AS5047D_ERR_SPI;
    }

    return AS5047D_OK;
}

as5047d_Status_t as5047d_read_register(as5047d_t *dev, uint16_t registro, uint16_t *dato)
{
    if (spi_mutex == NULL) 
    {
        return AS5047D_ERROR;
    }

    if (xSemaphoreTake(spi_mutex, portMAX_DELAY))
    {
        uint16_t frame = registro;
        frame |= 0x40;                              // Operacion leer
        frame &= 0x7FFF;                            // Se quita el bit de paridad

        // Se calcula paridad
        uint8_t parity = 0;
        for(int i = 0; i<15; i++)
        {
            parity ^= (frame>>i) & 1;
        }
        frame = frame | (parity << 15);

        // El commando se fragmenta en dos bytes para mandarlo pos spi
        uint8_t MSB = (registro >> 8) & 0xFF;       
        uint8_t LSB = frame & 0x00FF;
        MSB |= 0b01000000;
        uint8_t tx_data[2] = {MSB, LSB};
        uint8_t rx_data[2] = {0};

        // Se prepara el comando
        spi_transaction_t t = {0};
        t.length = 8 * 2;
        t.tx_buffer = tx_data;
        t.rx_buffer = rx_data;

        // Se manda el comando por SPI
        esp_err_t ret = spi_device_transmit(dev->spi_handle, &t);

        xSemaphoreGive(spi_mutex);

        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Fallo al enviar el comando");
            return AS5047D_ERR_SPI;
        }

        // El frame recibido se guarda en la variable dato
        *dato = rx_data[1] | rx_data[0];
        
        // si el bit 14 es 1 es que hay error
        if((*dato & 0x40) != 0)
        {
            ESP_LOGE(TAG, "Error en el frame mandado");
            return AS5047D_ERR_SPI;
        }

        // El resultado esta en los ultimos 13 bits
        *dato = *dato & 0x3FFF; 

        return AS5047D_OK;
    }
    return AS5047D_ERROR;
}

as5047d_Status_t as5047d_read_bits(as5047d_t *dev, uint16_t registro, uint16_t mascara, uint16_t *dato)
{
    if(as5047d_read_register(dev, registro, dato) != AS5047D_OK)
    {
        return AS5047D_ERROR;
    }

    if(mascara == 0)
    {
        return AS5047D_OK;
    }
    else 
    {
        *dato &= mascara;
        return AS5047D_OK;
    }
}