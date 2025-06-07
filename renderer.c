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

int projection_offset[2] = {20, 20};
int projection_scale = 3;

int render_offset_position[3] = {0};
float render_offset_rotation[3] = {0}; 


void rotation_matrix_x(float* output_matrix, float theta) { //theta is in radians
  float _matrix[3][3] = {{1, 0, 0},
                        {0, cos(theta), -sin(theta)},
                        {0, sin(theta), cos(theta)}};
  memcpy(output_matrix, _matrix, sizeof _matrix);
} 

void rotation_matrix_y(float* output_matrix, float theta) {
  float _matrix[3][3] = {{cos(theta), 0, sin(theta)},
                        {0, 1, 0},
                        {-sin(theta), 0, cos(theta)}};
  memcpy(output_matrix, _matrix, sizeof _matrix);
} 

void rotation_matrix_z(float* output_matrix, float theta) {
  float _matrix[3][3] = {{cos(theta), -sin(theta), 0},
                        {sin(theta), cos(theta), 0},
                        {0, 0, 1}};
  memcpy(output_matrix, _matrix, sizeof _matrix);
} 

void matmult(const float vec[3], const float mat[3][3], float result[3]) {
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

void clear_viewport(void *viewport) {
  for (unsigned int i = 0; i < sizeof(char) * SCREEN_HEIGTH * SCREEN_WIDTH; i++) {
    ((char*)viewport)[i] = ' ';
  }
}

void renderer_draw(void *viewport, FILE* stream) { //TODO: byl jsem linej kdyz jsem toto psal
  /*char row[SCREEN_WIDTH * 2 + 1] = {0};
  for (int y = 0; y < SCREEN_HEIGTH; y++) {
    for (int i = 0; i < SCREEN_WIDTH; i++) { row[i*2] = ' '; } // filler znak prijde zde
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      // printf(" tested x: %i %i tested y: %i %i\n", vertex_data[2 * i + 0], x, vertex_data[2 * i + 1], y);
      if ((((char*)viewport)[(y * SCREEN_WIDTH) + x] == 'X')) {
        row[x*2] = 'x';
      }
      row[x*2 + 1] = ' ';
    }
    fprintf(stream, "%s\n", row);
  }
  
  char row[(SCREEN_WIDTH * 2) + 1 + 2];

  row[(SCREEN_WIDTH * 2) + 1 + 0] = '\';
  row[(SCREEN_WIDTH * 2) + 1 + 1] = 'n';

  for (int y = 0; y < SCREEN_HEIGTH; y++) {
    for (int i = 0; i < SCREEN_WIDTH * 2 + 1; i++) { row[i] = ' '; } // clear row
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      row[x*2] = ((char*)viewport)[(y * SCREEN_WIDTH) + x];
    }
  }*/

  for (int y = 0; y < SCREEN_HEIGTH; y++) {
    for (int x = 0; x < SCREEN_HEIGTH; x++) {
      fprintf(stream, "%c ", ((char*)viewport)[(y * SCREEN_WIDTH) + x]);
    }
    fprintf(stream, "\n");
  }

}

void renderer_vertex_pipeline(int *loaded_vertex_data, int loaded_data_length, void *viewport) {
  clear_viewport(viewport);

  float vertex_buffer[3];
  float vertex_buffer_rotated[3];
  int projected_coordinates[2];


  float rot_x[3][3];
  rotation_matrix_x(*rot_x, render_offset_rotation[0]);
  float rot_y[3][3];
  rotation_matrix_y(*rot_y, render_offset_rotation[1]);
  float rot_z[3][3];
  rotation_matrix_z(*rot_z, render_offset_rotation[2]);

  for (int i = 0; i < loaded_data_length; i++) { // iteruje skrze vsechny vertexy
    for (int xyz = 0; xyz < 3; xyz++) {
      vertex_buffer[xyz] = loaded_vertex_data[3 * i + xyz] + render_offset_position[xyz];    // nacte vertex do docasneho bufferu a aplikuje offset pozice
    }

    // TODO: opravit tenhle hack hack
    matmult(vertex_buffer, rot_x, vertex_buffer_rotated);                        // aplikuje offset rotace
    matmult(vertex_buffer_rotated, rot_y, vertex_buffer);
    matmult(vertex_buffer, rot_z, vertex_buffer_rotated);

    // output_data[2 * i + 0] = (data_buffer_rotated[0] / data_buffer_rotated[2]) * projection_scale;
    // output_data[2 * i + 1] = (data_buffer_rotated[1] / data_buffer_rotated[2]) * projection_scale;
    
    projected_coordinates[0] = ((vertex_buffer_rotated[0] / vertex_buffer_rotated[2]) * projection_scale) + projection_offset[0];
    projected_coordinates[1] = ((vertex_buffer_rotated[1] / vertex_buffer_rotated[2]) * projection_scale) + projection_offset[1];
    // printf("pipeline: %i %i\n", projected_coordinates[0], projected_coordinates[1]);
    if (0 <= projected_coordinates[0] && projected_coordinates[0] < SCREEN_HEIGTH && 0 <= projected_coordinates[1] && projected_coordinates[1] < SCREEN_WIDTH) {
      ((char*)viewport)[(projected_coordinates[0] * sizeof(char) * SCREEN_HEIGTH) + (projected_coordinates[1] * sizeof(char))] = 'X';
    }    

  }
}
