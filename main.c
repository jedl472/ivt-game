#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define VERTEX_FILE "cube.txt"
#define OUT_FILE "test.txt"

#define PROJECTION_SCALE 10

#define SCREEN_WIDTH 50
#define SCREEN_HEIGTH 20

void rotation_matrix_x(float* output_matrix, float theta) {
  float _matrix[3][3] = {{1, 0, 0},
                         {0, cos(theta), -sin(theta)},
                         {0, sin(theta), cos(theta)}};
  memcpy(output_matrix, _matrix, sizeof _matrix);
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

  float rot_x[9];
  rotation_matrix_x(rot_x, rotation[0]);
  
  for (int i = 0; i < data_size; i++) {
    // printf("data\n");

    data_buffer[0] = data[3 * i + 0];    // load 1 vertex into temporary buffer
    data_buffer[1] = data[3 * i + 1];
    data_buffer[2] = data[3 * i + 2];

    printf("Buffer: %f %f %f\n", data_buffer[0], data_buffer[1], data_buffer[2]);

    // TODO: magic matrix multiplication step

    output_data[2 * i + 0] = (data_buffer[0] / data_buffer[2]) * PROJECTION_SCALE;
    output_data[2 * i + 1] = (data_buffer[1] / data_buffer[2]) * PROJECTION_SCALE;


    printf("output data: %f %f\n", output_data[0], output_data[1]);
    
  }
}

void render_verticies(int* vertex_data, int data_size, FILE* stream) {
  char row[SCREEN_WIDTH * 2 + 1];
  for (int y = 0; y < SCREEN_HEIGTH; y++) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
      for (int i = 0; i < data_size; i++) {
        if (vertex_data[2 * i + 0] == x && vertex_data[2 * i + 1] == y) {
          row[x*2] = 'x';
        } else {
          row[x*2] = '.';
        }
        row[x*2 + 1] = ' ';
      } 
    }
    fprintf(stream, "%s\n", row);
  }
}

//TODO: render triangles

int main(void)
{
  int buffer_size = 0;
  int *raw_data = load_vertex_from_file(&buffer_size);
  printf("Size of allocated buffer: %i\n", buffer_size);

  for (int i = 0; i < buffer_size; i++) {
    printf("%i %i %i\n", raw_data[3 * i + 0], raw_data[3 * i + 1], raw_data[3 * i + 2]);
  }
 
  int* projected_data = malloc(buffer_size * sizeof(int[2]));
  if (projected_data == NULL) fprintf(stderr, "Failed to allocate memory for projected data.\n");
  float rotation[3] = {1, 0, 0};
  printf("Rotating and projecting... \n");
  rotate_and_project(projected_data, raw_data, buffer_size, rotation);

  for (int i = 0; i < buffer_size; i++) {
    printf("%i %i\n", projected_data[2 * i + 0], projected_data[2 * i + 1]);
  }

  printf("Rendering... \n");
  FILE *file = fopen(OUT_FILE, "w");
  render_verticies(projected_data, buffer_size, file);
  
  free(projected_data);
  free(raw_data); 
  return 0;
}
