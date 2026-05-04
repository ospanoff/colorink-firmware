#pragma once

#include <cstddef>
#include <cstdint>

/** Decode palettized 8 bpp or RGB24 BMP from RAM into packed 4 bpp gray in
 * PSRAM. BMP width/height must exactly match displayW × displayH (no scaling).
 */
bool bmpDecodeGray4PackedFromRam(const uint8_t *bmpData, size_t bmpLen,
                                 int displayW, int displayH,
                                 uint8_t **outGray4);
