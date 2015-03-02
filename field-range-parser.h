
#ifndef FIELD_RANGE_PARSER_H
#define FIELD_RANGE_PARSER_H

#include <stdlib.h>

#define FIELD_TYPE_UNKNOWN 0
#define FIELD_TYPE_RANGE 1
#define FIELD_TYPE_SPARSE 2

#define FPST_ERROR 0
#define FPST_START 1
#define FPST_DONE 2
#define FPST_RANGE_END 3
#define FPST_RANGE_AFTER_FIRST 4
#define FPST_FIRST_FIELD 5
#define FPST_AFTER_START_INT 6
#define FPST_FIELD 7
#define FPST_AFTER_FIELD 8

#define FPERR_EXPECTED_INT 0
#define FPERR_EXPECTED_DASH_OR_COMMA 1

struct fp_err {
  int code;
  const char *message;
};

struct field_range {
  int type;
  int range_start;
  int range_end;
  int* fields;

  // bookkeeping
  int state;
  const struct fp_err * err;
  size_t err_loc;
  size_t num_fields;
  size_t bytes_allocated;
  void* (*realloc_fn)(void*, size_t);
};

struct field_range *
field_range_init(struct field_range *fs, void* (*_realloc)(void*, size_t));

void
field_range_free(struct field_range *fs, void (*_free)(void*));

int
field_range_parse(struct field_range *fs, const char *str);

#endif /* FIELD_RANGE_PARSER_H */
