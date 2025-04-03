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

// Usage channel number - e.g. Left:1
void
HRTF (int suffix);

void
*thread_function(void* arg) {
    int suffix = *(int*)arg;
    HRTF(suffix);
    return NULL;
}

int
main (int argc __attribute__((unused)),
      char *argv[] __attribute__((unused)))
{
  printf ("Welcome!\n");
  pthread_t threads[6];
  int suffixes[6];

  // Crear hilos para llamar a HRTF del 1 al 6
  for (int i = 0; i < 6; i++) {
    suffixes[i] = i + 1; // Sufijos del 1 al 6
    if (pthread_create(&threads[i], NULL, thread_function, &suffixes[i]) != 0) {
      perror("Failed to create thread");
      return EXIT_FAILURE;
    }
  }

  // Esperar a que todos los hilos terminen
  for (int i = 0; i < 6; i++) {
    pthread_join(threads[i], NULL);
  }

  return EXIT_SUCCESS;
}
