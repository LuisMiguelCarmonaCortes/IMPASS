#include "utils.h"

#define I2C_TIMEOUT_MS 1000

static SemaphoreHandle_t i2c_mutex = NULL;
static SemaphoreHandle_t spi_mutex = NULL;

// Funcion generica para escribir registro de 8 bits
esp_err_t i2c_write_reg_8(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t reg, uint8_t value)
{
    if (i2c_mutex == NULL) 
    {
        return ESP_ERR_INVALID_STATE;
    }

    // Se bloquea el bus I2C, si no está disponible espera indefinidamente
    if (xSemaphoreTake(i2c_mutex, portMAX_DELAY)) 
    {
        // Se prepara el comando de I2C
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        if (!cmd)
        {
            // Liberar el bus si falla
            xSemaphoreGive(i2c_mutex);
            return ESP_ERR_NO_MEM;
        } 

        i2c_master_start(cmd);
        // Dirección del dispositivo + bit de escritura
        i2c_master_write_byte(cmd, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
        // Registro del dispositivo
        i2c_master_write_byte(cmd, reg, true);
        // Valor a escribir
        i2c_master_write_byte(cmd, value, true);
        i2c_master_stop(cmd);

        // Ejecutar la transacción
        esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
        // Se libera memoria, borrando el comando
        i2c_cmd_link_delete(cmd);
        // Liberar el bus
        xSemaphoreGive(i2c_mutex);

        return ret;
    }

    return ESP_ERR_TIMEOUT;
}

// Funcion generica para leer registro de 8 bits
esp_err_t i2c_read_reg_8(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t reg, uint8_t *value)
{
    if (i2c_mutex == NULL) 
    {
        return ESP_ERR_INVALID_STATE;
    }

    // Se bloquea el bus I2C, si no está disponible espera indefinidamente
    if (xSemaphoreTake(i2c_mutex, portMAX_DELAY))
    {
        // Se prepara el comando de I2C para indicar registro y dispositivo a leer
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        if (!cmd)
        {
            // Liberar el bus si falla
            xSemaphoreGive(i2c_mutex);
            return ESP_ERR_NO_MEM;
        } 

        i2c_master_start(cmd);
        // Dirección del dispositivo + bit de escritura
        i2c_master_write_byte(cmd, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
        // Registro del dispositivo
        i2c_master_write_byte(cmd, reg, true);
        i2c_master_stop(cmd);
        
        // Ejecutar la transacción
        esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
        // Se libera memoria, borrando el comando
        i2c_cmd_link_delete(cmd);
        if(ret != ESP_OK)
        {
            // Liberar el bus si falla
            xSemaphoreGive(i2c_mutex);
            return ret;
        }

        // Se prepara el comando de I2C para leer el valor del registro y dispositivo antes indicado
        cmd = i2c_cmd_link_create();
        if (!cmd)
        {
            // Liberar el bus si falla
            xSemaphoreGive(i2c_mutex);
            return ESP_ERR_NO_MEM;
        } 

        i2c_master_start(cmd);
        // Dirección del dispositivo + bit de lectura
        i2c_master_write_byte(cmd, (i2c_addr << 1) | I2C_MASTER_READ, true);
        // Lectura del valor
        i2c_master_read_byte(cmd, value, I2C_MASTER_NACK);
        i2c_master_stop(cmd);

        // Ejecutar la transacción
        ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
        // Se libera memoria, borrando el comando
        i2c_cmd_link_delete(cmd);
        // Liberar el bus
        xSemaphoreGive(i2c_mutex);

        return ret;
    }

    return ESP_ERR_TIMEOUT;
}

// Funcion generica para escribir registro de 16 bits
esp_err_t i2c_write_reg_16(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t reg, uint16_t value)
{
    if (i2c_mutex == NULL) 
    {
        return ESP_ERR_INVALID_STATE;
    }

    // Se bloquea el bus I2C, si no está disponible espera indefinidamente
    if (xSemaphoreTake(i2c_mutex, portMAX_DELAY))
    {
        // Se prepara el comando de I2C para indicar registro y dispositivo a leer
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        if (!cmd)
        {
            // Liberar el bus si falla
            xSemaphoreGive(i2c_mutex);
            return ESP_ERR_NO_MEM;
        } 

        i2c_master_start(cmd);
     
        // Dirección del dispositivo + bit de escritura
        i2c_master_write_byte(cmd, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
        // Registro del dispositivo
        i2c_master_write_byte(cmd, reg, true);
        
        // Escribir MSB primero (Big Endian)
        i2c_master_write_byte(cmd, (uint8_t)(value >> 8), true);
        i2c_master_write_byte(cmd, (uint8_t)(value & 0xFF), true);
        
        i2c_master_stop(cmd);

        // Ejecutar la transacción
        esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
        // Se libera memoria, borrando el comando
        i2c_cmd_link_delete(cmd);
        // Liberar el bus
        xSemaphoreGive(i2c_mutex);

        return ret;
    }

    return ESP_ERR_TIMEOUT;
}

// Funcion generica para leer registro de 16 bits
esp_err_t i2c_read_reg_16(i2c_port_t i2c_num, uint8_t i2c_addr, uint8_t reg, uint16_t *value)
{
    if (i2c_mutex == NULL) 
    {
        return ESP_ERR_INVALID_STATE;
    }

    // Se bloquea el bus I2C, si no está disponible espera indefinidamente
    if (xSemaphoreTake(i2c_mutex, portMAX_DELAY))
    {
        uint8_t data[2];

        // Primero: apuntar al registro que queremos leer
        i2c_cmd_handle_t cmd = i2c_cmd_link_create();
        if (!cmd)
        {
            // Liberar el bus si falla
            xSemaphoreGive(i2c_mutex);
            return ESP_ERR_NO_MEM;
        } 

        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i2c_addr << 1) | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, reg, true);
        i2c_master_stop(cmd);
        
        esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);
        if(ret != ESP_OK)
        {
            // Liberar el bus si falla
            xSemaphoreGive(i2c_mutex);
            return ret;
        }

        // Segundo: leer los 2 bytes
        cmd = i2c_cmd_link_create();
        if (!cmd)
        {
            // Liberar el bus si falla
            xSemaphoreGive(i2c_mutex);
            return ESP_ERR_NO_MEM;
        } 

        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, (i2c_addr << 1) | I2C_MASTER_READ, true);
        i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK); // Leemos los 2 bytes de golpe
        i2c_master_stop(cmd);

        ret = i2c_master_cmd_begin(i2c_num, cmd, I2C_TIMEOUT_MS / portTICK_PERIOD_MS);
        i2c_cmd_link_delete(cmd);

        if(ret == ESP_OK) {
            // Reconstruimos el valor de 16 bits (Big Endian)
            *value = (data[0] << 8) | data[1];
        }
        // Liberar el bus
        xSemaphoreGive(i2c_mutex);

        return ret;
    }

    return ESP_ERR_TIMEOUT;
}

esp_err_t i2c_init(uint8_t i2c_controller, uint32_t i2c_freq)
{

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_GPIO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_SCL_GPIO,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = i2c_freq,
    };

    // Se configuran los parámetros del I2C
    i2c_param_config(i2c_controller, &conf);

    // Se instala el driver I2C
    esp_err_t ret = i2c_driver_install(i2c_controller, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE("I2C", "Error instalando driver I2C: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI("I2C", "Driver I2C instalado correctamente");
    }
        i2c_mutex = xSemaphoreCreateMutex();
    if (i2c_mutex == NULL) {
        return ESP_ERR_NO_MEM;
    }
    return ESP_OK;
}

esp_err_t spi_bus_init(spi_host_device_t spi_host)
{
    esp_err_t ret;

    spi_bus_config_t buscfg = {
        .mosi_io_num = MOSI,
        .miso_io_num = MISO,
        .sclk_io_num = SCK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 4096,
    };

    // Inicializa el bus SPI
    ret = spi_bus_initialize(spi_host, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE("SPI", "Error inicializando bus SPI: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI("SPI", "Bus SPI inicializado correctamente");

    // Mutex opcional (como en tu I2C)
    spi_mutex = xSemaphoreCreateMutex();
    if (spi_mutex == NULL) {
        return ESP_ERR_NO_MEM;
    }

    return ESP_OK;
}