#pragma once

#include <cstdint>

/** Deep sleep after each panel refresh; RTC wakes the chip after this interval. */
inline constexpr uint64_t kSleepAfterRefreshUs = 3600ULL * 1'000'000ULL;

/** Coarse SOC for nominal 3.7 V Li-ion 1S: linear from ~empty to full charge (~4.2 V). */
inline constexpr int32_t kBatSocEmptyMv = 3200;
inline constexpr int32_t kBatSocFullMv = 4200;
