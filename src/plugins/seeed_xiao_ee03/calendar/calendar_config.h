#pragma once

#include <cstdint>

/** Calendar HTTP base (scheme + authority). Paths are appended under calendar_http.cpp */
inline constexpr const char kCalendarHttpBaseUrl[] =
    "http://dietpi.local:8000/api/v1/plugins/calendar";

inline constexpr const char kCalendarDeviceHeader[] = "X-Device-ID";
inline constexpr const char kCalendarDeviceId[] =
    "f057b3dd-90ea-43ae-956f-7bbee82549cf";

/** Idle read/write timeout per socket syscall (covers slow renders / large body). */
inline constexpr uint32_t kCalendarIoTimeoutMs = 120000UL;

/** Nominal BMP from server /image — matches EE03 panel (1872×1404); letterboxing if BMP differs. */
inline constexpr uint32_t kCalendarImageBmpWidth = 1872;
inline constexpr uint32_t kCalendarImageBmpHeight = 1404;
