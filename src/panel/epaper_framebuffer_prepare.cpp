#include "panel/epaper_framebuffer_prepare.h"

void prepareEpaperFramebuffer(EPaper &epaper) {
#ifdef USE_MUTIGRAY_EPAPER
  epaper.initGrayMode(GRAY_LEVEL16);
  epaper.fillScreen(TFT_GRAY_15);
#endif
}
