/* main.c
 *
 * Copyright 2025 Daniel Mendoza
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <time.h>
#include <spawn.h>
#include <sys/wait.h>
#include <sys/stat.h>

extern char **environ;

// Usage channel number - e.g. Left:1
void
HRTF (int suffix);

void
*thread_function(void* arg) {
    int suffix = *(int*)arg;
    HRTF(suffix);
    return NULL;
}

void
run_process(const char *path, char *const argv[]) {
    pid_t pid;
    int status;

    if (posix_spawn(&pid, path, NULL, NULL, argv, environ) != 0) {
        perror("posix_spawn failed");
        exit(EXIT_FAILURE);
    }

    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid failed");
        exit(EXIT_FAILURE);
    }

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Process %s failed\n", path);
        exit(EXIT_FAILURE);
    }
}

int
main (int argc __attribute__((unused)),
      char *argv[] __attribute__((unused)))
{
    printf("Welcome!\n");

    // MagicSplit
    char *split_args[] = {"./MagicSplit", "input.wav", NULL};
    run_process("./MagicSplit", split_args);

    // MagicHeadphone
    pthread_t threads[6];
    int suffixes[6];

    for (int i = 0; i < 6; i++) {
        suffixes[i] = i + 1;
        if (pthread_create(&threads[i], NULL, thread_function, &suffixes[i]) != 0) {
            perror("Failed to create thread");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < 6; i++) {
        pthread_join(threads[i], NULL);
    }

    // MagicMix
    char *mix_args[9];
    mix_args[0] = "./MagicMix";
    mix_args[1] = "final-output.wav";

    for (int i = 0; i < 6; i++) {
        char *name = malloc(32);
        snprintf(name, 32, "output-%d.wav", i + 1);
        mix_args[i + 2] = name;
    }
    mix_args[8] = NULL;

    run_process("./MagicMix", mix_args);
    
    for (int i = 0; i < 6; i++) {
        free(mix_args[i + 2]);
    }

    printf("Your file is ready.\n");

    return EXIT_SUCCESS;
}

