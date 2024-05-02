#ifndef _CAT_H
#define _CAT_H

#include <getopt.h>
#include <stdbool.h>

typedef struct {
  bool flag_e;
  bool flag_v;
  bool flag_b;
  bool flag_n;
  bool flag_s;
  bool flag_t;
} opt_t;

void cat_arg_n(int *count, opt_t args);

void print_non_printable(char c);

void cat_arg_b(char prev, int *count, opt_t args);

void cat(char *filename, opt_t args);

#endif
