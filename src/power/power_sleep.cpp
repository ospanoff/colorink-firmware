#include "power/power_sleep.h"
#include "battery/battery.h"

#include "esp_sleep.h"

void enterDeepSleepWakeOnRtcMicros(uint64_t duration_micros) {
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
  esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_EXT1);
  esp_sleep_enable_timer_wakeup(duration_micros);

  batteryPinsIdleForSleep();

  esp_deep_sleep_start();
}
