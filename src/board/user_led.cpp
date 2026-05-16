#include "board/user_led.h"

#include <Arduino.h>

#if !defined(USE_XIAO_EPAPER_DISPLAY_BOARD_EE02) &&                             \
    !defined(USE_XIAO_EPAPER_DISPLAY_BOARD_EE03)
#error "user_led is defined for USE_XIAO_EPAPER_DISPLAY_BOARD EE02 / EE03"
#endif

namespace {

// Seeed XIAO ESP32-S3 user LED is active low (LOW = illuminated).
constexpr bool kLitLevel = LOW;
constexpr bool kDarkLevel = HIGH;

void setLit(bool lit) {
  digitalWrite(LED_BUILTIN, lit ? kLitLevel : kDarkLevel);
}

} // namespace

void userLedWakeBegin() {
  pinMode(LED_BUILTIN, OUTPUT);
  setLit(true);
}

void userLedSleepPrepare() { setLit(false); }
