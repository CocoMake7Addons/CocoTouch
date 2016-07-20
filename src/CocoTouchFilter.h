#ifndef SAMPLEFILTER_H_
#define SAMPLEFILTER_H_

typedef struct {
  int history[SAMPLEFILTER_TAP_NUM];
  unsigned int last_index;
} CocoTouchFilterSetting;

void CocoTouchFilter_init(CocoTouchFilterSetting* f) {
  int i;
  for(i = 0; i < SAMPLEFILTER_TAP_NUM; ++i)
    f->history[i] = 0;
  f->last_index = 0;
}

void CocoTouchFilter_put(CocoTouchFilterSetting* f, int input) {
  f->history[f->last_index++] = input;
  if(f->last_index == SAMPLEFILTER_TAP_NUM)
    f->last_index = 0;
}

int CocoTouchFilter_get(CocoTouchFilterSetting* f) {
  long long acc = 0;
  int index = f->last_index, i;
  for(i = 0; i < SAMPLEFILTER_TAP_NUM; ++i) {
    index = index != 0 ? index-1 : SAMPLEFILTER_TAP_NUM-1;
    acc += (long long)f->history[index] * filter_taps[i];
  };
  return acc >> 10;
}

#endif
