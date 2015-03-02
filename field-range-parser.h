
#ifndef FIELD_RANGE_PARSER_H
#define FIELD_RANGE_PARSER_H

#include <stdlib.h>

#define FIELD_TYPE_UNKNOWN 0
#define FIELD_TYPE_RANGE 1
#define FIELD_TYPE_SPARSE 2

#define FPERR_EXPECTED_INT 0
#define FPERR_EXPECTED_DASH_OR_COMMA 1

#define FPST_ERROR 0
#define FPST_START 1
#define FPST_DONE 2
#define FPST_OPEN_FORWARD_RANGE 3
#define FPST_INT 4
#define FPST_AFTER_INT 5
#define FPST_RANGE_START 6
#define FPST_FORWARD_RANGE_START 7

struct fp_err {
  int code;
  const char *message;
};

struct field_range {
  const struct fp_err * err;
  size_t err_loc;
  int num_explicit_fields;
  int all_from;
  int all_to;
  int *fields;

  // internal
  int _highest_set;
  int state;
  char* _intmap;
  size_t _bytes_allocated;
  void* (*_realloc_fn)(void*, size_t);
};

struct field_range *
field_range_init(struct field_range *fs, void* (*_realloc)(void*, size_t));

int
field_range_is_set(struct field_range *fs, int field);

void
field_range_free(struct field_range *fs, void (*_free)(void*));

int
field_range_parse(struct field_range *fs, const char *str);

#endif /* FIELD_RANGE_PARSER_H */
