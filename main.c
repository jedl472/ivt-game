#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef STDLIB_H
#define STDLIB_H
#include <stdlib.h>
#endif

#ifndef STRING_H
#define STRING_H
#include <string.h>
#endif



#include "control_linux.h" //TODO urgent: pridat win podporu
#include "renderer.h"

#define VERTEX_FILE "cube.txt"
#define OUT_FILE "rendered.txt"


int *load_vertex_from_file(int *buffer_size) {
  int data_index = 0;
  int data_size = 5;
  void *data;

  int vertex_value[3];

  FILE *file = fopen(VERTEX_FILE, "r");
  if (file == NULL) { 
    fprintf(stderr, "Failed to open file.\n");
    return NULL;
  }
  data = malloc(data_size * sizeof vertex_value);


  if(data == NULL) {
    fprintf(stderr, "Failed to allocate initial buffer memory.\n");
    return NULL;
  }

  while (fscanf(file, "%i %i %i", &(vertex_value[0]), &(vertex_value[1]), &(vertex_value[2])) != EOF) {
    if (data_index == data_size - 1) {
      data_size *= 2;
      int *new_data = realloc(data, data_size * sizeof vertex_value);

      // fprintf(stdout, "Realocating memory\n");
      if(new_data == NULL) {
        fprintf(stderr, "Failed to reallocate temporary file buffer memory.\n");
        free(data);
        return NULL;
      }
      data = new_data;
    }

    memcpy(data + (data_index * sizeof vertex_value), vertex_value, sizeof vertex_value); 

    data_index++;
  }

  *buffer_size = data_index;

  data = realloc(data, data_index * sizeof(vertex_value));
  if (data == NULL) {
    fprintf(stderr, "Failed to reallocate file buffer memory.\n");
    free(data);
  }
  return data;
}

int main(void)
{
  printf("starting program\n");
  int raw_data_buffer_size = 0;
  int *raw_data = load_vertex_from_file(&raw_data_buffer_size);
  printf("Size of allocated buffer: %i\n", raw_data_buffer_size);

  void* viewport = renderer_viewport_setup();
  if (viewport == NULL) { fprintf(stderr, "Failed to allocate memory for viewport.\n"); }

  char key_buffer[KEYBOARD_BUFFER_LENGTH] = {0};

  while (1) {
    update_key_buffer(key_buffer);

    if (key_buffer[0] != 0) {
      switch (key_buffer[0]) {
        case 'a':
          render_offset_rotation[1] += 0.1;
          break;
        case 'd':
          render_offset_rotation[1] -= 0.1;
          break;

        case 'w':
          render_offset_rotation[0] -= 0.1;
          break;
        case 's':
          render_offset_rotation[0] += 0.1;
          break;
      }
      key_buffer[0] = 0;

      // renderer_vertex_pipeline(raw_data, raw_data_buffer_size, viewport);
      int a[2] = {1, 2};
      int b[2] = {20, 20};
      _render_line(viewport, b, a);

      printf("\e[1;1H\e[2J"); // pouze linux, zbavit se co nejdrive to pujde
      renderer_draw_viewport(viewport, stdout);
    }
  }


  free(viewport);
  free(raw_data); 
  return 0;
}
