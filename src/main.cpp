/*
 * Supported Colors:
 * - TFT_WHITE
 * - TFT_BLACK
 * - TFT_YELLOW
 * - TFT_GREEN
 * - TFT_BLUE
 * - TFT_RED
 */

#include "wifi_secrets.h"
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <time.h>

EPaper epaper;

static constexpr gpio_num_t kBootButtonPin = GPIO_NUM_0;
static constexpr uint64_t kSleepDurationUs = 3'600'000'000ULL;

void drawScreenContent(EPaper &display) {
  display.fillScreen(TFT_WHITE);

  display.fillCircle(25, 25, 15, TFT_BLACK);
  display.fillRect(display.width() - 40, 10, 30, 30, TFT_BLACK);

  for (int i = 0; i < display.height() / 80; i++) {
    display.setTextSize(i + 1);
    display.drawLine(10, 70 + 60 * i, display.width() - 10, 70 + 60 * i,
                     TFT_BLACK);
    display.drawString("Hello EE02", 10, 80 + 60 * i);
  }

  display.update();
}

static void enterDeepSleepHourOrButton() {
  pinMode(kBootButtonPin, INPUT_PULLUP);

  esp_sleep_enable_timer_wakeup(kSleepDurationUs);
  // One pin in the mask: wake when BOOT (GPIO0) is low.
  esp_sleep_enable_ext1_wakeup(1ULL << kBootButtonPin, ESP_EXT1_WAKEUP_ANY_LOW);

  esp_deep_sleep_start();
}

static bool connectWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  const uint8_t status = WiFi.waitForConnectResult(30'000);
  if (status != WL_CONNECTED) {
    Serial.println("WiFi: connection failed");
    return false;
  }
  Serial.printf("WiFi: connected, IP %s\n", WiFi.localIP().toString().c_str());
  return true;
}

static void syncTimeAndLog() {
  configTime(WIFI_GMT_OFFSET_SEC, 0, "pool.ntp.org", "time.nist.gov",
             "time.google.com");
  struct tm timeinfo{};
  const uint32_t start = millis();
  bool synced = false;
  while (millis() - start < 60'000) {
    if (getLocalTime(&timeinfo, 1000)) {
      synced = true;
      break;
    }
    delay(200);
  }
  if (!synced) {
    Serial.println("NTP: failed to obtain time");
    return;
  }
  char buf[48];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeinfo);
  Serial.printf("Time (offset from UTC): %s (WIFI_GMT_OFFSET_SEC=%ld)\n", buf,
                (long)WIFI_GMT_OFFSET_SEC);
}

static void printWakeupCause() {
  switch (esp_sleep_get_wakeup_cause()) {
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wake cause: RTC timer (~1 h)");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wake cause: EXT1 (BOOT / GPIO0)");
    break;
  case ESP_SLEEP_WAKEUP_UNDEFINED:
    Serial.println("Wake cause: power-on, reset, or not from deep sleep");
    break;
  default:
    Serial.printf("Wake cause: other (%d)\n",
                  (int)esp_sleep_get_wakeup_cause());
    break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(500); // USB CDC often needs a moment before the first prints appear
  printWakeupCause();

  if (connectWifi()) {
    syncTimeAndLog();
  }

  epaper.begin();
  drawScreenContent(epaper);

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  enterDeepSleepHourOrButton();
}

void loop() {
  // Deep sleep does not return; loop is unused.
}
