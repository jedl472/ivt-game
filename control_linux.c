#include "control_linux.h"

#ifndef UNISTD_H
#define UNISTD_H 
#include <unistd.h>
#endif

#ifndef TERMIOS_H
#define TERMIOS_H
#include <termios.h>
#endif

void enable_raw_mode(struct termios *orig_termios) {
  struct termios raw;

  // Get current terminal attributesn
  tcgetattr(STDIN_FILENO, orig_termios);

  raw = *orig_termios;

  // Disable canonical mode and echo
  raw.c_lflag &= ~(ICANON | ECHO);

  // Apply the new attributes
  tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disable_raw_mode(const struct termios *orig_termios) {
  // Restore original terminal attributes
  tcsetattr(STDIN_FILENO, TCSANOW, orig_termios);
}

void update_key_buffer(char buffer[KEYBOARD_BUFFER_LENGTH]) { 
  struct termios orig_termios;
  char ch;

  enable_raw_mode(&orig_termios);
  if (read(STDIN_FILENO, &ch, 1) == 1) {
    /*if (ch == 27) { // ESC key
              printf("\nESC pressed. Exiting...\n");
              break;
          }

          if (ch >= 32 && ch <= 126) {
              printf("You pressed: '%c'\n", ch);
          } else {
              printf("You pressed a control key (ASCII code: %d)\n", ch);
          }*/

    for (int i = 0; i < KEYBOARD_BUFFER_LENGTH; i++) {
      if (buffer[i] == 0) {
        buffer[i] = ch;
        break;
      }
    }
  }

  disable_raw_mode(&orig_termios);
}

