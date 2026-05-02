#pragma once

#include <cstdint>

void printWakeBatteryToSerial(bool wakeup_from_rtc_timer, int32_t battery_mv,
                              int battery_percent);
