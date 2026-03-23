#ifndef UNIT_TEST

#include <stdio.h>
#include <unity.h>
#include "lm75.h"
#include "definitions.h"
#include "driver/i2c.h"

/* Funciones obligatorias de Unity */
void setUp(void) {
    
}

void tearDown(void) {

}

/* --- Funciones de Test --- */

void test_lm75_init_and_read(void) {
    lm75_t sensor;
    
    // Inicialización
    lm75_Status_t status = lm75_Init(&sensor, LM75_LOGIC_ADDR);
    TEST_ASSERT_EQUAL(LM75_OK, status);

    // Lectura de temperatura Celsius
    float temp;
    status = lm75_read_celsius_temp(&sensor, &temp);
    TEST_ASSERT_EQUAL(LM75_OK, status);
    
    // Validación de rango razonable (evitar ruidos extremos)
    TEST_ASSERT_TRUE(temp > -40.0f && temp < 125.0f);
}

void test_lm75_fault_queue(void) {
    lm75_t sensor;
    lm75_Init(&sensor, LM75_LOGIC_ADDR);
    
    TEST_ASSERT_EQUAL(LM75_OK, lm75_SetFaultQueue(&sensor, 2));
    TEST_ASSERT_EQUAL(LM75_INVALID_PARAM, lm75_SetFaultQueue(&sensor, 5));
}

void test_lm75_polarity(void) {
    lm75_t sensor;
    lm75_Init(&sensor, LM75_LOGIC_ADDR);
    
    TEST_ASSERT_EQUAL(LM75_OK, lm75_SetPolarity(&sensor, 1));
    TEST_ASSERT_EQUAL(LM75_INVALID_PARAM, lm75_SetPolarity(&sensor, 5));
}

void test_lm75_mode(void) {
    lm75_t sensor;
    lm75_Init(&sensor, LM75_LOGIC_ADDR);
    
    TEST_ASSERT_EQUAL(LM75_OK, lm75_SetMode(&sensor, 1));
    TEST_ASSERT_EQUAL(LM75_INVALID_PARAM, lm75_SetMode(&sensor, 5));
}

void test_lm75_shutdown(void) {
    lm75_t sensor;
    lm75_Init(&sensor, LM75_LOGIC_ADDR);
    
    TEST_ASSERT_EQUAL(LM75_OK, lm75_SetShutdown(&sensor, 1));
    TEST_ASSERT_EQUAL(LM75_INVALID_PARAM, lm75_SetShutdown(&sensor, 5));
}

void test_lm75_config_and_limits(void) {
    lm75_t sensor;
    lm75_Init(&sensor, LM75_LOGIC_ADDR);

    TEST_ASSERT_EQUAL(LM75_OK, lm75_GetConfig(&sensor));
    TEST_ASSERT_EQUAL(LM75_OK, lm75_SetTos(&sensor, 90.0f));
    TEST_ASSERT_EQUAL(LM75_OK, lm75_SetThyst(&sensor, 85.0f));
    TEST_ASSERT_EQUAL(LM75_OK, lm75_GetConfig(&sensor));
}

/* --- Punto de entrada para PlatformIO --- */
void app_main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_lm75_init_and_read);
    RUN_TEST(test_lm75_fault_queue);
    RUN_TEST(test_lm75_polarity);
    RUN_TEST(test_lm75_mode);
    RUN_TEST(test_lm75_shutdown);
    RUN_TEST(test_lm75_config_and_limits);

    UNITY_END();
}

#endif