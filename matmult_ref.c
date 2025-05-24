#include <stdio.h>
#include <math.h>
#include <string.h>


/*void multiplyMatrix(int m1[R1 1][C1 3], int m2[R2 3][C2 3])
{
    int result[R1][C2];

    printf("Resultant Matrix is:\n");

    for (int i = 0; i < R1; i++) {
        for (int j = 0; j < C2; j++) {
            result[i][j] = 0;

            for (int k = 0; k < R2; k++) {
                result[i][j] += m1[i][k] * m2[k][j];
            }

            printf("%d\t", result[i][j]);
        }

        printf("\n");
    }
}*/

void multiplyMatrix(int m1[3], float m2[9])
{
    float result[3];

    printf("Resultant Matrix is:\n");

    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 3; j++) {
            result[i * j] = 0;

            for (int k = 0; k < 3; k++) {
                result[i * j] += m1[i * k] * m2[k * j];
            }

            printf("%f\t", result[i * j]);
        }

        printf("\n");
    }
}

void matmult(const int vec[3], const float mat[3][3], float result[3]) {
    for (int i = 0; i < 3; i++) {
        result[i] = 0.0f;
        for (int j = 0; j < 3; j++) {
            result[i] += vec[j] * mat[j][i]; // Note: Vector is treated as a row vector
      //
        }
    }
}


void rotation_matrix_x(float* output_matrix, const float theta) {
  float _matrix[3][3] = {{1, 0, 0},
                         {0, cos(theta), -sin(theta)},
                         {0, sin(theta), cos(theta)}};
  memcpy(output_matrix, _matrix, sizeof _matrix);
} 

int main()
{
  int mat[3] = {1, 2, 3};

  float rot_x[3][3];

  rotation_matrix_x(*rot_x, 1.0);

  float result[3];

  matmult(mat, rot_x, result);
  
  for (int i = 0; i < 9; i++) {
    printf("%f ", rot_x[i]);
  }
  printf("\n");

  fprintf(stdout, "%f %f %f\n", result[0], result[1], result[2]);
}
