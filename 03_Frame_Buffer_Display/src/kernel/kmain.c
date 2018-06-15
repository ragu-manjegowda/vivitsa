#include <frame_buffer.h>

/* The C function */
int sum_of_three(int a, int b, int c) {
  char buffer[25] = "Ragu, You are awesome!!!";
  write(buffer, 25);
  return a + b + c;
}
