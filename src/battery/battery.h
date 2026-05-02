#pragma once

#include <cstdint>

int32_t measureBatteryMilliVolts();
void batteryPinsIdleForSleep();

/** Returns 0–100, or -1 if voltage unknown (not a fuel gauge). */
int approximateBatteryPercentNominal3700Mv(int32_t batteryMilliVolts);
