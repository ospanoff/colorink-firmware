#pragma once

#include "boot_display_error.h"

#include <cstddef>
#include <cstdint>

/** Ask the plugin to regenerate the BMP, then download it into PSRAM.
 * On failure, *outBuf stays nullptr and *outLen zero.
 */
BootDisplayError colorinkAppRefreshBmpToPsram(uint8_t **outBuf, size_t *outLen);
