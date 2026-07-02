#include "daisy_pod.h"
#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")
#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
using namespace daisy;
DaisyPod hw;
float phase, freq;
int bpm = 120, ticks, tickreset;
#define freqhi 2100
#define freqlo 1300
float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }
static void audio(AudioHandle::InterleavingInputBuffer in,
                  AudioHandle::InterleavingOutputBuffer out, size_t size) {
  hw.ProcessAllControls();
  bpm += hw.encoder.Increment();
  bpm = max(min(bpm, 300), 20);
  tickreset = (2.5 * hw.AudioSampleRate()) / bpm;
  for (int i = 0; i < (int)size; i += 2) {
    if (ticks >= tickreset)
      ticks = 0;
    if (ticks < tickreset / 2 && !hw.button1.Pressed()) {
      freq = hztofreq(freqhi);
    } else {
      freq = hztofreq(freqlo);
    }
    ticks++;
    phase = (phase + freq) - floorf(phase + freq);
    out[i] = out[i + 1] = phase > 0.5 ? 1.0 : -1.0;
  }
}
int main(void) {
  hw.Init();
  hw.StartAudio(audio);
  while (1)
    ;
}
