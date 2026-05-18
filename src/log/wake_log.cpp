#include "log/wake_log.h"

#include "log/esp_reason_labels.h"

#include <Arduino.h>

void printWakeBatteryToSerial(esp_sleep_wakeup_cause_t wakeup_cause,
                              int32_t battery_mv, int battery_percent) {
  Serial.print(espWakeupCauseLogLabel(wakeup_cause));
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
