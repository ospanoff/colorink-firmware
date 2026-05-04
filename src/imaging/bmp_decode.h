#pragma once

#include <cstddef>
#include <cstdint>

#ifdef USE_XIAO_EPAPER_DISPLAY_BOARD_EE03
bool bmpDecodeLetterboxGray4PackedFromRam(const uint8_t *bmpData, size_t bmpLen,
                                          uint32_t expectedBmpW,
                                          uint32_t expectedBmpH, int dstW,
                                          int dstH, uint8_t **outGray4);
#endif
