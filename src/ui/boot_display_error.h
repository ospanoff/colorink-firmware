#pragma once

#include <TFT_eSPI.h>

/** Failure surfaced on the boot UI (short corner label + Serial detail). */
enum class BootDisplayError : uint8_t {
  None = 0,
  WifiConnectFailed,
  ColorInkPostBeginFailed,
  ColorInkPostRejected,
  ColorInkGetBeginFailed,
  ColorInkImageHttpNotOk,
  ColorInkNoContentLength,
  ColorInkPsramAllocFailed,
  ColorInkReadTimeout,
  ColorInkIncompleteBody,
  BmpNoData,
  BmpDecodeFailed,
  BmpPanelUnsupported,
};

const char *bootDisplayErrorShortLabel(BootDisplayError e);

/** Draw a short label at the top-right with opaque text (does not clear the
 * panel). Caller runs epaper.update() after all painting. On multigray EPaper,
 * uses gray indices compatible with the 4bpp sprite buffer. */
void drawBootErrorTopRight(EPaper &display, BootDisplayError error);
