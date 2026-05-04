#pragma once

#include <cstdint>

#include "colorink_app_secrets.h"

/** ColorInk app HTTP base (no trailing slash); refresh POSTs here, GET
 * {base}/image for the BMP. */
inline constexpr const char kColorinkAppHttpBaseUrl[] =
    "http://dietpi.local:8000/api/v1/plugins/device";

/** Header name required by colorink-app for this hardware client. */
inline constexpr const char kColorinkAppHttpHeaderClientId[] = "X-Device-ID";

/** Idle read/write timeout per socket syscall (covers slow renders / large
 * body). */
inline constexpr uint32_t kColorinkAppIoTimeoutMs = 120000UL;
