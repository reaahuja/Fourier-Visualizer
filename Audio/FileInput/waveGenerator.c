#include <math.h>
#include <stdio.h>

#define samplingRate 8000
#define duration 5

const int audioSamples = samplingRate * duration;

float sine[audioSamples] = {0};
float square[audioSamples] = {0};
float sawtooth[audioSamples] = {0};
float triangle[audioSamples] = {0};
float time[audioSamples] = {0};

void createArray(const char* fileName, float output[]);
void fillInputTimes(float time[]);
void fillSine(float sine[], int frequency);
void fillSquare(float square[], int frequency);

int main() {
  fillInputTimes(time);
  fillSine(sine, 75);
  const char* fileName = "sine.txt";
  createArray(fileName, sine);
}

void fillInputTimes(float time[]) {
  float secondsPerSample = 1.0 / samplingRate;
  for (int i = 0; i < audioSamples; i++) {
    time[i] = secondsPerSample * i;
  }
}

void fillSine(float sine[], int frequency) {
  for (int i = 0; i < audioSamples; i++) {
    sine[i] = sin(2 * M_PI * frequency * time[i]);
  }
}

void fillSquare(float square[], int frequency) {}

void createArray(const char* fileName, float output[]) {
  // Save results into text files
  FILE* File = fopen(fileName, "w");

  if (File) {
    fprintf(File, "{");
    for (int i = 0; i < audioSamples; i++) {
      fprintf(File, "%.6f", output[i]);
      if (i < audioSamples -
                  1) {  // if not last entry, add comma to end of each entry
        fprintf(File, ", ");
      }
    }
    fprintf(File, "};");
    fclose(File);
  } else {
    printf("Error opening files for writing.\n");
  }
}