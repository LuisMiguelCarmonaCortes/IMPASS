#include "motores.h"
#include "driver/mcpwm_prelude.h"
#include "esp_log.h"
#include "driver/gpio.h"


static const char *TAG = "MOTORES";

motor_mcpwm_obj_t motor[6];

motores_Status_t motores_init(uint32_t resolution, uint32_t period)
{

    // Se inicializan los pines de los solenoides
    gpio_reset_pin(SOL1);
    gpio_reset_pin(SOL2);
    gpio_reset_pin(SOL3);
    gpio_set_direction(SOL1, GPIO_MODE_OUTPUT);
    gpio_set_direction(SOL2, GPIO_MODE_OUTPUT);
    gpio_set_direction(SOL3, GPIO_MODE_OUTPUT);


    // Se configura el timer de los motores
    ESP_LOGI(TAG, "Inicializando MCPWM para 6 bobinas");
    mcpwm_timer_handle_t timer = NULL;
    mcpwm_timer_config_t timer_config = {
        .group_id = 0,
        .intr_priority = 0,
        .clk_src = MCPWM_TIMER_CLK_SRC_DEFAULT,
        .resolution_hz = resolution,
        .period_ticks = period,
        .count_mode = MCPWM_TIMER_COUNT_MODE_UP,
    };

    if(mcpwm_new_timer(&timer_config, &timer) != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al instanciar el timer de las bobinas");
        return MOTORES_ERROR;
    }

    int dir [] = {EJE1_DIR1, EJE1_DIR2, EJE2_DIR3, EJE2_DIR4, EJE3_DIR5, EJE3_DIR6};
    int pwm [] = {EJE1_PWM1, EJE1_PWM2, EJE2_PWM3, EJE2_PWM4, EJE3_PWM5, EJE3_PWM6,};

    // Se configuran los pines y lo necesario de cada motor
    for (int i = 0; i < 6; i++) {
        // Se configura el pin de dirección
        gpio_reset_pin(dir[i]);
        gpio_set_direction(dir[i], GPIO_MODE_OUTPUT);
        motor[i].pin_dir = dir[i];
        motor[i].pin_pwm = pwm[i];

        // Se crear operador
        mcpwm_oper_handle_t oper = NULL;
        mcpwm_operator_config_t operator_config = { .group_id = 0 };

        if(mcpwm_new_operator(&operator_config, &oper) != ESP_OK)
        {
            ESP_LOGE(TAG, "Error al instanciar el operador de la bobina numero %d", i);
            return MOTORES_ERROR;
        }

        if(mcpwm_operator_connect_timer(oper, timer) != ESP_OK)
        {
            ESP_LOGE(TAG, "Error al conectar el operador de la bobina numero %d", i);
            return MOTORES_ERROR;
        }

        // Se crea el comparador para cpntrolar el ancho de pulso
        mcpwm_comparator_config_t compare_config = { .flags.update_cmp_on_tez = true };
        if(mcpwm_new_comparator(oper, &compare_config, &motor[i].comparator) != ESP_OK)
        {
            ESP_LOGE(TAG, "Error al crear el comparador de la bobina numero %d", i);
            return MOTORES_ERROR;
        }

        // Se crea el generador y asociarlo al pin PWM
        mcpwm_gen_handle_t generator = NULL;
        mcpwm_generator_config_t generator_config = { .gen_gpio_num = pwm[i] };
        if(mcpwm_new_generator(oper, &generator_config, &generator) != ESP_OK)
        {
            ESP_LOGE(TAG, "Error al crear el generador y asociarlo de la bobina numero %d", i);
            return MOTORES_ERROR;
        }

        // Configurar acciones del generador: Subir al llegar a 0, bajar al coincidir con comparador
        if(mcpwm_generator_set_action_on_timer_event(generator, MCPWM_GEN_TIMER_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, MCPWM_TIMER_EVENT_EMPTY, MCPWM_GEN_ACTION_HIGH)) != ESP_OK)
        {
            ESP_LOGE(TAG, "Error al setear evento de la bobina numero %d", i);
            return MOTORES_ERROR;
        }

        if(mcpwm_generator_set_action_on_compare_event(generator, MCPWM_GEN_COMPARE_EVENT_ACTION(MCPWM_TIMER_DIRECTION_UP, motor[i].comparator, MCPWM_GEN_ACTION_LOW)) != ESP_OK)
        {
            ESP_LOGE(TAG, "Error al setear evento de la bobina numero %d", i);
            return MOTORES_ERROR;
        }

        // Se inicializar a 0% duty
        if(mcpwm_comparator_set_compare_value(motor[i].comparator, 0) != ESP_OK)
        {
            ESP_LOGE(TAG, "Error al inicializar dutycycle a 0% de la bobina numero %d", i);
            return MOTORES_ERROR;
        }
    }

    // Se arrancar el Timer
    if(mcpwm_timer_enable(timer) != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al habilitar el timer");
        return MOTORES_ERROR;
    }

    if(mcpwm_timer_start_stop(timer, MCPWM_TIMER_START_NO_STOP) != ESP_OK)
    {
        ESP_LOGE(TAG, "Error al arrancar el timer");
        return MOTORES_ERROR;
    }

    return MOTORES_OK;
}

motores_Status_t motores_avanza(int eje, uint8_t duty_cycle)
{
    //TODO
    return MOTORES_OK;
}

motores_Status_t motores_retrocede(int eje, uint8_t duty_cycle)
{
    // TODO
    return MOTORES_OK;
}

motores_Status_t motores_para(int eje)
{
    // TODO
    return MOTORES_OK;
}

motores_Status_t solenoide_bloquea(int solenoide)
{
    // TODO
    return MOTORES_OK;
}

motores_Status_t solenoide_desbloquea(int solenoide)
{
    // TODO
    return MOTORES_OK;
}