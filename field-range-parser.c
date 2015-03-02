
#include "field-range-parser.h"
#include <stdio.h>
#include <string.h>

struct fp_err fp_errs[] = {
  { FPERR_EXPECTED_INT,           "field integer expected" },
  { FPERR_EXPECTED_DASH_OR_COMMA, "field expected '-' or ','" }
};

static int
consume_num(const char *str, size_t *consumed) {
  char *end;
  int ret;
  ret = strtol(str, &end, 10);
  // consumed = end - str
  *consumed = end - str;
  return ret;
}

void
inc_fields(struct field_range *fs, int num) {
  if (fs->num_fields + 1 > (fs->bytes_allocated / sizeof(int))) {
    // realloc if we outgrow our list
    fs->bytes_allocated *= 1.5;
    fs->fields = fs->realloc_fn(fs->fields, fs->bytes_allocated);
  }

  fs->num_fields++;
  *fs->fields++ = num;
}

int
field_range_parse(struct field_range *fs, const char *str) {
  char c;
  int num;
  size_t len = strlen(str);
  size_t pos = 0;
  size_t consumed = 0;
  size_t range_start = 0;
  size_t range_end = 0;

  while (pos < len) {
    c = str[pos++];

    switch (fs->state) {
    case FPST_START:
      // starting state
      if (c == '-') {
        range_start = 1;
        fs->state = FPST_OPEN_FORWARD_RANGE;
        continue;
      }
      else {
        // looks like we didn't get an initial range indicator
        // we should have an int next
        fs->state = FPST_FIRST_FIELD;
        pos--;
        continue;
      }
    case FPST_FIRST_FIELD: {
      // after -
      // we didn't get a -, so this should be an int
      num = consume_num(&str[pos - 1], &consumed);
      if (consumed < 1) {
        fs->state = FPST_ERROR;
        fs->err_loc = pos - 1;
        fs->err = &fp_errs[FPERR_EXPECTED_INT];
        return 0;
      }
      pos += consumed;
      fs->range_start = num;
      fs->state = FPST_AFTER_START_INT;
      continue;
    }
    case FPST_AFTER_START_INT: {
      // might be a dash or more fields here
      if (c == '-') {
        fs->state = FPST_RANGE_AFTER_FIRST;
        continue;
      }
      else if (c == ',') {
        // field list starts
        inc_fields(fs, fs->range_start);
        fs->range_start = 0;
        fs->state = FPST_FIELD;
        continue;
      }
      else {
        // error, expected - or ,
        fs->state = FPST_ERROR;
        fs->err_loc = pos - 1;
        fs->err = &fp_errs[FPERR_EXPECTED_DASH_OR_COMMA];
        return 0;
      }
    }
    case FPST_FIELD: {
      // right after ','
      // should have an int here
      num = consume_num(&str[pos - 1], &consumed);
      if (consumed < 1) {
        fs->state = FPST_ERROR;
        fs->err_loc = pos - 1;
        fs->err = &fp_errs[FPERR_EXPECTED_INT];
        return 0;
      }
      pos += consumed;
      inc_fields(fs, num);
      fs->state = FPST_AFTER_FIELD;
    }
    case FPST_AFTER_FIELD: {
      // right after ',n'
      // should have another , or -
      if (c == '-') {
        // looks like we're ending our field list with a range
        fs->state
      }
      else if (c == ',') {
        // more fields!
        fs->state = FPST_FIELD;
        continue;
      }
      else if (c == ' ') {
        // uhhh whitespace here should be fine?
        // let's continue parsing and see
        continue;
      }
      else {
        // we didn't get anything we expected here
        // error!

      }
    }
    case FPST_RANGE_END:
      // the next character should be range_end
      fs->range_end = consume_num(&str[pos - 1], &consumed);
      if (consumed < 1) {
        fs->state = FPST_ERROR;
        fs->err = &fp_errs[FPERR_EXPECTED_INT];
        return 0;
      }
      return 1;
    }
  }

  return 1;
}

struct field_range *
field_range_init(struct field_range *fs, void* (*_realloc)(void*, size_t)) {
  _realloc = _realloc ? _realloc : realloc;
  fs->fields          = _realloc(NULL, 64);
  fs->bytes_allocated = 1024;
  fs->state           = FPST_START;
  fs->type            = FIELD_TYPE_UNKNOWN;
  return fs;
}

void
field_range_free(struct field_range *fs, void (*_free)(void*)) {
  _free = _free ? _free : free;
  _free(fs->fields);
}
