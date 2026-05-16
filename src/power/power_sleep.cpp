#include "power/power_sleep.h"
#include "battery/battery.h"
#include "board/user_led.h"

#include <Arduino.h>

#include "esp_sleep.h"

void enterDeepSleepWakeOnRtcMicros(uint64_t duration_micros) {
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_EXT1);
  const esp_err_t err = esp_sleep_enable_timer_wakeup(duration_micros);
  if (err != ESP_OK) {
    Serial.printf("power_sleep: esp_sleep_enable_timer_wakeup err=%d us=%llu\n",
                  static_cast<int>(err),
                  static_cast<unsigned long long>(duration_micros));
    Serial.flush();
  }

  batteryPinsIdleForSleep();

  Serial.printf("power_sleep: entering deep sleep (%llu us)\n",
                static_cast<unsigned long long>(duration_micros));
  Serial.flush();
  userLedSleepPrepare();
  esp_deep_sleep_start();
}
