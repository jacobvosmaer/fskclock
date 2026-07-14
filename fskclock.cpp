#include "daisy_pod.h"
using namespace daisy;
DaisyPod hw;
float phase;
int ticks;
float hztofreq(float hz) { return hz / hw.AudioSampleRate(); }
static void audio(AudioHandle::InterleavingInputBuffer in,
                  AudioHandle::InterleavingOutputBuffer out, size_t size) {
  hw.ProcessAllControls();
  /* knob 1 (coarse) and knob 2 (fine) control the tempo */
  int bpm = 10 + 16 * (int)(hw.knob1.Value() * 15.9) +
            (int)(hw.knob2.Value() * 15.9),
      tickreset = (2.5 * hw.AudioSampleRate()) / bpm;
  float freqhi = hztofreq(2100), freqlo = hztofreq(1300.0);
  for (int i = 0; i < (int)size; i += 2, ticks++) {
    ticks = ticks >= tickreset ? 0 : ticks;
    /* holding down button 1 freezes the clock */
    float freq =
        ticks < tickreset / 2 && !hw.button1.Pressed() ? freqhi : freqlo;
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
