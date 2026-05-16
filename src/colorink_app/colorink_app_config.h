#pragma once

#include "colorink_app_secrets.h"

/** ColorInk API URLs (no trailing slash). BMP: POST ``plugins/device``, GET
 * ``…/image``. Device logs: POST ``kColorinkAppHttpDeviceLogsUrl`` (opaque
 * lines). Same host/port. */
inline constexpr const char kColorinkAppHttpBaseUrl[] =
    "http://dietpi.local:8000/api/v1/plugins/device";
inline constexpr const char kColorinkAppHttpDeviceLogsUrl[] =
    "http://dietpi.local:8000/api/v1/devices/logs";

/** Header name required by colorink-app for this hardware client. */
inline constexpr const char kColorinkAppHttpHeaderClientId[] = "X-Device-ID";
