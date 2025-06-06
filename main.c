#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "control_linux.h" //TODO urgent: pridat win podporu

#define VERTEX_FILE "cube.txt"
#define OUT_FILE "rendered.txt"

#define PROJECTION_SCALE 5
#define PROJECTION_OFFSET_XY -20


#define Z_PIXEL_BUFFER_LENGTH 8
#define SCREEN_WIDTH 50
#define SCREEN_HEIGTH 50


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
      result[i] += vec[j] * mat[j][i]; // Note: Vector is treated as a row vector
    }
  }
}

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

void rotate_and_project(int* output_data, int* data, int data_size, float rotation[3]) {
  float data_buffer[3];
  float data_buffer_rotated[3];

  float rot_x[3][3];
  rotation_matrix_x(*rot_x, rotation[0]);
  float rot_y[3][3];
  rotation_matrix_y(*rot_y, rotation[1]);
  float rot_z[3][3];
  rotation_matrix_z(*rot_z, rotation[2]);

  for (int i = 0; i < data_size; i++) {
    // printf("data\n");

    data_buffer[0] = data[3 * i + 0];    // load 1 vertex into temporary buffer
    data_buffer[1] = data[3 * i + 1];
    data_buffer[2] = data[3 * i + 2];

    // printf("Buffer: %f %f %f\n", data_buffer[0], data_buffer[1], data_buffer[2]);

    // TODO: Tohle je tak neuvěřitelně strašnej hack
    matmult(data_buffer, rot_x, data_buffer_rotated);
    matmult(data_buffer_rotated, rot_y, data_buffer);
    matmult(data_buffer, rot_z, data_buffer_rotated);

    output_data[2 * i + 0] = (data_buffer_rotated[0] / data_buffer_rotated[2]) * PROJECTION_SCALE;
    output_data[2 * i + 1] = (data_buffer_rotated[1] / data_buffer_rotated[2]) * PROJECTION_SCALE;


    // printf("output data: %f %f\n", output_data[0], output_data[1]);

  }
}

void render_verticies(int* vertex_data, int data_size, FILE* stream) {
  char row[SCREEN_WIDTH * 2 + 1] = {0};
  for (int y = 0; y < SCREEN_HEIGTH; y++) {
    for (int i = 0; i < SCREEN_WIDTH; i++) { row[i*2] = ' '; /*filler znak prijde zde*/ }
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      for (int i = 0; i < data_size; i++) {
        // printf(" tested x: %i %i tested y: %i %i\n", vertex_data[2 * i + 0], x, vertex_data[2 * i + 1], y);
        if (vertex_data[2 * i + 0] == (x + PROJECTION_OFFSET_XY) && vertex_data[2 * i + 1] == (y + PROJECTION_OFFSET_XY)) {
          row[x*2] = 'x';
        }
        row[x*2 + 1] = ' ';
      } 
    }
    fprintf(stream, "%s\n", row);
  }
}


void *viewport_setup() {  //DANGER: memory freeing needed
  // todo add pixel data color support
  void *viewport = malloc(sizeof(char) * SCREEN_HEIGTH * SCREEN_WIDTH * Z_PIXEL_BUFFER_LENGTH);

  return viewport;
} 

//TODO: render triangles


int main(void)
{
  printf("starting program\n");
  int buffer_size = 0;
  int *raw_data = load_vertex_from_file(&buffer_size);
  printf("Size of allocated buffer: %i\n", buffer_size);

  for (int i = 0; i < buffer_size; i++) {
    printf("%i %i %i\n", raw_data[3 * i + 0], raw_data[3 * i + 1], raw_data[3 * i + 2]);
  }

  /*int* projected_data = malloc(buffer_size * sizeof(int[2]));
  if (projected_data == NULL) fprintf(stderr, "Failed to allocate memory for projected data.\n");
  printf("Rotating and projecting... \n");
  rotate_and_project(projected_data, raw_data, buffer_size, rotation);

  for (int i = 0; i < buffer_size; i++) {
    printf("%i %i\n", projected_data[2 * i + 0], projected_data[2 * i + 1]);
  }

  printf("Rendering... \n");
  // FILE *file = fopen(OUT_FILE, "w");
  render_verticies(projected_data, buffer_size, stdout);

  free(projected_data);
  free(raw_data);*/ 



  char key_buffer[KEYBOARD_BUFFER_LENGTH] = {0};
  float rotation[3] = {0, 0, 0};

  while (true) {
    update_key_buffer(key_buffer);

    if (key_buffer[0] != 0) {
      switch (key_buffer[0]) {
        case 'a':
          rotation[1] += 0.1;
          break;
        case 'd':
          rotation[1] -= 0.1;
          break;

        case 'w':
          rotation[0] -= 0.1;
          break;
        case 's':
          rotation[0] += 0.1;
          break;
      }
      key_buffer[0] = 0;


      int* projected_data = malloc(buffer_size * sizeof(int[2]));
      if (projected_data == NULL) fprintf(stderr, "Failed to allocate memory for projected data.\n");
      // printf("Rotating and projecting... \n");
      rotate_and_project(projected_data, raw_data, buffer_size, rotation);

      /*for (int i = 0; i < buffer_size; i++) {
        printf("%i %i\n", projected_data[2 * i + 0], projected_data[2 * i + 1]);
      }*/

      // FILE *file = fopen(OUT_FILE, "w");
      printf("\e[1;1H\e[2J"); // pouze linux, zbavit se co nejdrive to pujde

      render_verticies(projected_data, buffer_size, stdout);

      free(projected_data);
    }
  }

  /*char key_buffer[KEYBOARD_BUFFER_LENGTH] = {0};
  while (true) {
    update_key_buffer(key_buffer);
    printf("buffer: %c %c %c\n", key_buffer[0], key_buffer[1], key_buffer[2]);
  }*/



  free(raw_data); 
  return 0;
}
