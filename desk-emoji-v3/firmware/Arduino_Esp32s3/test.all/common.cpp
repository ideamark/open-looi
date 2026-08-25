#include "common.h"

void setup_FFat() {
  if (!FFat.begin(true)) {
    log_error("FATFS Initiate Failed.");
    while (true);  // Pause
  } else {
    log_info("FATFS is Ready.");
  }
}
