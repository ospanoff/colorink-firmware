#pragma once

/** Drive user LED solid on for the duration of this wake. Call early in setup(). */
void userLedWakeBegin();

/** Dark LED before entering deep sleep. */
void userLedSleepPrepare();
