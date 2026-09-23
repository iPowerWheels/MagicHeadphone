#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sndfile.h>

void convolve(float *signal, int signal_length, float *impulse, int impulse_length, float *output, int channels) {
  for (int ch = 0; ch < channels; ch++){
    // Channel by channel
    for (int n = 0; n < signal_length + impulse_length - 1; n++) {
      
      // Use double for bit-perfect accumulation to prevent floating-point drift
      double sum = 0.0;
      
      for (int k = 0; k < impulse_length; k++) {
        if (n - k >= 0 && n - k < signal_length) {
          sum += (double)signal[(n - k) * channels + ch] * (double)impulse[k * channels + ch];
        }
      }
      
      // Cast back to 32-bit float only at the very end to write to the buffer
      output[n * channels + ch] = (float)sum;
    }
  }
}

void HRTF (int suffix){
  // Read variables
  SNDFILE *signal_file, *impulse_file;
  SF_INFO signal_info, impulse_info;

  // Write variables
  SNDFILE *outfile;
  SF_INFO sfinfo;

  float *signal_buffer, *impulse_buffer, *full_output_buffer, *final_output_buffer;

  // Names
  char signal_file_path[50];
  snprintf(signal_file_path, sizeof(signal_file_path), "song-%d.wav", suffix);

  char impulse_file_path[50];
  snprintf(impulse_file_path, sizeof(impulse_file_path), "impulses/impulse-%d.wav", suffix);

  // Open input files
  signal_file = sf_open(signal_file_path, SFM_READ, &signal_info);
  impulse_file = sf_open(impulse_file_path, SFM_READ, &impulse_info);

  if (!signal_file || !impulse_file) {
    printf("Error opening input files\n");
    exit(1);
  }

  // Memory assign
  signal_buffer = (float *) malloc(signal_info.frames * signal_info.channels * sizeof(float));
  impulse_buffer = (float *) malloc(impulse_info.frames * impulse_info.channels * sizeof(float));

  // Read files
  int signal_samples = sf_read_float(signal_file, signal_buffer, signal_info.frames * signal_info.channels);
  int impulse_samples = sf_read_float(impulse_file, impulse_buffer, impulse_info.frames * impulse_info.channels);

  // Verify channels
  if(signal_info.channels != impulse_info.channels) {
    printf("Mismatch channels\n");
    sf_close(signal_file);
    sf_close(impulse_file);
    free(signal_buffer);
    free(impulse_buffer);
    exit(1);
  }
  int channels = signal_info.channels;
  
  // Frames length
  int signal_frames = signal_samples / channels;
  int impulse_frames = impulse_samples / channels;

  // Full convolution
  int full_output_frames = signal_frames + impulse_frames - 1;
  int full_samples = full_output_frames * channels;
  full_output_buffer = (float *) malloc(full_samples * sizeof(float));

  // Convolution
  convolve(signal_buffer, signal_frames, impulse_buffer, impulse_frames, full_output_buffer, channels);

  // NORMALIZATION
  /*
  float max_peak = 0.0f;
  for (int i = 0; i < full_samples; i++) {
    float abs_val = fabsf(full_output_buffer[i]);
    if (abs_val > max_peak) {
      max_peak = abs_val;
    }
  }

  // If the signal exceeds 1.0 (0dBFS), normalize the whole buffer down.
  // We use 0.99f as a slight safety margin for inter-sample peaks.
  if (max_peak > 1.0f) {
    float normalization_factor = 0.99f / max_peak;
    for (int i = 0; i < full_samples; i++) {
      full_output_buffer[i] *= normalization_factor;
    }
  }
  */
    
  // HRTF ALIGNMENT - ZERO-OFFSET TRUNCATION
  int final_output_frames = signal_frames;
  final_output_buffer = (float *) malloc(final_output_frames * channels * sizeof(float));

  // Copy exactly from the beginning. No offset shifting!
  for (int n = 0; n < final_output_frames; n++) {
    for (int ch = 0; ch < channels; ch++){
      int index = n * channels + ch;
      final_output_buffer[index] = full_output_buffer[index];
    }
  }

  // Output file setup
  // Keeping exactly as requested: 24-bit PCM
  sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_24;
  sfinfo.channels = channels;
  sfinfo.samplerate = signal_info.samplerate;

  char output_file_path[50];
  snprintf(output_file_path, sizeof(output_file_path), "output-%d.wav", suffix);
  outfile = sf_open(output_file_path, SFM_WRITE, &sfinfo);
  if (!outfile) {
    printf("Error opening output file: %s\n", sf_strerror(NULL));
    free(signal_buffer);
    free(impulse_buffer);
    free(full_output_buffer);
    free(final_output_buffer);
    exit(1);
  }

  // Write final buffer
  sf_count_t frames_written = sf_writef_float(outfile, final_output_buffer, final_output_frames);
  if (frames_written < 0) {
    printf("Error writing file: %s\n", sf_strerror(outfile));
  }

  // Close files and clean memory
  sf_close(signal_file);
  sf_close(impulse_file);
  sf_close(outfile);
  free(signal_buffer);
  free(impulse_buffer);
  free(full_output_buffer);
  free(final_output_buffer);
}
