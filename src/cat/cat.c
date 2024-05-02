#include "cat.h"

#include <stdio.h>

void print_non_printable(char c) {
  if (c < -96) {
    printf("M-^%c", c + 192);
  } else if (c < 0) {
    printf("M-%c", c + 128);
  } else if (c == 9 || c == 10) {
    printf("%c", c);
  } else if (c < 32) {
    printf("^%c", c + 64);
  } else if (c < 127) {
    printf("%c", c);
  } else {
    printf("^?");
  }
}

void pre_main(int argc, char **argv) {
  struct option long_options[] = {{"number-nonblank", no_argument, 0, 'b'},
                                  {"number", no_argument, 0, 'n'},
                                  {"squeeze-blank", no_argument, 0, 's'},
                                  {0, 0, 0, 0}};
  int c;
  opt_t args = {false, false, false, false, false, false};

  while ((c = getopt_long(argc, argv, "eEvtTnvsb", long_options, NULL)) != -1) {
    switch (c) {
      case 'E':
        args.flag_e = true;
        break;
      case 'e':
        args.flag_e = true;
        args.flag_v = true;
        break;
      case 'b':
        args.flag_b = true;
        break;
      case 's':
        args.flag_s = true;
        break;
      case 'T':
        args.flag_t = true;
        break;
      case 't':
        args.flag_v = true;
        args.flag_t = true;
        break;
      case 'n':
        args.flag_n = true;
        break;
      case 'v':
        args.flag_v = true;
        break;
    }
  }

  while (optind < argc) {
    cat(argv[optind++], args);
  }
}

void cat_arg_b(char c, int *count, opt_t args) {
  if (!args.flag_b || c == '\n') {
    return;
  }
  *count = *count + 1;
  if ((c < 127 && c > 31) || (c == 9 || c == 10)) printf("% 6d\t", *count);
}

void cat_arg_n(int *count, opt_t args) {
  if (args.flag_n && !args.flag_b) {
    *count = *count + 1;
    printf("% 6d\t", *count);
  }
}

void cat(char *filename, opt_t args) {
  char c, prev = '.';
  int line = 0, c_spaces = 0, f_line = true, s_line = false;
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    printf("cat: %s: No such file or directory", filename);
    return;
  }
  while ((c = fgetc(file)) != EOF) {
    if (f_line) {
      cat_arg_b(c, &line, args);
      cat_arg_n(&line, args);
    }
    if (s_line && c == '\n' && args.flag_s) {
      while ((c = fgetc(file)) == '\n') {
      }
      s_line = false;
    }
    if (c == '\t' && args.flag_t) {
      printf("^I");
      if (f_line) {
        f_line = false;
        s_line = true;
      }
      continue;
    }
    if (c == '\n')
      c_spaces++;
    else
      c_spaces = 0;
    if ((c_spaces > 2) && args.flag_s)
      while ((c = fgetc(file)) == '\n')
        ;
    if (!args.flag_b && prev == '\n') {
      cat_arg_n(&line, args);
      prev = c;
    }
    if (c == '\n')
      if (args.flag_e) printf("%c", '$');
    if (prev == '\n' && args.flag_b) {
      cat_arg_b(c, &line, args);
      prev = c;
    }
    if (args.flag_v) {
      print_non_printable(c);
    } else {
      if ((c < 127 && c > 31) || (c == 9 || c == 10)) printf("%c", c);
    }
    prev = c;
    if (f_line) {
      f_line = false;
      s_line = true;
    }
  }
  fclose(file);
}