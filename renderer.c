#include "renderer.h"

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef MATH_H
#define MATH_H
#include <math.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif

// ------------------------------------
// Matrix math
// ------------------------------------

void _matmult(const float vec[3], const float mat[3][3], float result[3]) {
  for (int i = 0; i < 3; i++) {
    result[i] = 0.0f;
    for (int j = 0; j < 3; j++) {
      result[i] += vec[j] * mat[j][i]; // Vector is treated as a row vector
    }
  }
}


// ------------------------------------
// Viewport magic
// ------------------------------------

struct _viewport_vertex {
  char rendered_char;
  // int 3D_position[2];
  int distance;
};

long int viewport_allocation_size; 

void *renderer_viewport_setup(void) {  //DANGER: memory freeing needed
  viewport_allocation_size = SCREEN_HEIGTH * SCREEN_WIDTH * Z_BUFFER_LEGTH * sizeof(struct _viewport_vertex);
  void *viewport = malloc(viewport_allocation_size);
  
  return viewport;
}

void renderer_draw_viewport(void *viewport, FILE* stream) { //TODO: byl jsem linej kdyz jsem toto psal
  for (int y = 0; y < SCREEN_HEIGTH; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      fprintf(stream, "%c ", ((char*)viewport)[(y * SCREEN_HEIGTH * sizeof(char)) + (x * sizeof(char))]);
    }
    fprintf(stream, "\n");
  }
}

void _clear_viewport(void *viewport) {
  for (unsigned int i = 0; i < viewport_allocation_size; i++) {
    ((char*)viewport)[i] = RENDERER_FILLER_CHAR;
  }
}

void _viewport_add_vertex(void *viewport, int vertex[2], int distance) {
  printf("test: %i %i\n", vertex[1], vertex[0]);
  struct _viewport_vertex;
  _viewport_vertex.distance = distance;
  _viewport_vertex.rendered_char = RENDERER_VERTEX_CHAR;

  // ((char*)viewport)[(vertex[1] SCREEN_HEIGTH * sizeof(struct _viewport_vertex)) + (vertex[0] * sizeof(char))] = RENDERER_VERTEX_CHAR;
  // memcpy(viewport + ((vertex[1] * SCREEN_HEIGTH * Z_BUFFER_LEGTH * sizeof(struct _viewport_vertex)) + (vertex[0] * Z_BUFFER_LEGTH)));

}


// ------------------------------------
// Testing driver code
// ------------------------------------


int main(void)
{
  void *viewport = renderer_viewport_setup();
  _clear_viewport(viewport);
  int test_vertex[2] = {3, 4};
  _viewport_add_vertex(viewport, test_vertex);
  int test_vertex2[2] = {7, 8};
  _viewport_add_vertex(viewport, test_vertex2);

  renderer_draw_viewport(viewport, stdout);

  free(viewport);

  return 1;
}
