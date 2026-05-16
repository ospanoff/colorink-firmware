#include "boot_display_error.h"

const char *bootDisplayErrorHumanLogLabel(BootDisplayError e) {
  switch (e) {
  case BootDisplayError::None:
    return "OK";
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
