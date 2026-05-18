#pragma once

#include <esp_sleep.h>
#include <esp_system.h>

/** Stable string names matching ``esp_sleep_wakeup_cause_t`` (Espressif enum
 * tokens). */
const char *espWakeupCauseLogLabel(esp_sleep_wakeup_cause_t cause);

/** Stable string names matching ``esp_reset_reason_t`` (Espressif enum
 * tokens). */
const char *espResetReasonLogLabel(esp_reset_reason_t reason);
