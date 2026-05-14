#include "ui/wake_display_overlays.h"

#include <cstdio>

static void setWakeOverlayTextColor(EPaper &display) {
#ifdef USE_MUTIGRAY_EPAPER
  display.setTextColor(TFT_GRAY_0, TFT_GRAY_15, true);
#else
  display.setTextColor(TFT_BLACK, TFT_WHITE, true);
#endif
}

const char *wakeOverlayErrorShortLabel(BootDisplayError e) {
  switch (e) {
  case BootDisplayError::None:
    return "";
  case BootDisplayError::WifiConnectFailed:
    return "WiFi failed";
  case BootDisplayError::ColorInkPostBeginFailed:
    return "POST open fail";
  case BootDisplayError::ColorInkPostRejected:
    return "POST rejected";
  case BootDisplayError::ColorInkGetBeginFailed:
    return "GET open fail";
  case BootDisplayError::ColorInkImageHttpNotOk:
    return "Image HTTP";
  case BootDisplayError::ColorInkNoContentLength:
    return "No Content-Length";
  case BootDisplayError::ColorInkPsramAllocFailed:
    return "PSRAM alloc";
  case BootDisplayError::ColorInkReadTimeout:
    return "Read timeout";
  case BootDisplayError::ColorInkIncompleteBody:
    return "Truncated body";
  case BootDisplayError::BmpNoData:
    return "No BMP";
  case BootDisplayError::BmpDecodeFailed:
    return "BMP decode";
  case BootDisplayError::BmpPanelUnsupported:
    return "No BMP panel";
  default:
    return "?";
  }
}

void drawWakeErrorOverlayTopRight(EPaper &display, BootDisplayError error) {
  if (error == BootDisplayError::None) {
    return;
  }

  const char *msg = wakeOverlayErrorShortLabel(error);
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
