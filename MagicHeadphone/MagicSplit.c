#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

#define CHANNELS_IN 6
#define CHANNELS_OUT 2
#define BLOCK_SIZE 2048

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s input.wav\n", argv[0]);
        return 1;
    }

    const char *input_file = argv[1];
    SNDFILE *infile, *outfile;
    SF_INFO sfinfo_in, sfinfo_out;

    // Open input file
    sfinfo_in.format = 0;
    infile = sf_open(input_file, SFM_READ, &sfinfo_in);
    if (!infile) {
        printf("Error opening %s: %s\n", input_file, sf_strerror(NULL));
        return 1;
    }

    if (sfinfo_in.channels != CHANNELS_IN) {
        printf("Your file must have %d channels, but it has %d\n", CHANNELS_IN, sfinfo_in.channels);
        sf_close(infile);
        return 1;
    }

    // Configure stereo out
    sfinfo_out = sfinfo_in;
    sfinfo_out.channels = CHANNELS_OUT;

    float *buffer_in = malloc(BLOCK_SIZE * CHANNELS_IN * sizeof(float));
    float *buffer_out = malloc(BLOCK_SIZE * CHANNELS_OUT * sizeof(float));

    if (!buffer_in || !buffer_out) {
        printf("Error assigning memory\n");
        sf_close(infile);
        return 1;
    }

    // Procesar cada canal individualmente
    for (int ch = 0; ch < CHANNELS_IN; ch++) {
        char filename[64];
        snprintf(filename, sizeof(filename), "song-%d.wav", ch + 1);

        outfile = sf_open(filename, SFM_WRITE, &sfinfo_out);
        if (!outfile) {
            printf("Error creating %s: %s\n", filename, sf_strerror(NULL));
            free(buffer_in);
            free(buffer_out);
            sf_close(infile);
            return 1;
        }

        sf_seek(infile, 0, SEEK_SET); // Regresar al inicio del archivo

        sf_count_t frames;
        while ((frames = sf_readf_float(infile, buffer_in, BLOCK_SIZE)) > 0) {
            for (sf_count_t i = 0; i < frames; i++) {
                float sample = buffer_in[i * CHANNELS_IN + ch];
                buffer_out[i * 2] = sample;     // L
                buffer_out[i * 2 + 1] = sample; // R
            }
            sf_writef_float(outfile, buffer_out, frames);
        }

        sf_close(outfile);
        printf("Channel %d → %s\n", ch + 1, filename);
    }

    free(buffer_in);
    free(buffer_out);
    sf_close(infile);

    printf("Split Completed!\n");
    return 0;
}
