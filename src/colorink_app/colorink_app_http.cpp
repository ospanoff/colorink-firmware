#include "colorink_app/colorink_app_http.h"

#include "colorink_app/colorink_app_config.h"
#include "log/esp_reason_labels.h"

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <esp_heap_caps.h>
#include <esp_system.h>

static void slog(const char *line) {
  Serial.println(line);
  Serial.flush();
}

static BootDisplayError postForceUpdate(WiFiClient &client) {
  HTTPClient http;
  if (!http.begin(client, String(kColorinkAppHttpBaseUrl))) {
    Serial.println("colorink: POST begin failed");
    return BootDisplayError::ColorInkPostBeginFailed;
  }
  http.setTimeout(UINT16_MAX);
  http.addHeader(kColorinkAppHttpHeaderClientId, COLORINK_DEVICE_ID);
  http.addHeader(String("Content-Type"), String("application/json"));
  const int code = http.POST(String("{\"forceUpdate\":true}"));
  Serial.printf("colorink: POST base → %d\n", code);
  Serial.flush();
  http.end();

  if (code >= 200 && code < 300) {
    return BootDisplayError::None;
  }
  return BootDisplayError::ColorInkPostRejected;
}

static BootDisplayError
downloadImageBmpToPsram(WiFiClient &client, uint8_t **outBuf, size_t *outLen) {
  *outBuf = nullptr;
  *outLen = 0;

  HTTPClient http;
  String url(kColorinkAppHttpBaseUrl);
  url += "/image";
  if (!http.begin(client, url)) {
    Serial.println("colorink: GET begin failed");
    return BootDisplayError::ColorInkGetBeginFailed;
  }
  http.setTimeout(UINT16_MAX);
  http.addHeader(kColorinkAppHttpHeaderClientId, COLORINK_DEVICE_ID);

  const int code = http.GET();
  Serial.printf("colorink: GET …/image → %d\n", code);
  Serial.flush();
  if (code != HTTP_CODE_OK) {
    http.end();
    return BootDisplayError::ColorInkImageHttpNotOk;
  }

  const int bodyLen = http.getSize();
  if (bodyLen <= 0) {
    slog("colorink: image needs Content-Length (chunked unsupported)");
    http.end();
    return BootDisplayError::ColorInkNoContentLength;
  }

  Serial.printf("colorink: loading %d B into PSRAM…\n", bodyLen);
  Serial.flush();

  uint8_t *buf = static_cast<uint8_t *>(
      heap_caps_malloc(static_cast<size_t>(bodyLen), MALLOC_CAP_SPIRAM));
  if (!buf) {
    Serial.println("colorink: PSRAM alloc failed");
    http.end();
    return BootDisplayError::ColorInkPsramAllocFailed;
  }

  WiFiClient *stream = http.getStreamPtr();
  size_t filled = 0;
  const size_t total = static_cast<size_t>(bodyLen);
  const uint32_t t0_ms = millis();
  // Total wall time for body read; per-read idle is UINT16_MAX via
  // http.setTimeout above.
  constexpr uint32_t kBodyReadWallMs = 120000UL;

  while (filled < total) {
    if (millis() - t0_ms > kBodyReadWallMs) {
      Serial.println("colorink: PSRAM fill timeout");
      heap_caps_free(buf);
      http.end();
      return BootDisplayError::ColorInkReadTimeout;
    }

    const int avail = stream->available();
    if (avail <= 0) {
      if (!http.connected()) {
        break;
      }
      delay(2);
      continue;
    }

    size_t want = total - filled;
    if (static_cast<size_t>(avail) < want) {
      want = static_cast<size_t>(avail);
    }
    const size_t rd =
        stream->readBytes(reinterpret_cast<char *>(buf + filled), want);
    if (rd == 0) {
      delay(2);
      continue;
    }
    filled += rd;
    if ((filled & ((size_t{1} << 18) - 1)) == 0 && filled != 0) {
      Serial.printf("colorink: %u / %u B\n", unsigned(filled), unsigned(total));
      Serial.flush();
    }
  }

  http.end();

  Serial.printf("colorink: PSRAM read %u B in %lu ms\n", unsigned(filled),
                static_cast<unsigned long>(millis() - t0_ms));
  Serial.flush();

  if (filled != total) {
    Serial.printf("colorink: incomplete body got=%u expected=%u\n",
                  unsigned(filled), unsigned(total));
    heap_caps_free(buf);
    return BootDisplayError::ColorInkIncompleteBody;
  }

  *outBuf = buf;
  *outLen = total;
  return BootDisplayError::None;
}

BootDisplayError colorinkAppRefreshBmpToPsram(uint8_t **outBuf,
                                              size_t *outLen) {
  *outBuf = nullptr;
  *outLen = 0;

  WiFiClient client;

  const BootDisplayError post_err = postForceUpdate(client);
  if (post_err != BootDisplayError::None) {
    return post_err;
  }
  return downloadImageBmpToPsram(client, outBuf, outLen);
}

static void appendJsonEscaped(String &out, const char *src) {
  for (; *src != '\0'; ++src) {
    const char c = *src;
    switch (c) {
    case '\\':
      out += F("\\\\");
      break;
    case '"':
      out += F("\\\"");
      break;
    case '\n':
      out += F("\\n");
      break;
    case '\r':
      out += F("\\r");
      break;
    case '\t':
      out += F("\\t");
      break;
    default: {
      const auto u = static_cast<unsigned char>(c);
      if (u >= 0x20) {
        out += c;
      }
      break;
    }
    }
  }
}

/** POST ``logUtf8`` to device-logs URL (JSON envelope, ``X-Device-ID``);
 * TU-local. */
static void postDeviceLogsHttp(const char *logUtf8) {
  if (logUtf8 == nullptr || logUtf8[0] == '\0') {
    return;
  }
  WiFiClient client;
  constexpr uint16_t kLogHttpTimeoutMs = 8000;

  String envelope;
  const size_t n = strlen(logUtf8);
  envelope.reserve(static_cast<unsigned>(n * 2U + 12U));
  envelope = F("{\"log\":\"");
  appendJsonEscaped(envelope, logUtf8);
  envelope += F("\"}");

  HTTPClient http;
  if (!http.begin(client, String(kColorinkAppHttpDeviceLogsUrl))) {
    Serial.println("colorink: device logs POST begin failed");
    Serial.flush();
    return;
  }
  http.setTimeout(kLogHttpTimeoutMs);
  http.addHeader(kColorinkAppHttpHeaderClientId, COLORINK_DEVICE_ID);
  http.addHeader(String("Content-Type"), String("application/json"));
  const int code = http.POST(envelope);
  Serial.printf("colorink: POST …/devices/logs → %d\n", code);
  Serial.flush();
  http.end();
}

void colorinkAppPostBootWakeLog(esp_sleep_wakeup_cause_t wakeup_cause,
                                esp_reset_reason_t reset_reason, int32_t battery_mv,
                                int battery_percent, BootDisplayError overlay_err,
                                int wifi_rssi) {
  char logLine[512];
  const int ln = std::snprintf(
      logLine, sizeof(logLine),
      "wakeup_cause=%s reset_reason=%s battery_mv=%ld battery_pct=%d "
      "display_status=\"%s\" wifi_rssi=%d",
      espWakeupCauseLogLabel(wakeup_cause), espResetReasonLogLabel(reset_reason),
      static_cast<long>(battery_mv), battery_percent,
      bootDisplayErrorHumanLogLabel(overlay_err), wifi_rssi);
  if (ln <= 0 || static_cast<size_t>(ln) >= sizeof(logLine)) {
    Serial.println("colorink: boot wake log line truncated");
    Serial.flush();
    return;
  }

  postDeviceLogsHttp(logLine);
}
