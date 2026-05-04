#include "panel/display_bmp.h"

#if defined(ED103TC2_DRIVER)
#include "panel/ee03/display_bmp_ed103.h"
#endif

bool displayBmp([[maybe_unused]] EPaper &epaper,
                [[maybe_unused]] const uint8_t *bmpRam,
                [[maybe_unused]] size_t bmpLen) {
#if defined(ED103TC2_DRIVER)
  return displayBmpEe03Ed103(epaper, bmpRam, bmpLen);
#else
  return false;
#endif
}
