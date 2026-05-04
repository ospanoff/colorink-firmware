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
#include "power/power_sleep.h"
#include "ui/wake_screen.h"

#ifdef USE_XIAO_EPAPER_DISPLAY_BOARD_EE03
#include "colorink_app/colorink_app_config.h"
#include "colorink_app/colorink_app_http.h"
#include "imaging/bmp_decode.h"
#include "net/wifi_station.h"
#include <WiFi.h>
#include <esp_heap_caps.h>
#endif

#include <Arduino.h>

#include <TFT_eSPI.h>
#include <esp_sleep.h>

EPaper epaper;

#ifdef USE_XIAO_EPAPER_DISPLAY_BOARD_EE03

static void wipeWifiRadio() {
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

#endif

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

#ifdef ED103TC2_DRIVER
  uint8_t *frameGray4 = nullptr;
#endif

#ifdef USE_XIAO_EPAPER_DISPLAY_BOARD_EE03

  if (connectWifiStation()) {
    const bool post_ok = colorinkAppPostForceUpdate();
    uint8_t *bmpRam = nullptr;
    size_t bmpLen = 0;
    const bool fetch_ok =
        post_ok && colorinkAppDownloadImageBmpToPsram(&bmpRam, &bmpLen);

    bool decoded_ok = false;
    if (fetch_ok && bmpRam != nullptr) {
      decoded_ok = bmpDecodeGray4PackedFromRam(bmpRam, bmpLen, TFT_WIDTH,
                                               TFT_HEIGHT, &frameGray4);
    }
    Serial.printf("colorink: BMP decode → %s\n", decoded_ok ? "ok" : "failed");
    Serial.flush();
    if (!decoded_ok) {
      frameGray4 = nullptr;
    }

    if (bmpRam != nullptr) {
      heap_caps_free(bmpRam);
    }

    wipeWifiRadio();
  }

#endif

  Serial.println("epaper: begin…");
  Serial.flush();
  epaper.begin();
  Serial.println("epaper: begin done");
  Serial.flush();

#ifdef ED103TC2_DRIVER
  if (frameGray4 != nullptr) {
    Serial.println("epaper: gray push + update (may take tens of seconds)…");
    Serial.flush();
    epaper.initGrayMode(GRAY_LEVEL16);
    epaper.fillScreen(TFT_GRAY_15);
    epaper.pushImage(0, 0, TFT_WIDTH, TFT_HEIGHT,
                     reinterpret_cast<uint16_t *>(frameGray4));
    heap_caps_free(frameGray4);
    frameGray4 = nullptr;
    epaper.update();
    Serial.println("epaper: update done");
    Serial.flush();
  } else {
    epaper.fillScreen(TFT_WHITE);
    drawWakeBatteryScreen(epaper, wakeup_from_rtc_timer, battery_mv,
                          battery_percent);
  }
#else
  epaper.fillScreen(TFT_WHITE);
  drawWakeBatteryScreen(epaper, wakeup_from_rtc_timer, battery_mv,
                        battery_percent);
#endif

  enterDeepSleepWakeOnRtcMicros(kSleepAfterRefreshUs);
}

void loop() {
  // Deep sleep does not return; loop is unused.
}
