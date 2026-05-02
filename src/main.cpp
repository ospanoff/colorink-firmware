/*
 * Supported Colors:
 * - TFT_WHITE
 * - TFT_BLACK
 * - TFT_YELLOW
 * - TFT_GREEN
 * - TFT_BLUE
 * - TFT_RED
 */

#include "battery/battery.h"
#include "config/app_config.h"
#include "log/wake_log.h"
#include "net/wifi_station.h"
#include "power/power_sleep.h"
#include "ui/wake_screen.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "esp_sleep.h"

EPaper epaper;

void setup() {
  Serial.begin(115200);
  delay(500); // USB CDC often needs a moment before the first prints appear

  const esp_sleep_wakeup_cause_t wakeup = esp_sleep_get_wakeup_cause();
  const bool wakeup_from_rtc_timer =
      (wakeup == ESP_SLEEP_WAKEUP_TIMER);
  const int32_t battery_mv = measureBatteryMilliVolts();
  const int battery_percent =
      approximateBatteryPercentNominal3700Mv(battery_mv);

  // connectWifiStation();
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);

  epaper.begin();
  drawWakeBatteryScreen(epaper, wakeup_from_rtc_timer, battery_mv,
                        battery_percent);

  Serial.print("Wake: ");
  printWakeBatteryToSerial(wakeup_from_rtc_timer, battery_mv,
                           battery_percent);
  Serial.flush();

  enterDeepSleepWakeOnRtcMicros(kSleepAfterRefreshUs);
}

void loop() {
  // Deep sleep does not return; loop is unused.
}
