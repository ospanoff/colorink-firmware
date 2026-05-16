// Deep-sleep wake: WiFi → ColorInk BMP fetch → e-paper paint → RTC wake stub.

#include "battery/battery.h"
#include "board/user_led.h"
#include "colorink_app/colorink_app_http.h"
#include "config/app_config.h"
#include "log/wake_log.h"
#include "net/wifi_station.h"
#include "panel/display_bmp.h"
#include "panel/epaper_framebuffer_prepare.h"
#include "power/power_sleep.h"
#include "ui/wake_display_overlays.h"

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
  userLedWakeBegin();
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

  // boot_err: WiFi or HTTP→PSRAM; bmp_err: BMP decode / epaper paint (after
  // begin).
  BootDisplayError boot_err = BootDisplayError::None;
  uint8_t *bmpRam = nullptr;
  size_t bmpLen = 0;

  const BootDisplayError wifi_err = connectWifiStation();
  const bool wifi_connected = (wifi_err == BootDisplayError::None);
  if (!wifi_connected) {
    boot_err = wifi_err;
  } else {
    const BootDisplayError refresh_err =
        colorinkAppRefreshBmpToPsram(&bmpRam, &bmpLen);
    if (refresh_err != BootDisplayError::None) {
      boot_err = refresh_err;
    }
  }

  Serial.println("epaper: begin…");
  Serial.flush();
  epaper.begin();
  Serial.println("epaper: begin done");
  Serial.flush();

  prepareEpaperFramebuffer(epaper);

  BootDisplayError bmp_err = BootDisplayError::None;
  // Only paint when WiFi + fetch succeeded (boot_err == None). On failure, HTTP
  // clears bmpRam; this guard makes that contract obvious and avoids painting
  // if boot_err is ever set while a buffer exists.
  if (boot_err == BootDisplayError::None && bmpRam != nullptr && bmpLen > 0) {
    bmp_err = displayBmp(epaper, bmpRam, bmpLen);
  }

  const BootDisplayError overlay_err =
      (boot_err != BootDisplayError::None) ? boot_err : bmp_err;

  drawWakeErrorOverlayTopRight(epaper, overlay_err);
  drawWakeLowBatteryBottomRight(epaper, battery_percent);
  epaper.update();

  if (bmpRam != nullptr) {
    heap_caps_free(bmpRam);
  }

  if (wifi_connected) {
    colorinkAppPostBootWakeLog(wakeup_from_rtc_timer, battery_mv,
                               battery_percent, overlay_err, WiFi.RSSI());
  }
  wipeWifiRadio();

  enterDeepSleepWakeOnRtcMicros(kSleepAfterRefreshUs);
}

void loop() {
  // Deep sleep does not return; loop is unused.
}
