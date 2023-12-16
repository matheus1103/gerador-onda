#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <math.h>
#include <string.h>

// Definições para a UART
#define UART_NUM UART_NUM_0
#define BUF_SIZE (1024)

// Definições para GPIOs
// Definindo os GPIOs para os pinos D0 a D7
#define GPIO_D0 GPIO_NUM_16
#define GPIO_D1 GPIO_NUM_5
#define GPIO_D2 GPIO_NUM_4
#define GPIO_D3 GPIO_NUM_0
#define GPIO_D4 GPIO_NUM_2
#define GPIO_D5 GPIO_NUM_14
#define GPIO_D6 GPIO_NUM_12
#define GPIO_D7 GPIO_NUM_13


typedef enum {
    STATE_IDLE,
    STATE_SINE_WAVE,
    STATE_SQUARE_WAVE,
    STATE_SAWTOOTH_WAVE,
    STATE_TRIANGLE_WAVE

} WaveState;

WaveState state = STATE_IDLE;
// Função para gerar onda senoidal

void set_dac_value(uint8_t value) {
    gpio_set_level(GPIO_D0, value & 0x01);
    gpio_set_level(GPIO_D1, value & 0x02);
    gpio_set_level(GPIO_D2, value & 0x04);
    gpio_set_level(GPIO_D3, value & 0x08);
    gpio_set_level(GPIO_D4, value & 0x10);
    gpio_set_level(GPIO_D5, value & 0x20);
    gpio_set_level(GPIO_D6, value & 0x40);
    gpio_set_level(GPIO_D7, value & 0x80);
}
void generate_sine_wave() {
    const int max_value = 255; // 8 bits
    static float angle = 0.0;  // 'static' para manter o estado entre chamadas
    const float two_pi = 2.0 * M_PI;    
    const float step = two_pi / 360;

    // Calcula o valor para simular a onda senoidal
    int dac_value = (int)((max_value / 2) * (sin(angle) + 1));
    set_dac_value(dac_value);

    angle += step;
    if (angle >= two_pi) {
        angle = 0;
    }

    vTaskDelay(1 / portTICK_PERIOD_MS); 
}
void generate_sawtooth_wave() {
    static int value = 0;
    const int max_value = 255; // 8 bits
    const int step = 1; // Ajuste o step para controlar a velocidade da onda

    set_dac_value(value);

    value += step;
    if (value > max_value) {
        value = 0;
    }

    vTaskDelay(1 / portTICK_PERIOD_MS); // Ajuste para controlar a frequência
}
void generate_triangle_wave() {
    static int value = 0;
    static bool increasing = true;
    const int max_value = 255; // 8 bits
    const int step = 1; // Ajuste o step para controlar a velocidade da onda

    set_dac_value(value);

    if (increasing) {
        value += step;
        if (value >= max_value) {
            increasing = false;
        }
    } else {
        value -= step;
        if (value <= 0) {
            increasing = true;
        }
    }

    vTaskDelay(1 / portTICK_PERIOD_MS); // Ajuste para controlar a frequência
}

void generate_square_wave() {
    static bool high = false;
 static int value = 0;
    int dac_value = high ? 255 : 0; // 3.3V ou 0V
    set_dac_value(dac_value);
     value++;
     if (value > 127)
     {
        high = !high;
        value = 0;
     }

    vTaskDelay(1 / portTICK_PERIOD_MS); 
}

void gpio_setup() {
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<GPIO_D0) | (1ULL<<GPIO_D1) | (1ULL<<GPIO_D2) |
                           (1ULL<<GPIO_D3) | (1ULL<<GPIO_D4) | (1ULL<<GPIO_D5) |
                           (1ULL<<GPIO_D6) | (1ULL<<GPIO_D7);
    io_conf.pull_up_en = 0;
    io_conf.pull_down_en = 0;
    gpio_config(&io_conf);
}

void uart_setup() {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);
}


void send_message_to_cutecom(const char* message) {
    uart_write_bytes(UART_NUM, message, strlen(message));
}

void app_main() {
    gpio_setup();
    uart_setup();
    send_message_to_cutecom(" ######## Gerador de Ondas #######\n Selecione: \n -- seno: s\n -- quadrada: q\n -- serra: w\n -- triangular: t\n");

    char data[BUF_SIZE];
    bool generate_sine = false;

       while (1) {
        // Leitura dos dados da UART
        int length = uart_read_bytes(UART_NUM, (uint8_t*)data, BUF_SIZE - 1, 20 / portTICK_RATE_MS);
        if (length > 0) {
            data[length] = 0; // Null-terminate the string
            switch (data[0]) {
                case 's':
                    send_message_to_cutecom("Generating Sine Wave\n");
                    state = STATE_SINE_WAVE;
                    break;
                case 'q':
                    send_message_to_cutecom("Generating Square Wave\n");
                    state = STATE_SQUARE_WAVE;
                    break;
                case 'w': // Por exemplo, 'w' para onda serrilhada
                    send_message_to_cutecom("Generating Sawtooth Wave\n");
                    state = STATE_SAWTOOTH_WAVE;
                    break;
                case 't': // Por exemplo, 't' para onda triangular
                    send_message_to_cutecom("Generating Triangle Wave\n");
                    state = STATE_TRIANGLE_WAVE;
                    break;
                default:
                    send_message_to_cutecom("Unknown command\n");
            }
        }

        // Geração das ondas baseada no estado
        switch (state) {
             case STATE_SINE_WAVE:
                generate_sine_wave();
                break;
            case STATE_SQUARE_WAVE:
                generate_square_wave();
                break;
            case STATE_SAWTOOTH_WAVE:
                generate_sawtooth_wave();
                break;
            case STATE_TRIANGLE_WAVE:
                generate_triangle_wave();
                break;
            default:
                // Estado ocioso ou desconhecido
                vTaskDelay(100 / portTICK_PERIOD_MS); // Pequeno delay para evitar sobrecarga da CPU
                break;
        }
    }
}