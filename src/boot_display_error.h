#pragma once

#include <cstdint>

/** Result of WiFi, ColorInk HTTP, BMP decode, or panel paint; may be shown on
 * the e-paper as a short corner label (see wake_display_overlays). */
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

/** Human-readable status: ``OK`` when ``None``, otherwise short panel-style
 * text. Draw as an e-paper overlay only when ``error !=
 * BootDisplayError::None``; logs may call for any value (including ``None`` →
 * ``OK``). */
const char *bootDisplayErrorHumanLogLabel(BootDisplayError e);
