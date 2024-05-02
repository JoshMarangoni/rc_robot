#include <esp_event.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <stdio.h>
#include <sys/param.h>

#include "driver/gpio.h"
#include "esp_eth.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_tls_crypto.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "sdkconfig.h"


#define    LED_PIN       (2U)

// left motor
#define    MOTOR_PIN_M11 (25U)
#define    MOTOR_PIN_M12 (26U)

// right motor
#define    MOTOR_PIN_M21 (19U)
#define    MOTOR_PIN_M22 (18U)

#define    HIGH          (1U)
#define    LOW           (0U)

static const char *TAG = "INFO";

static uint8_t s_led_state = 0;

static void blink_led(void)
{
    gpio_set_level(LED_PIN, s_led_state);
}

static void configure_led(void)
{
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
    ESP_LOGI(TAG, "On board LED initialized");
}

static void configure_motor_pins(void)
{
    gpio_reset_pin(MOTOR_PIN_M11);
    gpio_set_direction(MOTOR_PIN_M11, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MOTOR_PIN_M12);
    gpio_set_direction(MOTOR_PIN_M12, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MOTOR_PIN_M21);
    gpio_set_direction(MOTOR_PIN_M21, GPIO_MODE_OUTPUT);

    gpio_reset_pin(MOTOR_PIN_M22);
    gpio_set_direction(MOTOR_PIN_M22, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "Motors initialized");
}

static void drive_straight()
{
    ESP_LOGI(TAG, "Going straight");
    gpio_set_level(MOTOR_PIN_M11, LOW);
    gpio_set_level(MOTOR_PIN_M12, HIGH);
    gpio_set_level(MOTOR_PIN_M21, LOW);
    gpio_set_level(MOTOR_PIN_M22, HIGH);
}

static void turn_left()
{
    ESP_LOGI(TAG, "Turning left");
    gpio_set_level(MOTOR_PIN_M11, LOW);
    gpio_set_level(MOTOR_PIN_M12, LOW);
    gpio_set_level(MOTOR_PIN_M21, LOW);
    gpio_set_level(MOTOR_PIN_M22, HIGH);
}

static void drive_backwards()
{
    ESP_LOGI(TAG, "Going backwards");
    gpio_set_level(MOTOR_PIN_M11, HIGH);
    gpio_set_level(MOTOR_PIN_M12, LOW);
    gpio_set_level(MOTOR_PIN_M21, HIGH);
    gpio_set_level(MOTOR_PIN_M22, LOW);
}

static void turn_right()
{
    ESP_LOGI(TAG, "Turning right");
    gpio_set_level(MOTOR_PIN_M11, LOW);
    gpio_set_level(MOTOR_PIN_M12, HIGH);
    gpio_set_level(MOTOR_PIN_M21, LOW);
    gpio_set_level(MOTOR_PIN_M22, LOW);
}

static void stop()
{
    // ESP_LOGI(TAG, "Stopping");
    gpio_set_level(MOTOR_PIN_M11, LOW);
    gpio_set_level(MOTOR_PIN_M12, LOW);
    gpio_set_level(MOTOR_PIN_M21, LOW);
    gpio_set_level(MOTOR_PIN_M22, LOW);
}


/* An HTTP GET handler */
static esp_err_t get_handler(httpd_req_t *req)
{
    esp_err_t ret;
    char* response_data = "OK";  // Default response data
    char* response_code = "200";     // Default HTTP status code

    if (strcmp(req->uri, "/blink") == 0)
    {
        s_led_state = !s_led_state;
        blink_led();
        response_data = "Blink toggled";
    }
    else if (strcmp(req->uri, "/drive/w") == 0)
    {
        drive_straight();
        response_data = "Driving straight";
    }
    else if (strcmp(req->uri, "/drive/a") == 0)
    {
        turn_left();
        response_data = "Turning left";
    }
    else if (strcmp(req->uri, "/drive/s") == 0)
    {
        drive_backwards();
        response_data = "Driving backwards";
    }
    else if (strcmp(req->uri, "/drive/d") == 0)
    {
        turn_right();
        response_data = "Turning right";
    }
    else if (strcmp(req->uri, "/drive/q") == 0)
    {
        stop();
        response_data = "Stopped";
    }
    else
    {
        response_data = "Unknown Request";
        response_code = "404";  // Not found status code
    }

    // Note: client should be setup to not rely on a response,
    // but it is helpful to have a response nonetheless.
    httpd_resp_set_status(req, response_code);
    ret = httpd_resp_send(req, response_data, HTTPD_RESP_USE_STRLEN);
    if (ret != ESP_OK)
    {
        ESP_LOGE("HTTPD", "Failed to send response: %s", esp_err_to_name(ret));
    }

    return ret;
}

static const httpd_uri_t hello = {
    .uri       = "/hello",
    .method    = HTTP_GET,
    .handler   = get_handler,
    /* Let's pass response string in user
     * context to demonstrate it's usage */
    .user_ctx  = "Hello World!"
};

static const httpd_uri_t blink = {
    .uri       = "/blink",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = "LED TOGGLED"
};

static const httpd_uri_t drive_w = {
    .uri       = "/drive/w",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = "DRIVING STRAIGHT"
};

static const httpd_uri_t drive_a = {
    .uri       = "/drive/a",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = "TURNING LEFT"
};

static const httpd_uri_t drive_s = {
    .uri       = "/drive/s",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = "DRIVING BACKWARDS"
};

static const httpd_uri_t drive_d = {
    .uri       = "/drive/d",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = "TURNING RIGHT"
};

static const httpd_uri_t drive_q = {
    .uri       = "/drive/q",
    .method    = HTTP_GET,
    .handler   = get_handler,
    .user_ctx  = "STOPPING"
};

static httpd_handle_t start_webserver(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &hello);
        httpd_register_uri_handler(server, &blink);
        httpd_register_uri_handler(server, &drive_w);
        httpd_register_uri_handler(server, &drive_a);
        httpd_register_uri_handler(server, &drive_s);
        httpd_register_uri_handler(server, &drive_d);
        httpd_register_uri_handler(server, &drive_q);
        return server;
    }

    ESP_LOGI(TAG, "Error starting server!");
    return NULL;
}

static esp_err_t stop_webserver(httpd_handle_t server)
{
    return httpd_stop(server);
}

static void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        if (stop_webserver(*server) == ESP_OK) {
            *server = NULL;
        } else {
            ESP_LOGE(TAG, "Failed to stop http server");
        }
    }
}

static void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "APP BEGIN");

    configure_led();
    configure_motor_pins();

    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));

    server = start_webserver();
}
