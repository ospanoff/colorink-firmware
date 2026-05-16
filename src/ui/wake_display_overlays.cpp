#include "ui/wake_display_overlays.h"

#include <cstdio>

static void setWakeOverlayTextColor(EPaper &display) {
#ifdef USE_MUTIGRAY_EPAPER
  display.setTextColor(TFT_GRAY_0, TFT_GRAY_15, true);
#else
  display.setTextColor(TFT_BLACK, TFT_WHITE, true);
#endif
}

void drawWakeErrorOverlayTopRight(EPaper &display, BootDisplayError error) {
  if (error == BootDisplayError::None) {
    return;
  }

  const char *msg = bootDisplayErrorHumanLogLabel(error);
  const int pad_x = 12;
  const int pad_y = 8;
  const int text_size = display.width() >= 900 ? 3 : 2;

  display.setTextDatum(TR_DATUM);
  display.setTextSize(text_size);
  setWakeOverlayTextColor(display);
  display.drawString(msg, display.width() - pad_x, pad_y);
  display.setTextDatum(TL_DATUM);
}

void drawWakeLowBatteryBottomRight(EPaper &display, int battery_percent) {
  if (battery_percent < 0 || battery_percent >= 20) {
    return;
  }

  char buf[8];
  std::snprintf(buf, sizeof(buf), "%d%%", battery_percent);

  const int pad_x = 12;
  const int pad_y = 8;
  const int text_size = display.width() >= 900 ? 3 : 2;

  display.setTextDatum(BR_DATUM);
  display.setTextSize(text_size);
  setWakeOverlayTextColor(display);
  display.drawString(buf, display.width() - pad_x, display.height() - pad_y);
  display.setTextDatum(TL_DATUM);
}
