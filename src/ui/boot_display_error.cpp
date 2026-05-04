#include "ui/boot_display_error.h"

const char *bootDisplayErrorShortLabel(BootDisplayError e) {
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

void drawBootErrorTopRight(EPaper &display, BootDisplayError error) {
  if (error == BootDisplayError::None) {
    return;
  }

  const char *msg = bootDisplayErrorShortLabel(error);
  const int pad_x = 12;
  const int pad_y = 8;
  const int text_size = display.width() >= 900 ? 3 : 2;

  display.setTextDatum(TR_DATUM);
  display.setTextSize(text_size);
  display.setTextColor(TFT_BLACK, TFT_WHITE);
  display.drawString(msg, display.width() - pad_x, pad_y);
  display.setTextDatum(TL_DATUM);
}
