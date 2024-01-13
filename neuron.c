#include <math.h>
#include <stdio.h>
//TODO: optimize printf's into write's?

#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"


static int16_t BEST_WIFI_RSSI = -110; // No signal
static int16_t NEURON_ACTIVATION_THRESHOLD = -30;


/*
 * "Inspired by" pico-examples/pico_w/wifi/wifi_scan/picow_wifi_scan.c
 */
static int scan_result(void *env, const cyw43_ev_scan_result_t *result) {
    if (result && result->rssi > BEST_WIFI_RSSI) {
        BEST_WIFI_RSSI = result->rssi;

        printf("\n==> %4d  %-32s chan: %3d sec: %u mac: %02x:%02x:%02x:%02x:%02x:%02x",
            result->rssi, result->ssid, result->channel, result->auth_mode,
            result->bssid[0], result->bssid[1], result->bssid[2], // OUI
            result->bssid[3], result->bssid[4], result->bssid[5]);
    }
    return 0;
}


/*
 * Empirical fit to a line
 * (e.g. {-30, 50}, {-35, 100}, {-40, 150}, ..., {-75, 500})
 * for a visually-intuitive best Wi-Fi RSSI
 */
int delay_ms_adjusted_by_best_wifi_rssi(int16_t best_wifi_rssi) {
  //TODO: math.clamp floor & ceiling?
  //return -10 * (int)best_wifi_rssi - 250;
  //return MAX(15, -15 * (int)best_wifi_rssi - 350);
  //return MAX(15, -30 * (int)best_wifi_rssi - 780);
  return MAX(15, -10 * (int)best_wifi_rssi - 150);
  // sub-15ms not noticeable to many humans
}


/*
 * Blink LED at a frequency relative to best Wi-Fi RSSI
 */
void blink_best_wifi_rssi() {
  while (true) {
    int delay_ms = delay_ms_adjusted_by_best_wifi_rssi(BEST_WIFI_RSSI);
    //printf("\n* blink(%d) = %dms", BEST_WIFI_RSSI, delay_ms);

    printf("+");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1); // on-board LED
    sleep_ms(delay_ms);

    printf("-");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(delay_ms);
  }
}


int main(int argv, const char *argc[]) {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("[ERROR: Wi-Fi init failed]");
    return 1;
  }

  multicore_launch_core1(blink_best_wifi_rssi);
  cyw43_arch_enable_sta_mode();

  absolute_time_t scan_time = nil_time;
  bool scan_in_progress = false;
  while(true) {
      if (absolute_time_diff_us(get_absolute_time(), scan_time) < 0) {

          // Start scanning if not
          if (!scan_in_progress) {
              cyw43_wifi_scan_options_t scan_options = {0};

              // No? start a new one
              int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scan_result);
              if (err == 0) {
                  printf("\n___ ___ ___ ___ ___ ___ ___ ___\n");
                  scan_in_progress = true;

              } else {
                  printf("\n\n[ERROR: Failed to start scan: %d]\n\n", err);
                  scan_time = make_timeout_time_ms(800); //800?
              }

          // Did scan just stop?
          } else if (!cyw43_wifi_scan_active(&cyw43_state)) {
              scan_time = make_timeout_time_ms(500); // break between scans
              scan_in_progress = false; 
          }
      }
      
      printf("`");
      sleep_ms(100);
      BEST_WIFI_RSSI -= 1; // Forget best rssi reading over time (decay 10 per second)

      if (BEST_WIFI_RSSI > NEURON_ACTIVATION_THRESHOLD) {
          printf("<!>");
          //TODO: DO WHAT NEURONS ARE SUPPOSED TO DO!
      }
  }

  cyw43_arch_deinit();
  return 0;
}
