#include <stdio.h>
#include <string.h>

#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "pico/stdlib.h"

#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15

#define BUTTON_A_PIN 5

#define RED_LED_PIN 13
#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12

bool button_A_is_pressed = false;

enum Color {
    RED,
    GREEN,
    YELLOW
};

char *GREEN_TRAFFIC_LIGHT_TEXT[] = {
    "               ",
    "               ",
    " SINAL ABERTO  ",
    "               ",
    " ATRAVESSE COM ",
    "    CUIDADO    ",
    "               ",
    "               "};

char *YELLOW_TRAFFIC_LIGHT_TEXT[] = {
    "               ",
    "               ",
    "   SINAL DE    ",
    "    ATENCAO    ",
    "               ",
    "  PREPARE SE   ",
    "               ",
    "               "};

char *RED_TRAFFIC_LIGHT_TEXT[] = {
    "               ",
    "               ",
    " SINAL FECHADO ",
    "               ",
    "    AGUARDE    ",
    "               ",
    "               ",
    "               "};

void display_text(uint8_t *ssd, struct render_area *frame_area, char *text[], size_t line_count) {
    memset(ssd, 0, ssd1306_buffer_length);
    int y = 0;
    for (size_t i = 0; i < line_count; i++) {
        ssd1306_draw_string(ssd, 5, y, text[i]);
        y += 8;
    }
    render_on_display(ssd, frame_area);
}

bool wait_for_timeout_or_button_press(int time_ms) {
    for (int i = 0; i < time_ms; i++) {
        button_A_is_pressed = !gpio_get(BUTTON_A_PIN);
        if (button_A_is_pressed) {
            return true;
        }
        sleep_ms(1);
    }
    return false;
}

void set_leds(bool red, bool green, bool blue) {
    gpio_put(RED_LED_PIN, red);
    gpio_put(GREEN_LED_PIN, green);
    gpio_put(BLUE_LED_PIN, blue);
}

void turn_on_red_traffic_light() {
    set_leds(true, false, false);
}

void turn_on_green_traffic_light() {
    set_leds(false, true, false);
}

void turn_on_yellow_traffic_light() {
    set_leds(true, true, false);
}

void set_traffic_light(enum Color color, uint8_t *ssd, struct render_area *frame_area) {
    if (color == RED) {
        turn_on_red_traffic_light();
        display_text(ssd, frame_area, RED_TRAFFIC_LIGHT_TEXT, count_of(RED_TRAFFIC_LIGHT_TEXT));
    } else if (color == GREEN) {
        turn_on_green_traffic_light();
        display_text(ssd, frame_area, GREEN_TRAFFIC_LIGHT_TEXT, count_of(GREEN_TRAFFIC_LIGHT_TEXT));
    } else {
        turn_on_yellow_traffic_light();
        display_text(ssd, frame_area, YELLOW_TRAFFIC_LIGHT_TEXT, count_of(YELLOW_TRAFFIC_LIGHT_TEXT));
    }
}

void setup() {
    stdio_init_all();

    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(RED_LED_PIN);
    gpio_init(GREEN_LED_PIN);
    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);

    i2c_init(I2C_PORT, ssd1306_i2c_clock * 400);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init();
}

int main() {
    setup();

    set_leds(false, false, false);

    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];

    enum Color R = RED, G = GREEN, Y = YELLOW;

    int yellow_traffic_light_time, green_traffic_light_time;

    while (true) {
        yellow_traffic_light_time = 2 * 1000;
        green_traffic_light_time = 8 * 1000;

        set_traffic_light(R, ssd, &frame_area);
        button_A_is_pressed = wait_for_timeout_or_button_press(8 * 1000);

        if (button_A_is_pressed) {
            yellow_traffic_light_time = 5 * 1000;
            green_traffic_light_time = 10 * 1000;
        }

        set_traffic_light(Y, ssd, &frame_area);
        sleep_ms(yellow_traffic_light_time);
        set_traffic_light(G, ssd, &frame_area);
        sleep_ms(green_traffic_light_time);
    }

    return 0;
}
