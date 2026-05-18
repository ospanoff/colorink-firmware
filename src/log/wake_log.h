#pragma once

#include <esp_sleep.h>

#include <cstdint>

void printWakeBatteryToSerial(esp_sleep_wakeup_cause_t wakeup_cause,
                              int32_t battery_mv, int battery_percent);
