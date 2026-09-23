#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>

#define CHANNELS_IN 6
#define BLOCK_SIZE 512

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    SF_INFO sfinfo;
    SNDFILE *infile = sf_open(argv[1], SFM_READ, &sfinfo);

    if (!infile || sfinfo.channels != CHANNELS_IN) {
        printf("Invalid file. %d Channels\n", sfinfo.channels);
        return 1;
    }

    SNDFILE *outfiles[CHANNELS_IN];
    SF_INFO outinfo = sfinfo;
    outinfo.channels = 2;

    // abrir todos los archivos
    for (int ch = 0; ch < CHANNELS_IN; ch++) {
        char name[64];
        snprintf(name, sizeof(name), "song-%d.wav", ch + 1);
        outfiles[ch] = sf_open(name, SFM_WRITE, &outinfo);
    }

    float *buffer_in = malloc(BLOCK_SIZE * CHANNELS_IN * sizeof(float));
    float *buffer_out = malloc(BLOCK_SIZE * 2 * sizeof(float));

    sf_count_t frames;

    while ((frames = sf_readf_float(infile, buffer_in, BLOCK_SIZE)) > 0) {

        for (int ch = 0; ch < CHANNELS_IN; ch++) {

            for (sf_count_t i = 0; i < frames; i++) {
                float sample = buffer_in[i * CHANNELS_IN + ch];
                buffer_out[i * 2] = sample;
                buffer_out[i * 2 + 1] = sample;
            }

            sf_writef_float(outfiles[ch], buffer_out, frames);
        }
    }

    for (int ch = 0; ch < CHANNELS_IN; ch++)
        sf_close(outfiles[ch]);

    sf_close(infile);
    free(buffer_in);
    free(buffer_out);

    // printf("Split Finished\n");
}
