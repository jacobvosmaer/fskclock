#include "daisy_pod.h"
#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")
using namespace daisy;
DaisyPod hw;
float phase;
int ticks;
float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }
static void audio(AudioHandle::InterleavingInputBuffer in,
                  AudioHandle::InterleavingOutputBuffer out, size_t size) {
  hw.ProcessAllControls();
  int bpm = 10 + 16 * (int)(hw.knob1.Value() * 15.9) +
            (int)(hw.knob2.Value() * 15.9),
      tickreset = (2.5 * hw.AudioSampleRate()) / bpm;
  for (int i = 0; i < (int)size; i += 2, ticks++) {
    ticks = ticks >= tickreset ? 0 : ticks;
    float freq = ticks < tickreset / 2 && !hw.button1.Pressed()
                     ? hztofreq(2100)
                     : hztofreq(1300);
    phase = (phase + freq) - floorf(phase + freq);
    out[i] = out[i + 1] = phase > 0.5 ? 1.0 : -1.0;
  }
}
int main(void) {
  hw.Init();
  hw.StartAdc();
  hw.StartAudio(audio);
  while (1)
    ;
}
