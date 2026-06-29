#include "daisy_pod.h"
#undef assert
#define assert(x)                                                              \
  if (!(x))                                                                    \
  asm("bkpt 255")
using namespace daisy;
DaisyPod hw;
float phase, freq, tempoperiod;
int bpm = 120, ticks, tickreset;
#define freqhi 2100
#define freqlo 1300
float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }
static void audio(AudioHandle::InterleavingInputBuffer in,
                  AudioHandle::InterleavingOutputBuffer out, size_t size) {
  hw.encoder.Debounce();
  bpm += hw.encoder.Increment();
  bpm = bpm < 10 ? 10 : (bpm > 250 ? 250 : bpm);
  tempoperiod = 2.5 * (float)tickreset / (float)bpm;
  for (int i = 0; i < (int)size; i += 2) {
    if (!((int)((float)ticks / (0.5 * tempoperiod)) & 1) &&
        (tickreset - ticks) >= floorf(tempoperiod)) {
      freq = hztofreq(freqhi);
    } else {
      freq = hztofreq(freqlo);
    }
    if (++ticks == tickreset)
      ticks = 0;
    phase = (phase + freq) - floorf(phase + freq);
    out[i] = out[i + 1] = phase > 0.5 ? 1.0 : -1.0;
  }
}
int main(void) {
  hw.Init();
  tickreset = hw.AudioSampleRate();
  hw.StartAudio(audio);
  while (1)
    ;
}
