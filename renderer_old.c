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

int projection_offset[2] = {20, 30};
int projection_scale = 2;

int render_offset_position[3] = {0};
float render_offset_rotation[3] = {0}; 


void _rotation_matrix_x(float* output_matrix, float theta) { //theta is in radians
  float _matrix[3][3] = {{1, 0, 0},
                        {0, cos(theta), -sin(theta)},
                        {0, sin(theta), cos(theta)}};
  memcpy(output_matrix, _matrix, sizeof _matrix);
} 

void _rotation_matrix_y(float* output_matrix, float theta) {
  float _matrix[3][3] = {{cos(theta), 0, sin(theta)},
                        {0, 1, 0},
                        {-sin(theta), 0, cos(theta)}};
  memcpy(output_matrix, _matrix, sizeof _matrix);
} 

void _rotation_matrix_z(float* output_matrix, float theta) {
  float _matrix[3][3] = {{cos(theta), -sin(theta), 0},
                        {sin(theta), cos(theta), 0},
                        {0, 0, 1}};
  memcpy(output_matrix, _matrix, sizeof _matrix);
} 

void _matmult(const float vec[3], const float mat[3][3], float result[3]) {
  for (int i = 0; i < 3; i++) {
    result[i] = 0.0f;
    for (int j = 0; j < 3; j++) {
      result[i] += vec[j] * mat[j][i]; // Vector is treated as a row vector
    }
  }
}
void *renderer_viewport_setup() {  //DANGER: memory freeing needed
  // todo add pixel data color support
  // void *viewport = malloc(sizeof(char) * SCREEN_HEIGTH * SCREEN_WIDTH * Z_PIXEL_BUFFER_LENGTH);
  int viewport_allocation_size = sizeof(char) * SCREEN_HEIGTH * SCREEN_WIDTH;
  void *viewport = malloc(viewport_allocation_size);
  
  return viewport;
}

void _clear_viewport(void *viewport) {
  for (unsigned int i = 0; i < sizeof(char) * SCREEN_HEIGTH * SCREEN_WIDTH; i++) {
    ((char*)viewport)[i] = RENDERER_FILLER_CHAR;
  }
}

void renderer_draw_viewport(void *viewport, FILE* stream) { //TODO: byl jsem linej kdyz jsem toto psal
  for (int y = 0; y < SCREEN_WIDTH; y++) {
    for (int x = 0; x < SCREEN_HEIGTH; x++) {
      fprintf(stream, "%c ", ((char*)viewport)[(y * SCREEN_HEIGTH) + x]);
  }
    fprintf(stream, "\n");
  }

}

/*void _render_line(void *viewport, int a[2], int b[2]) {
  // Bresenhamuv algoritmus rasterizace car: https://classic.csunplugged.org/documents/activities/community-activities/line-drawing/line-drawing.pdf
  int x0; int x1; int y0; int y1;
  if (a[0] > b[0]) {
    x0 = b[0]; x1 = a[0]; y0 = b[1]; y1 = a[1];
  } else {
    x0 = a[0]; x1 = b[0]; y0 = a[1]; y1 = b[1];
  }

  int dx = x1 - x0;

  int dy; int y;
  if(y1 > y0) {
    dy = y1 - y0;
    y = y0;
  } else {
    dy = y0 - y1;
    y = y1;
  }
 
  int D = 2*dy - dx;

  for (int x = x0; x < x1; x++) {
    ((char*)viewport)[(x * sizeof(char) * SCREEN_HEIGTH) + (y * sizeof(char))] = RENDERER_VERTEX_CHAR;
    if (D > 0) {
      y = y + 1;
      D = D - 2*dx;
    }
    D = D + 2*dy;
  }
}*/

void _render_line(void *viewport, int a[2], int b[2]) {
    int x0 = a[0], y0 = a[1];
    int x1 = b[0], y1 = b[1];

    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    while (1) {
        ((char*)viewport)[(x0 * sizeof(char) * SCREEN_HEIGTH) + (y0 * sizeof(char))] = RENDERER_VERTEX_CHAR;

        if (x0 == x1 && y0 == y1) break;

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }

        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
    }
}


void _render_verticies(void *viewport, int data[][2], int data_buffer_length) {
  for (int i = 0; i < data_buffer_length; i++) { 
    if (data[i][0] != -1) {
      printf("%i: %i %i\n", i, data[i][0], data[i][1]);
      ((char*)viewport)[(data[i][0] * sizeof(char) * SCREEN_HEIGTH) + (data[i][1] * sizeof(char))] = RENDERER_VERTEX_CHAR;
    }
  }
}

void renderer_vertex_pipeline(int *loaded_vertex_data, int loaded_data_length, void *viewport, char TMP_render_lines) {
  _clear_viewport(viewport);

  float vertex_buffer[3];
  float vertex_buffer_rotated[3];
  int projected_coordinates[2];


  float rot_x[3][3];
  _rotation_matrix_x(*rot_x, render_offset_rotation[0]);
  float rot_y[3][3];
  _rotation_matrix_y(*rot_y, render_offset_rotation[1]);
  float rot_z[3][3];
  _rotation_matrix_z(*rot_z, render_offset_rotation[2]);

  int (*rendered_verticies)[2] = malloc(loaded_data_length * 2 * sizeof(int));
  if(rendered_verticies == NULL) { fprintf(stderr, "Could not allocate %i bytes for rendered_verticies_container in render_vertex_pipeline (renderer.c)\n", loaded_data_length * 2 * (int)sizeof(int)); }

  for (int i = 0; i < loaded_data_length; i++) { // iteruje skrze vsechny vertexy
    for (int xyz = 0; xyz < 3; xyz++) {
      vertex_buffer[xyz] = loaded_vertex_data[3 * i + xyz] + render_offset_position[xyz];    // nacte vertex do docasneho bufferu a aplikuje offset pozice
    }

    // TODO: opravit tenhle hack
    _matmult(vertex_buffer, rot_x, vertex_buffer_rotated);                        // aplikuje offset rotace
    _matmult(vertex_buffer_rotated, rot_y, vertex_buffer);
    _matmult(vertex_buffer, rot_z, vertex_buffer_rotated);

    // output_data[2 * i + 0] = (data_buffer_rotated[0] / data_buffer_rotated[2]) * projection_scale;
    // output_data[2 * i + 1] = (data_buffer_rotated[1] / data_buffer_rotated[2]) * projection_scale;
    
    projected_coordinates[0] = ((vertex_buffer_rotated[0] / vertex_buffer_rotated[2]) * projection_scale) + projection_offset[0];
    projected_coordinates[1] = ((vertex_buffer_rotated[1] / vertex_buffer_rotated[2]) * projection_scale) + projection_offset[1];

    printf("pipeline: %i %i\n", projected_coordinates[0], projected_coordinates[1]);
    if (0 <= projected_coordinates[0] && projected_coordinates[0] < SCREEN_WIDTH && 0 <= projected_coordinates[1] && projected_coordinates[1] < SCREEN_HEIGTH) {
      rendered_verticies[i][0] = projected_coordinates[0];
      rendered_verticies[i][1] = projected_coordinates[1];
    } else {
      rendered_verticies[i][0] = -1; rendered_verticies[i][1] = -1;
    }
  }

  printf("%i %i\n", (&(loaded_vertex_data[0]))[0], (&(loaded_vertex_data[1]))[1]);
  if (TMP_render_lines == 1) {
    _render_line(viewport, rendered_verticies[0], rendered_verticies[1]);
    _render_line(viewport, rendered_verticies[2], rendered_verticies[3]);
  }

  _render_verticies(viewport, rendered_verticies, loaded_data_length);

  free(rendered_verticies);
  
  printf("loaded data: %i \n", loaded_data_length);
  
}
