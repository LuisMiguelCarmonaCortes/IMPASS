#include <stdio.h>
#include <stdint.h>
#include "ads1015.h"
#include "driver/i2c.h"
#include "esp_log.h"
#include "utils.h"

// Debug tag
static const char *TAG = "ADS1015";

static ads1015_Status_t ads1015_WriteConfig(ads1015_t *dev)
{
    esp_err_t ret = i2c_write_reg_16(
        I2C_NUM_0,
        dev->i2c_addr,
        ADS1015_CONF_REG,
        dev->config_reg);

    if(ret != ESP_OK)
        return ADS1015_I2C_ERROR;

    return ADS1015_OK;
}

ads1015_Status_t ads1015_Init(ads1015_t *dev, uint8_t dir)
{
    uint16_t conf = 0;
    conf |= ADS1015_CANAL_0 | ADS1015_PGA_2_048V | ADS1015_DISPARO | ADS1015_1600_SPS | ADS1015_TRADICIONAL | ADS1015_NIVEL_BAJO | ADS1015_NO_FIJA | ADS1015_COMP_QUE_DISABLE;
    dev->i2c_addr = dir;
    dev->config_reg = conf;

    ads1015_Status_t status = ads1015_WriteConfig(dev);

    if (status != ADS1015_OK) 
    {
        ESP_LOGE(TAG, "Critical error writing initial configuration via I2C");
        return status;
    }

    ESP_LOGI(TAG, "ADS1015 initialized successfully.");
    return ADS1015_OK;
}

ads1015_Status_t ads1015_SetChannel(ads1015_t *dev, uint16_t ch)
{
    dev->config_reg &= ~ADS1015_MUX_MASK;
    dev->config_reg |= ch;
    return ads1015_WriteConfig(dev);
}

ads1015_Status_t ads1015_SetPGA(ads1015_t *dev, uint16_t pga)
{
    dev->config_reg &= ~ADS1015_PGA_MASK;
    dev->config_reg |= pga;
    return ads1015_WriteConfig(dev);
}

ads1015_Status_t ads1015_SetMode(ads1015_t *dev, uint16_t mode)
{
    dev->config_reg &= ~ADS1015_MODE_MASK;
    dev->config_reg |= mode;
    return ads1015_WriteConfig(dev);
}

ads1015_Status_t ads1015_SetSpeed(ads1015_t *dev, uint16_t speed)
{
    dev->config_reg &= ~ADS1015_DR_MASK;
    dev->config_reg |= speed;
    return ads1015_WriteConfig(dev);
}

ads1015_Status_t ads1015_SetCompMode(ads1015_t *dev, uint16_t mode)
{
    dev->config_reg &= ~ADS1015_COMP_MODE_MASK;
    dev->config_reg |= mode;
    return ads1015_WriteConfig(dev);
}

ads1015_Status_t ads1015_SetCompPol(ads1015_t *dev, uint16_t pol)
{
    dev->config_reg &= ~ADS1015_COMP_POL_MASK;
    dev->config_reg |= pol;
    return ads1015_WriteConfig(dev);
}


ads1015_Status_t ads1015_SetComLatch(ads1015_t *dev, uint16_t lat)
{
    dev->config_reg &= ~ADS1015_COMP_LAT_MASK;
    dev->config_reg |= lat;
    return ads1015_WriteConfig(dev);
}


ads1015_Status_t ads1015_SetCompQue(ads1015_t *dev, uint16_t que)
{
    dev->config_reg &= ~ADS1015_COMP_QUE_MASK;
    dev->config_reg |= que;
    return ads1015_WriteConfig(dev);
}

ads1015_Status_t ads1015_GetConfig(ads1015_t *dev)
{
    uint16_t conf;
    uint16_t low;
    uint16_t high;
    
    esp_err_t ret = i2c_read_reg_16(I2C_NUM_0, dev->i2c_addr, ADS1015_CONF_REG, &conf);

    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Error reading configuration register");
        if(ret == ESP_ERR_TIMEOUT)
        {
            ESP_LOGE(TAG, "Timeout");
            return ADS1015_I2C_ERROR;
        }
        if(ret == ESP_ERR_NO_MEM)
        {
            ESP_LOGE(TAG, "Failed to allocate memory for I2C command");
            return ADS1015_I2C_ERROR;
        } 
        return ADS1015_ERROR;
    }

    dev->config_reg = conf;

    ret = i2c_read_reg_16(I2C_NUM_0, dev->i2c_addr, ADS1015_LO_TRG_REG, &low);

    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read low-threshold register");
        if(ret == ESP_ERR_TIMEOUT)
        {
            ESP_LOGE(TAG, "Timeout");
            return ADS1015_I2C_ERROR;
        }
        if(ret == ESP_ERR_NO_MEM)
        {
            ESP_LOGE(TAG, "Failed to allocate memory for I2C command");
            return ADS1015_I2C_ERROR;
        } 
        return ADS1015_ERROR;
    }

    dev->lo_trg_reg = low;


    ret = i2c_read_reg_16(I2C_NUM_0, dev->i2c_addr, ADS1015_HI_TRG_REG, &high);

    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read high-threshold register");
        if(ret == ESP_ERR_TIMEOUT)
        {
            ESP_LOGE(TAG, "Timeout");
            return ADS1015_I2C_ERROR;
        }
        if(ret == ESP_ERR_NO_MEM)
        {
            ESP_LOGE(TAG, "Failed to allocate memory for I2C command");
            return ADS1015_I2C_ERROR;
        } 
        return ADS1015_ERROR;
    }

    dev->hi_trg_reg = high;

    // ADS1015 configuration
    uint8_t mux = (conf & ADS1015_MUX_MASK) >> 12;
    uint8_t pga = (conf & ADS1015_PGA_MASK) >> 9;
    uint8_t rate = (conf & ADS1015_DR_MASK) >> 5;
    uint8_t que = conf & ADS1015_COMP_QUE_MASK;

    ESP_LOGI(TAG, "Reading channel -> AIN%d", mux == 4 ? 0 : mux == 5 ? 1 : mux == 6 ? 2 : 3);
    ESP_LOGI(TAG, "Gain -> %s V",  pga == 0 ? "6.144" : pga == 1 ? "4.096" : pga == 2 ? "2.048" : pga == 3 ? "1.024": pga == 4 ? "0.512" : "256");
    ESP_LOGI(TAG, "Mode -> %s", (conf & ADS1015_DISPARO) ? "Single-shot" : "Continuous");
    ESP_LOGI(TAG, "Rate -> %s SPS", rate == 0 ? "128" : rate == 1 ? "250" : rate == 2 ? "490" : rate == 3 ? "920" : rate == 4 ? "1600" : rate == 5 ? "2400" : "3300");
    ESP_LOGI(TAG, "Comp mode -> %s", (conf & ADS1015_VENTANA) ? "Latching" : "Nonlatching");
    ESP_LOGI(TAG, "Comp que -> %s", que == 0 ? "one" : que == 1 ? "two" : que == 2 ? "four" : "Disabled");
    ESP_LOGI(TAG, "Low voltage alert -> %.2f", dev->lo_trg_reg);
    ESP_LOGI(TAG, "High voltage alert -> %.2f", dev->hi_trg_reg);

    return ADS1015_OK;
}

ads1015_Status_t ads1015_readADC(ads1015_t *dev, int16_t *adc)
{
    uint16_t raw;

    esp_err_t ret = i2c_read_reg_16(I2C_NUM_0, dev->i2c_addr, ADS1015_CONV_REG, &raw);
    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read hysteresis register");
        if(ret == ESP_ERR_TIMEOUT)
        {
            ESP_LOGE(TAG, "Timeout");
            return ADS1015_I2C_ERROR;
        }
        if(ret == ESP_ERR_NO_MEM)
        {
            ESP_LOGE(TAG, "Failed to allocate memory for I2C command");
            return ADS1015_I2C_ERROR;
        } 
        return ADS1015_ERROR;
    }

    *adc = ((int16_t)raw) >> 4;
    dev->conver_reg = *adc;

    ESP_LOGI(TAG, "Raw ADC -> 0x%04X", raw);

    return ADS1015_OK;
}

ads1015_Status_t ads1015_convertRead(ads1015_t *dev, float *volt)
{
    int16_t raw;
    esp_err_t ret = ads1015_readADC(dev, &raw);

    if(ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to read conversion register");
        if(ret == ADS1015_I2C_ERROR)
        {
            ESP_LOGE(TAG, "Timeout");
            return ADS1015_I2C_ERROR;
        }
        if(ret == ESP_ERR_NO_MEM)
        {
            ESP_LOGE(TAG, "Failed to allocate memory for I2C command");
            return ADS1015_I2C_ERROR;
        } 
        return ADS1015_ERROR;
    }

    // Read the PGA value to calculate the voltage
    
    float fsr;
    uint8_t pga = (dev->config_reg & ADS1015_PGA_MASK) >> 9;

    switch(pga)
    {
    case 0: fsr = 6.144f; break;
    case 1: fsr = 4.096f; break;
    case 2: fsr = 2.048f; break;
    case 3: fsr = 1.024f; break;
    case 4: fsr = 0.512f; break;
    case 5: fsr = 0.256f; break;
    default:
        return ADS1015_ERROR;
    }

    *volt = fsr * raw / 2048.0f;

    ESP_LOGI(TAG, "Voltage -> %.2f V", *volt);

    return ADS1015_OK;
}

ads1015_Status_t ads1015_SetLo(ads1015_t *dev, float volt)
{
    esp_err_t ret = i2c_write_reg_16(I2C_NUM_0, dev->i2c_addr, ADS1015_LO_TRG_REG, volt);

    if (ret == ESP_OK) 
    {
        ESP_LOGI(TAG, "16-bit register (0x%02X) written", ADS1015_LO_TRG_REG);
        return ADS1015_OK;
    } 
    else if (ret == ESP_ERR_NO_MEM) 
    {
        ESP_LOGE(TAG, "Failed to allocate memory for I2C command");
        return ADS1015_I2C_ERROR;
    } 
    else 
    {
        ESP_LOGE(TAG, "Failed to send I2C command");
        if (ret == ESP_ERR_TIMEOUT) 
        {
            ESP_LOGE(TAG, "Timeout");;
        }
        return ADS1015_I2C_ERROR;
    }
}

ads1015_Status_t ads1015_SetHi(ads1015_t *dev, float volt)
{
    esp_err_t ret = i2c_write_reg_16(I2C_NUM_0, dev->i2c_addr, ADS1015_HI_TRG_REG, volt);

    if (ret == ESP_OK) 
    {
        ESP_LOGI(TAG, "16-bit register (0x%02X) written", ADS1015_HI_TRG_REG);
        return ADS1015_OK;
    } 
    else if (ret == ESP_ERR_NO_MEM) 
    {
        ESP_LOGE(TAG, "Failed to allocate memory for I2C command");
        return ADS1015_I2C_ERROR;
    } 
    else 
    {
        ESP_LOGE(TAG, "Failed to send I2C command");
        if (ret == ESP_ERR_TIMEOUT) 
        {
            ESP_LOGE(TAG, "Timeout");;
        }
        return ADS1015_I2C_ERROR;
    }
}
