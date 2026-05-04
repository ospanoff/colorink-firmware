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
#include "colorink_app/colorink_app_http.h"
#include "config/app_config.h"
#include "log/wake_log.h"
#include "net/wifi_station.h"
#include "panel/display_bmp.h"
#include "power/power_sleep.h"
#include "ui/wake_screen.h"

#include <Arduino.h>

#include <TFT_eSPI.h>
#include <WiFi.h>
#include <esp_heap_caps.h>
#include <esp_sleep.h>

EPaper epaper;

static void wipeWifiRadio() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void setup() {
  Serial.begin(115200);
  delay(500); // USB CDC often needs a moment before the first prints appear

  const esp_sleep_wakeup_cause_t wakeup = esp_sleep_get_wakeup_cause();
  const bool wakeup_from_rtc_timer = (wakeup == ESP_SLEEP_WAKEUP_TIMER);
  const int32_t battery_mv = measureBatteryMilliVolts();
  const int battery_percent =
      approximateBatteryPercentNominal3700Mv(battery_mv);

  Serial.print("Wake: ");
  printWakeBatteryToSerial(wakeup_from_rtc_timer, battery_mv, battery_percent);
  Serial.flush();

  uint8_t *bmpRam = nullptr;
  size_t bmpLen = 0;

  if (connectWifiStation()) {
    const bool post_ok = colorinkAppPostForceUpdate();
    const bool fetched =
        post_ok && colorinkAppDownloadImageBmpToPsram(&bmpRam, &bmpLen);
    if (!fetched) {
      bmpRam = nullptr;
      bmpLen = 0;
    }
    wipeWifiRadio();
  }

  Serial.println("epaper: begin…");
  Serial.flush();
  epaper.begin();
  Serial.println("epaper: begin done");
  Serial.flush();

  const bool bmpShown = displayBmp(epaper, bmpRam, bmpLen);
  if (!bmpShown) {
    epaper.fillScreen(TFT_WHITE);
    drawWakeBatteryScreen(epaper, wakeup_from_rtc_timer, battery_mv,
                          battery_percent);
  }

  if (bmpRam != nullptr) {
    heap_caps_free(bmpRam);
  }

  enterDeepSleepWakeOnRtcMicros(kSleepAfterRefreshUs);
}

void loop() {
  // Deep sleep does not return; loop is unused.
}
