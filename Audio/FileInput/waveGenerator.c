#include <stdio.h>
#define audioSamples 40000
float sine[audioSamples] = {0};

void createArray(const char* fileName, float output[]);

int main() {
  const char* fileName = "sine.txt";
  createArray(fileName, sine);
}

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