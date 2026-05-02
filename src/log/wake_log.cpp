#include "log/wake_log.h"

#include <Arduino.h>

void printWakeBatteryToSerial(bool wakeup_from_rtc_timer, int32_t battery_mv,
                              int battery_percent) {
  Serial.print(wakeup_from_rtc_timer ? "Timer" : "Force");
  if (battery_mv >= 0) {
    Serial.printf("  %ld mV", static_cast<long>(battery_mv));
    if (battery_percent >= 0) {
      Serial.printf("  (~%d%%)\n", battery_percent);
    } else {
      Serial.println();
    }
  } else {
    Serial.print("  Battery: ---\n");
  }
}
