#include "net/wifi_station.h"
#include "wifi_secrets.h"

#include <Arduino.h>
#include <WiFi.h>

BootDisplayError connectWifiStation() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  const uint8_t status = WiFi.waitForConnectResult(30'000);
  if (status != WL_CONNECTED) {
    Serial.println("WiFi: connection failed");
    return BootDisplayError::WifiConnectFailed;
  }
  Serial.printf("WiFi: connected, IP %s\n", WiFi.localIP().toString().c_str());
  return BootDisplayError::None;
}
