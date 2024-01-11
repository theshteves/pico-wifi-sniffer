#include <math.h>
#include <stdio.h>
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
#include "pico/multicore.h"
#include "pico/stdlib.h"


/*
 * Empirical fit to an exponential curve (e.g. {80, 2000} {89, 1000} {95, 400} {100, 200}) for a visually-intuitive temperature
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


int main(int argv, const char *argc[]) {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("[ERROR: Wi-Fi init failed]");
    return -1;
  }

  multicore_launch_core1(blink_temperature);

  while (true) {
    printf("` tick");
    sleep_ms(1000);
  }
  return 0;
}
