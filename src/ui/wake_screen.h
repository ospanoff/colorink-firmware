#pragma once

#include <TFT_eSPI.h>
#include <cstdint>

void drawWakeBatteryScreen(EPaper &display, bool wakeup_from_rtc_timer,
                           int32_t battery_mv, int battery_percent);
