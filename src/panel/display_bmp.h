#pragma once

#include <cstddef>
#include <cstdint>

#include "ui/wake_screen.h"

/** Decode and paint BMP on the panel when supported by this build (EE03+ED103).
 * Returns true only after a successful decode + refresh; caller draws fallback
 * UI when false. */
bool displayBmp(EPaper &epaper, const uint8_t *bmpRam, size_t bmpLen);
