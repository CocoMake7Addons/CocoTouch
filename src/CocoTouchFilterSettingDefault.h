

#define SAMPLEFILTER_TAP_NUM 10

static uint8_t filter_taps[SAMPLEFILTER_TAP_NUM] = {
  8,
  39,
  97,
  168,
  217,
  217,
  168,
  97,
  39,
  8
};


#include "CocoTouchFilter.h"
