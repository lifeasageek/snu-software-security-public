#include <stdio.h>
#include <string.h>

#define MSG_HELO "> Started.\n"
#define MSG_VULN "> Vulnerability triggered.\n"
#define MSG_EXIT "> Exiting.\n"

#define BUFLEN 4

void crash() {
  // Intentionally trigger the crash to simulate a vulnerability.
  printf(MSG_VULN);

  int *y = NULL;
  *y = 0x1234;
}

int main(int argc, char *argv[]) {
  char c;
  FILE *file;
  char buf[BUFLEN];

  if (argc <= 1) {
    fprintf(stderr, "[-] ERROR: Not valid arguments\n");
    return -1;
  }

  printf(MSG_HELO);

  file = fopen(argv[1], "r");
  if (!file) {
    fprintf(stderr, "[-] ERROR: Failed to open a file [%s]\n", argv[1]);
    return -1;
  }

  for (int i=0; i<BUFLEN; i++) {
    c = getc(file);
    if (c == EOF)
      break;
    buf[i] = c;
  }

  fclose(file);

#ifdef COND_WEAK
  if (buf[0] == 0x54) {
    // Weak condition. Original AFL can easily solve.
    crash();
  }
#else
  unsigned int *x = (unsigned int *)buf;
  if (*x == 0x54534554) { // x == "TEST"
    // Strong condition. Original AFL cannot easily solve.
    crash();
  }
#endif

  printf(MSG_EXIT);

  return 0;
}
