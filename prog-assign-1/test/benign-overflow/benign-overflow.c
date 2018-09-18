// This test should not produce any warning (as overflown value is not
// impacting malloc).

// RUN: %cc %intcheck_cflags %s %rtsrc -o %t.after
// RUN: %t.after 10 10| %filecheck %s --check-prefix=NOOVERFLOW
// RUN: %t.after 100 10| %filecheck %s --check-prefix=NOOVERFLOW
// RUN: %t.after 300 400| %filecheck %s --check-prefix=NOOVERFLOW
// RUN: %t.after 1000000 1000000| %filecheck %s --check-prefix=OVERFLOW
// RUN: %t.after 100000 1000000| %filecheck %s --check-prefix=OVERFLOW

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char** argv) {
  printf("begin main()\n");
  // OVERFLOW: begin main()
  // NOOVERFLOW: begin main()

  uint x = atoi(argv[1]);
  uint y = atoi(argv[2]);
  uint z = x*y;

  // This malloc() is NOT vulnerable.
  char *p = malloc(x);

  printf("end main()\n");
  // OVERFLOW-NEXT: end main()
  // NOOVERFLOW-NEXT: end main()
  return 0;
}
