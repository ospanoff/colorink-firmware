#pragma once

#include "boot_display_error.h"

#include <cstddef>
#include <cstdint>

/** Ask the plugin to regenerate the BMP, then download it into PSRAM.
 * On failure, *outBuf stays nullptr and *outLen zero.
 */
BootDisplayError colorinkAppRefreshBmpToPsram(uint8_t **outBuf, size_t *outLen);

/** Format default boot/wake telemetry and POST it to the device-logs API (STA
 * up). */
void colorinkAppPostBootWakeLog(bool wakeup_from_rtc, int32_t battery_mv,
                                int battery_percent,
                                BootDisplayError overlay_err, int wifi_rssi);
