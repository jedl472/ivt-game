#define KEYBOARD_BUFFER_LENGTH 8

#ifndef CONTROL_LINUX_H
#define CONTROL_LINUX_H

// void control_loop_invoker(void (*invoked_function)(char));
void update_key_buffer(char buffer[KEYBOARD_BUFFER_LENGTH]); 

#endif
