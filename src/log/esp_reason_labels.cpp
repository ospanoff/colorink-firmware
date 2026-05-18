#include "log/esp_reason_labels.h"

#include <esp_sleep.h>
#include <esp_system.h>

const char *espWakeupCauseLogLabel(esp_sleep_wakeup_cause_t cause) {
  switch (cause) {
  case ESP_SLEEP_WAKEUP_UNDEFINED:
    return "ESP_SLEEP_WAKEUP_UNDEFINED";
  case ESP_SLEEP_WAKEUP_ALL:
    return "ESP_SLEEP_WAKEUP_ALL";
  case ESP_SLEEP_WAKEUP_EXT0:
    return "ESP_SLEEP_WAKEUP_EXT0";
  case ESP_SLEEP_WAKEUP_EXT1:
    return "ESP_SLEEP_WAKEUP_EXT1";
  case ESP_SLEEP_WAKEUP_TIMER:
    return "ESP_SLEEP_WAKEUP_TIMER";
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    return "ESP_SLEEP_WAKEUP_TOUCHPAD";
  case ESP_SLEEP_WAKEUP_ULP:
    return "ESP_SLEEP_WAKEUP_ULP";
  case ESP_SLEEP_WAKEUP_GPIO:
    return "ESP_SLEEP_WAKEUP_GPIO";
  case ESP_SLEEP_WAKEUP_UART:
    return "ESP_SLEEP_WAKEUP_UART";
  case ESP_SLEEP_WAKEUP_UART1:
    return "ESP_SLEEP_WAKEUP_UART1";
  case ESP_SLEEP_WAKEUP_UART2:
    return "ESP_SLEEP_WAKEUP_UART2";
  case ESP_SLEEP_WAKEUP_WIFI:
    return "ESP_SLEEP_WAKEUP_WIFI";
  case ESP_SLEEP_WAKEUP_COCPU:
    return "ESP_SLEEP_WAKEUP_COCPU";
  case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG:
    return "ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG";
  case ESP_SLEEP_WAKEUP_BT:
    return "ESP_SLEEP_WAKEUP_BT";
  case ESP_SLEEP_WAKEUP_VAD:
    return "ESP_SLEEP_WAKEUP_VAD";
  case ESP_SLEEP_WAKEUP_VBAT_UNDER_VOLT:
    return "ESP_SLEEP_WAKEUP_VBAT_UNDER_VOLT";
  default:
    return "ESP_SLEEP_WAKEUP_UNRECOGNIZED";
  }
}

const char *espResetReasonLogLabel(esp_reset_reason_t reason) {
  switch (reason) {
  case ESP_RST_UNKNOWN:
    return "ESP_RST_UNKNOWN";
  case ESP_RST_POWERON:
    return "ESP_RST_POWERON";
  case ESP_RST_EXT:
    return "ESP_RST_EXT";
  case ESP_RST_SW:
    return "ESP_RST_SW";
  case ESP_RST_PANIC:
    return "ESP_RST_PANIC";
  case ESP_RST_INT_WDT:
    return "ESP_RST_INT_WDT";
  case ESP_RST_TASK_WDT:
    return "ESP_RST_TASK_WDT";
  case ESP_RST_WDT:
    return "ESP_RST_WDT";
  case ESP_RST_DEEPSLEEP:
    return "ESP_RST_DEEPSLEEP";
  case ESP_RST_BROWNOUT:
    return "ESP_RST_BROWNOUT";
  case ESP_RST_SDIO:
    return "ESP_RST_SDIO";
  case ESP_RST_USB:
    return "ESP_RST_USB";
  case ESP_RST_JTAG:
    return "ESP_RST_JTAG";
  case ESP_RST_EFUSE:
    return "ESP_RST_EFUSE";
  case ESP_RST_PWR_GLITCH:
    return "ESP_RST_PWR_GLITCH";
  case ESP_RST_CPU_LOCKUP:
    return "ESP_RST_CPU_LOCKUP";
  default:
    return "ESP_RST_UNRECOGNIZED";
  }
}
