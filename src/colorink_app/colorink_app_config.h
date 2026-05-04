#pragma once

#include <cstdint>

#include "colorink_app_secrets.h"

/** ColorInk app HTTP base (no trailing slash); POST base, GET {base}/image. */
inline constexpr const char kColorinkAppHttpBaseUrl[] =
    "http://dietpi.local:8000/api/v1/plugins/device";

/** Header name required by colorink-app for this hardware client. */
inline constexpr const char kColorinkAppHttpHeaderClientId[] = "X-Device-ID";

/** Idle read/write timeout per socket syscall (covers slow renders / large
 * body). */
inline constexpr uint32_t kColorinkAppIoTimeoutMs = 120000UL;

/** Nominal BMP dimensions from server /image — caller chooses decode target;
 * letterboxing if BMP differs. */
inline constexpr uint32_t kColorinkAppExpectedBmpWidth = 1872;
inline constexpr uint32_t kColorinkAppExpectedBmpHeight = 1404;
