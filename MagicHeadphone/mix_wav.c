#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

#define BUFFER_LEN 1024  // Number of frames read in each iteration

// Function to clamp a sample to the range [-1.0, 1.0]
static float clamp(float sample) {
  if (sample > 1.0f)
    return 1.0f;
  else if (sample < -1.0f)
    return -1.0f;
  else
    return sample;
}

int main(int argc, char *argv[]) {
  if(argc != 8) {
    fprintf(stderr, "Usage: %s output.wav input1.wav input2.wav ... input6.wav\n", argv[0]);
    return 1;
  }

  int num_inputs = argc - 2;
  const char *output_filename = argv[1];

  // Open all input files and store their info
  SNDFILE **infiles = malloc(num_inputs * sizeof(SNDFILE*));
  SF_INFO *ininfos = malloc(num_inputs * sizeof(SF_INFO));
  if (!infiles || !ininfos) {
    fprintf(stderr, "Memory allocation error.\n");
    return 1;
  }

  // Open each input file and get its info
  for (int i = 0; i < num_inputs; i++) {
    ininfos[i].format = 0; // Initialize to let sf_open fill in the field
    infiles[i] = sf_open(argv[i+2], SFM_READ, &ininfos[i]);
    if (!infiles[i]) {
      fprintf(stderr, "Could not open file %s: %s\n", argv[i+2], sf_strerror(NULL));
      // Close already opened files
      for(int j = 0; j < i; j++)
        sf_close(infiles[j]);
      free(infiles);
      free(ininfos);
      return 1;
    }
    
    // Verify that the file is stereo
    if(ininfos[i].channels != 2) {
      fprintf(stderr, "File %s is not stereo.\n", argv[i+2]);
      for(int j = 0; j <= i; j++)
        sf_close(infiles[j]);
      free(infiles);
      free(ininfos);
      return 1;
  }
  }

  // Use the info from the first input file to define the output format
  SF_INFO outinfo = ininfos[0];
  // We assume that the mix will have the same length as the longest track.
  // First, find the maximum length (in frames).
  sf_count_t max_frames = 0;
  for (int i = 0; i < num_inputs; i++) {
    if (ininfos[i].frames > max_frames)
      max_frames = ininfos[i].frames;
  }

  // Open the output file
  SNDFILE *outfile = sf_open(output_filename, SFM_WRITE, &outinfo);
  if (!outfile) {
    fprintf(stderr, "Could not open output file %s: %s\n", output_filename, sf_strerror(NULL));
    for (int i = 0; i < num_inputs; i++) {
      sf_close(infiles[i]);
    }
    free(infiles);
    free(ininfos);
    return 1;
  }

  // Read buffers for each input file (buffer for frames, where each frame = 2 samples)
  float **in_buffers = malloc(num_inputs * sizeof(float*));
  if (!in_buffers) {
    fprintf(stderr, "Memory allocation error for buffers.\n");
    for (int i = 0; i < num_inputs; i++) {
      sf_close(infiles[i]);
    }
    sf_close(outfile);
    free(infiles);
    free(ininfos);
    return 1;
  }
  for (int i = 0; i < num_inputs; i++) {
    in_buffers[i] = malloc(BUFFER_LEN * outinfo.channels * sizeof(float));
    if (!in_buffers[i]) {
      fprintf(stderr, "Memory allocation error for buffer %d.\n", i);
      for (int j = 0; j < i; j++)
        free(in_buffers[j]);
      free(in_buffers);
      for (int j = 0; j < num_inputs; j++)
        sf_close(infiles[j]);
      sf_close(outfile);
      free(infiles);
      free(ininfos);
      return 1;
    }
  }

  // Output buffer
  float out_buffer[BUFFER_LEN * outinfo.channels];

  // Process frame by frame (in blocks of BUFFER_LEN frames)
  int done = 0;
  while (!done) {
    // Initialize the output buffer to 0
    int frames_to_write = BUFFER_LEN;
    for (int i = 0; i < BUFFER_LEN * outinfo.channels; i++)
      out_buffer[i] = 0.0f;

    // For each file, read BUFFER_LEN frames
    for (int i = 0; i < num_inputs; i++) {
      sf_count_t frames_read = sf_readf_float(infiles[i], in_buffers[i], BUFFER_LEN);

      // If no frames were read from this file, we'll consider its samples as 0.
      // If fewer frames were read, we use 0 for the missing samples.
      if (frames_read < frames_to_write)
          frames_to_write = frames_read < frames_to_write ? frames_read : frames_to_write;
    
      // Add to the output buffer (frame by frame and channel by channel)
      for (int j = 0; j < frames_read; j++) {
        for (int ch = 0; ch < outinfo.channels; ch++) {
            int pos = j * outinfo.channels + ch;
            out_buffer[pos] += in_buffers[i][pos];
        }
      }
    }

      // To prevent saturation, we can scale the mix; for example, by dividing by the number of tracks.
    for (int i = 0; i < BUFFER_LEN * outinfo.channels; i++) {
      out_buffer[i] = clamp(out_buffer[i] / num_inputs);
    }

    // Write the block of frames to the output file
    sf_writef_float(outfile, out_buffer, BUFFER_LEN);

    // Check if any input file still has frames left
    int anyActive = 0;
    for (int i = 0; i < num_inputs; i++) {
      if (sf_seek(infiles[i], 0, SEEK_CUR) < ininfos[i].frames)
        anyActive = 1;
    }
    if (!anyActive)
      done = 1;
  }

  // Free resources
  for (int i = 0; i < num_inputs; i++) {
    sf_close(infiles[i]);
    free(in_buffers[i]);
  }
  free(in_buffers);
  free(infiles);
  free(ininfos);
  sf_close(outfile);

  printf("Mixing complete, output file saved to %s.\n", output_filename);
  return 0;
}

