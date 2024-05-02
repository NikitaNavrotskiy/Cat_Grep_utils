#ifndef _GREP_H
#define _GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct opt {
  bool flag_e;
  char *arg_e;
  bool flag_i;
  bool flag_v;
  bool flag_c;
  bool flag_l;
  bool flag_n;
  bool flag_h;
  bool flag_s;
  bool flag_f;
  char *arg_f;
  bool flag_o;
} opt_t;

opt_t opt_get(int argc, char **argv);

void grep(char *filename, char *pattern, opt_t args);

void print_matches(regex_t regex, FILE *file, opt_t args, char *filename,
                   FILE *file_f);

int need_print(int res, opt_t args);

int check_file(char *filename);

int grep_flag_o(regex_t *regex, char *line, opt_t args, int n_line, int *count,
                char *filename);

void print_o_flag(opt_t args, char *line, int print, char *filename,
                  regmatch_t matches[1], int n_line);

int grep_flag_f(opt_t args, char *line, FILE *file_f, int n_line, int *count,
                char *filename, regex_t regex);

int Getline(char **line_ptr, size_t *n, FILE *stream);

void pre_main(int argc, char **argv);

#endif
