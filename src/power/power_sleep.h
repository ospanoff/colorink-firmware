#pragma once

#include <cstdint>

/** RTC timer wakeup only; arms no EXT1. Idles battery sense pin first. */
void enterDeepSleepWakeOnRtcMicros(uint64_t duration_micros);
