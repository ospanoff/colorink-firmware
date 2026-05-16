#include "battery/battery.h"
#include "board/board_pins.h"
#include "config/app_config.h"

#include <Arduino.h>

void batteryPinsIdleForSleep() { pinMode(kBatAdcEnablePin, INPUT_PULLDOWN); }

int32_t measureBatteryMilliVolts() {
  analogReadResolution(12);
  analogSetPinAttenuation(static_cast<uint8_t>(kBatAdcSensePin), ADC_11db);

  pinMode(kBatAdcEnablePin, OUTPUT);
  digitalWrite(kBatAdcEnablePin, LOW);
  delay(2);
  digitalWrite(kBatAdcEnablePin, HIGH);
  delay(10);

  const uint32_t sense_mv =
      analogReadMilliVolts(static_cast<uint8_t>(kBatAdcSensePin));
  digitalWrite(kBatAdcEnablePin, LOW);
  batteryPinsIdleForSleep();

  if (sense_mv > 2800 || sense_mv < 40) {
    return -1;
  }

  const int32_t pack_mv =
      static_cast<int32_t>(static_cast<float>(sense_mv) * kBatDividerRatio);
  return pack_mv > 9800 ? -1 : pack_mv;
}

int approximateBatteryPercentNominal3700Mv(int32_t battery_mv) {
  if (battery_mv < 0) {
    return -1;
  }
  if (battery_mv >= kBatSocFullMv) {
    return 100;
  }
  if (battery_mv <= kBatSocEmptyMv) {
    return 0;
  }
  const int span = static_cast<int>(kBatSocFullMv - kBatSocEmptyMv);
  return static_cast<int>((battery_mv - kBatSocEmptyMv) * 100L / span);
}
