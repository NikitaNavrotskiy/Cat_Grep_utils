#include "grep.h"

opt_t opt_get(int argc, char **argv) {
  opt_t args = {false, false, false, false, false, false,
                false, false, false, false, NULL,  NULL};
  int c;
  char *options = "e:ivclnhsf:o";

  while ((c = getopt(argc, argv, options)) != -1) {
    switch (c) {
      case 'e':
        args.flag_e = true;
        args.arg_e = optarg;
        break;
      case 'i':
        args.flag_i = true;
        break;
      case 'v':
        args.flag_v = true;
        break;
      case 'c':
        args.flag_c = true;
        break;
      case 'l':
        args.flag_l = true;
        break;
      case 'n':
        args.flag_n = true;
        break;
      case 'h':
        args.flag_h = true;
        break;
      case 's':
        args.flag_s = true;
        break;
      case 'f':
        args.flag_f = true;
        args.arg_f = optarg;
        break;
      case 'o':
        args.flag_o = true;
        break;
    }
  }
  return args;
}

void grep(char *filename, char *pattern, opt_t args) {
  regex_t regex;
  FILE *file;
  FILE *f_file;
  int res = 0;
  int stop = 0;
  if (pattern != NULL) {
    if (args.flag_i)
      res = regcomp(&regex, pattern, REG_ICASE | REG_EXTENDED);
    else
      res = regcomp(&regex, pattern, REG_EXTENDED);
    if (res) {
      if (!args.flag_s) fprintf(stderr, "Failed to compile regex\n");
      stop = 1;
    }
  }
  file = fopen(filename, "r");
  if (file == NULL) {
    if (!args.flag_s)
      fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    stop = 1;
  }

  if (args.flag_f) {
    f_file = fopen(args.arg_f, "r");
    if (f_file == NULL) {
      if (!args.flag_s)
        fprintf(stderr, "grep: %s: No such file or directory\n", args.arg_f);
      stop = 1;
    }
  }
  if (!stop) {
    print_matches(regex, file, args, filename, f_file);
    fclose(file);
  }
  if (pattern != NULL) regfree(&regex);
}

void print_matches(regex_t regex, FILE *file, opt_t args, char *filename,
                   FILE *filef) {
  int n_line = 0;
  int count = 0;
  int res;
  size_t buff = 0;
  char *line = NULL;
  while ((Getline(&line, &buff, file)) != -1) {
    n_line++;
    int print = 0;
    if (args.flag_f) {
      print = grep_flag_f(args, line, filef, n_line, &count, filename, regex);
      fclose(filef);
      filef = fopen(args.arg_f, "r");
    } else if (args.flag_o) {
      print = grep_flag_o(&regex, line, args, n_line, &count, filename);
      if (print && args.flag_l) {
        printf("%s\n", filename);
        break;
      }
    } else {
      res = regexec(&regex, line, 0, NULL, 0);
      print = need_print(res, args);
    }
    if (!args.flag_o) {
      if (!args.flag_h && print && !args.flag_c) printf("%s:", filename);
      if (print == 1 && args.flag_c && !args.flag_l)
        count++;
      else if (print && args.flag_l) {
        printf("%s\n", filename);
        if (line != NULL) free(line);
        return;
      } else if (!print && args.flag_l) {
        continue;
      } else if (print && args.flag_n)
        printf("%d:%s", n_line, line);
      else if (print)
        printf("%s", line);
    }
  }
  if (args.flag_c && !args.flag_l) {
    if (args.flag_h)
      printf("%d\n", count);
    else
      printf("%s:%d\n", filename, count);
  }
  if (line != NULL) free(line);
  if (args.flag_f) {
    fclose(filef);
  }
}

int need_print(int res, opt_t args) {
  int print = 0;
  if (args.flag_v && res == REG_NOMATCH)
    print = 1;

  else if (args.flag_v && !res)
    print = 0;

  else if (!res)
    print = 1;

  else if (res == REG_NOMATCH)
    print = 0;

  return print;
}

int check_file(char *filename) {
  int ret = 0;
  FILE *file;
  if (filename != NULL) {
    file = fopen(filename, "r");
    if (file != NULL) {
      ret = 1;
      fclose(file);
    }
  }
  return ret;
}

int grep_flag_f(opt_t args, char *line, FILE *file_f, int n_line, int *count,
                char *filename, regex_t regex) {
  size_t f_buff = 0;
  int res;
  char *line_f = NULL;
  int res_f;
  int print = 0;

  while (Getline(&line_f, &f_buff, file_f) != -1) {
    int len = strlen(line_f);
    for (int i = 0; i < len; i++) {
      if (line_f != NULL) {
        if (line_f[i] == '\n') line_f[i] = '\0';
      }
    }
    regex_t regex_f;
    if (args.flag_o) {
      print = grep_flag_o(&regex_f, line, args, n_line, count, filename);
    } else if (args.flag_i) {
      regcomp(&regex_f, line_f, REG_ICASE | REG_EXTENDED);
      res_f = regexec(&regex_f, line, 0, NULL, 0);
      print = need_print(res_f, args);
      regfree(&regex_f);
    } else {
      regcomp(&regex_f, line_f, REG_EXTENDED);
      res_f = regexec(&regex_f, line, 0, NULL, 0);
      print = need_print(res_f, args);
      regfree(&regex_f);
    }
    if (print) {
      break;
    }
    if (!print && args.flag_e && !args.flag_o) {
      res = regexec(&regex, line, 0, NULL, 0);
      print = need_print(res, args);
    }
  }

  free(line_f);

  return print;
}

int grep_flag_o(regex_t *regex, char *line, opt_t args, int n_line, int *count,
                char *filename) {
  size_t f_buff = 0;
  FILE *file_f = NULL;
  int print = 0;
  char *line_f = NULL;
  regmatch_t matches[1];

  if (args.flag_f) {
    file_f = fopen(args.arg_f, "r");
    if (file_f == NULL) {
      if (!args.flag_s)
        fprintf(stderr, "grep: %s: No such file or directory\n", filename);
    }
  }
  if (args.flag_f) {
    while (Getline(&line_f, &f_buff, file_f) != -1) {
      int len = strlen(line_f);
      for (int i = 0; i < len; i++) {
        if (line_f != NULL) {
          if (line_f[i] == '\n') line_f[i] = '\0';
        }
      }
      if (args.flag_i)
        regcomp(regex, line_f, REG_ICASE);
      else
        regcomp(regex, line_f, 0);

      char *line_cp = line;
      while (!regexec(regex, line_cp, 1, matches, 0)) {
        print = 1;
        print_o_flag(args, line_cp, print, filename, matches, n_line);
        line_cp += matches[0].rm_eo + 1;
      }
      regfree(regex);
    }
  } else {
    while (!regexec(regex, line, 1, matches, 0)) {
      print = 1;
      if (args.flag_l) return print;
      print_o_flag(args, line, print, filename, matches, n_line);
      line += matches[0].rm_eo + 1;
    }
  }
  if (print) *count = *count + 1;

  return print;
}

int Getline(char **line_ptr, size_t *n, FILE *stream) {
  size_t pos = 0;
  int c;

  if (*line_ptr == NULL) {
    *n = 128;
    *line_ptr = (char *)malloc(*n);
    if (*line_ptr == NULL) {
      return -1;
    }
  }

  while (1) {
    c = fgetc(stream);
    if (c == EOF) {
      if (pos == 0) {
        free(*line_ptr);
        *line_ptr = NULL;
      } else {
        (*line_ptr)[pos] = '\0';
      }
      return pos == 0 ? -1 : (int)pos;
    }

    if (pos >= *n - 1) {
      size_t new_size = *n * 2;
      char *new_line = (char *)realloc(*line_ptr, new_size);
      if (new_line == NULL) {
        return -1;
      }
      *line_ptr = new_line;
      *n = new_size;
    }

    (*line_ptr)[pos++] = c;

    if (c == '\n') {
      (*line_ptr)[pos] = '\0';
      return pos;
    }
  }
}

void print_o_flag(opt_t flags, char *line, int print, char *filename,
                  regmatch_t matches[1], int n_line) {
  if (!flags.flag_h && print && !flags.flag_c && !flags.flag_v)
    printf("%s:", filename);
  if (!flags.flag_c) {
    int start = matches[0].rm_so;
    int end = matches[0].rm_eo;
    if (flags.flag_n) printf("%d:", n_line);
    if (!flags.flag_v && !flags.flag_l) {
      for (int i = start; i < end; i++) {
        printf("%c", line[i]);
      }
      printf("\n");
    }
  }
}

void pre_main(int argc, char **argv) {
  opt_t args = opt_get(argc, argv);

  if (!args.flag_h && (optind + 1 == argc)) args.flag_h = true;

  if (args.flag_l) args.flag_h = true;

  if (args.flag_f && args.flag_e) {
    while (optind < argc) {
      if (optind + 1 < argc) args.flag_h = 0;
      grep(argv[optind++], args.arg_e, args);
    }
  } else if (args.flag_f && !args.flag_e) {
    while (optind < argc) {
      if (optind + 1 < argc) args.flag_h = 0;
      grep(argv[optind++], NULL, args);
    }
  } else if (args.flag_e) {
    while (optind < argc) {
      grep(argv[optind], args.arg_e, args);
      optind++;
    }
  } else {
    if ((argc == 3) || (optind + 2 == argc)) args.flag_h = true;
    char *pattern = argv[optind];
    while (optind < argc - 1) {
      grep(argv[optind + 1], pattern, args);
      optind++;
    }
  }
}