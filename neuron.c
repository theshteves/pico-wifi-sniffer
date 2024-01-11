#include <math.h>
#include <stdio.h>

#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"


/*
 * Empirical fit to an exponential curve
 * (e.g. {80, 2000} {89, 1000} {95, 400} {100, 200})
 * for a visually-intuitive temperature
 */
int delay_ms_adjusted_by_temperature_c(float temperature_c) {
  double exponent = pow(2.7, (-0.096 * temperature_c));
  long int delay_ms = 4096000 * exponent;
  //TODO: math.clamp floor & ceiling?
  return delay_ms;
}


/*
 * Blink LED at a frequency relative to on-board temperature
 */
void blink_temperature() {
  adc_init();
  adc_gpio_init(26);
  adc_select_input(4); // on-board temperature sensor

  while (true) {
    uint16_t result = adc_read();
    float voltage = result * 3.3f / (1 << 12);
    float temperature_c = 27 - (voltage - 0.706f) / 0.001721f;

    int delay_ms = delay_ms_adjusted_by_temperature_c(temperature_c);
    printf("temp(C): %.2f  =>  delay(ms): %dms\n", temperature_c, delay_ms);

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); // on-board LED
    sleep_ms(delay_ms);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(delay_ms);
  }
}


/*
 * "Inspired by" pico-examples/pico_w/wifi/wifi_scan/picow_wifi_scan.c
 */
static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result) {
        printf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
            result->ssid, result->rssi, result->channel,
            result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
            result->auth_mode);
    }
    return 0;
}


int main(int argv, const char *argc[]) {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("[ERROR: Wi-Fi init failed]");
    return 1;
  }

  multicore_launch_core1(blink_temperature);
  cyw43_arch_enable_sta_mode();

  absolute_time_t scan_time = nil_time;
  bool scan_in_progress = false;
  while(true) {
      if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {
          if (!scan_in_progress) {
              cyw43_wifi_scan_options_t scan_options = {0};
              int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
              if (err == 0) {
                  printf("\nPerforming wifi scan\n");
                  scan_in_progress = true;
              } else {
                  printf("Failed to start scan: %d\n", err);
                  scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
              }
          } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
              scan_time = make_timeout_time_ms(10000); // wait 10s and scan again
              scan_in_progress = false; 
          }
      }
      
      printf("` tick");
      sleep_ms(1000);
  }

  cyw43_arch_deinit();
  return 0;
}
