void fillSawtooth(float sawtooth[], int frequency) {
  float period = 1.0 / frequency;
  for (int i = 0; i < audioSamples; i++) {
    sawtooth[i] = 2.0 * (time[i] / period - floor(0.5 + time[i] / period));
  }
}