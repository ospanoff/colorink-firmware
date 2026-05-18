#pragma once

#include "boot_display_error.h"

#include <esp_sleep.h>
#include <esp_system.h>

#include <cstddef>
#include <cstdint>

/** Ask the plugin to regenerate the BMP, then download it into PSRAM.
 * On failure, *outBuf stays nullptr and *outLen zero.
 */
BootDisplayError colorinkAppRefreshBmpToPsram(uint8_t **outBuf, size_t *outLen);

/** Format default boot/wake telemetry and POST it to the device-logs API (STA
 * up). ``wakeup_cause`` / ``reset_reason`` are from ``esp_sleep_get_wakeup_cause``
 * and ``esp_reset_reason`` (caller supplies both; see ``esp_reason_labels`` for
 * string tokens). */
void colorinkAppPostBootWakeLog(esp_sleep_wakeup_cause_t wakeup_cause,
                                esp_reset_reason_t reset_reason, int32_t battery_mv,
                                int battery_percent, BootDisplayError overlay_err,
                                int wifi_rssi);
