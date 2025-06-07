#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#define SCREEN_WIDTH 50  // TODO akutne: fixnout velikost viewportu
#define SCREEN_HEIGTH 50

#ifndef RENDERER_H
#define RENDERER_H

extern int render_offset_position[3];
extern float render_offset_rotation[3]; 
 

void renderer_vertex_pipeline(int *loaded_vertex_data, int loaded_data_length, void *viewport);

void *renderer_viewport_setup();  //DANGER: memory freeing needed

void renderer_draw(void *viewport, FILE* stream);

#endif
