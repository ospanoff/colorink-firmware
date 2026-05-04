#pragma once

#include <TFT_eSPI.h>
#include <cstddef>
#include <cstdint>

#include "ui/boot_display_error.h"

/** Decode and paint BMP on the panel when supported by this build (EE03+ED103).
 * Returns BootDisplayError::None on success. Does not call epaper.update();
 * caller updates after optional overlays.
 */
BootDisplayError displayBmp(EPaper &epaper, const uint8_t *bmpRam,
                            size_t bmpLen);
