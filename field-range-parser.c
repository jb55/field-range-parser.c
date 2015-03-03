
#include "field-range-parser.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define BUCKETS 1024

//#define FRP_TESTY

struct fp_err fp_errs[] = {
  { FPERR_EXPECTED_INT,           "field integer expected" },
  { FPERR_EXPECTED_DASH_OR_COMMA, "field expected '-' or ','" }
};

static int
consume_num(const char *str, size_t *consumed, size_t *pos) {
  size_t ret;
  char *end;
  ret = strtol(str, &end, 10);
  *consumed = end - str;
  *pos += *consumed - 1;
  return ret;
}

static int
push_ints(struct field_range *fs, int start, int end) {
  int i;
  char *p;
  size_t old_size, extra_size;
  int added = 0;

  if ((size_t)end > fs->_bytes_allocated) {
    // realloc if we outgrow our list
    old_size = fs->_bytes_allocated;
    fs->_bytes_allocated = end * 1.5;
    extra_size = fs->_bytes_allocated - old_size;
    fs->_intmap = fs->_realloc_fn(fs->_intmap, fs->_bytes_allocated);
    memset(&fs->_intmap[old_size], 0, extra_size);
  }

  // this will be useful for when we're generating our int list
  if (end > fs->_highest_set) {
    fs->_highest_set = end;
  }

  // fill our _intmap with the ints
  for (i = start - 1; i < end; i++) {
    p = &fs->_intmap[i];
    if (*p != 1) {
      fs->num_explicit_fields++;
      added++;
      *p = 1;
    }
  }

  return added;
}

static void
finalize_fields(struct field_range *fs) {
  int i;
  int count = 0;
  fs->fields = fs->_realloc_fn(NULL, sizeof(int) * fs->num_explicit_fields);

  assert((size_t)fs->_highest_set <= fs->_bytes_allocated);

  for (i = 0; i < fs->_highest_set; i++) {
    if (fs->_intmap[i] == 1) {
      fs->fields[count++] = i + 1;
    }
  }

  assert(count == fs->num_explicit_fields);
}

int
field_range_parse(struct field_range *fs, const char *str) {
  char c;
  size_t len = strlen(str);
  size_t pos = 0;
  size_t consumed = 0;
  size_t range_start = 0;
  int last_num = 0;

  while (pos < len) {
    c = str[pos++];

    switch (fs->state) {
    case FPST_START:
      // starting state
      if (c == '-') {
        range_start = 1;
        fs->state = FPST_FORWARD_RANGE_START;
        continue;
      }
      else {
        // looks like we didn't get an initial range indicator
        // we should have a num then
        fs->state = FPST_INT;
        pos--;
        continue;
      }
    case FPST_FORWARD_RANGE_START: {
      last_num = consume_num(&str[pos - 1], &consumed, &pos);
      if (consumed < 1) {
        fs->state = FPST_ERROR;
        fs->err = &fp_errs[FPERR_EXPECTED_INT];
        return 0;
      }
      push_ints(fs, range_start, last_num);
      fs->all_to = last_num;
      fs->state = FPST_AFTER_INT;
      continue;
    }
    case FPST_INT: {
      last_num = consume_num(&str[pos - 1], &consumed, &pos);
      if (consumed < 1) {
        fs->state = FPST_ERROR;
        fs->err = &fp_errs[FPERR_EXPECTED_INT];
        return 0;
      }
      push_ints(fs, last_num, last_num);
      fs->state = FPST_AFTER_INT;
      continue;
    }
    case FPST_AFTER_INT: {
      // we just parsed a number into last_num
      if (c == '-') {
        // start of a range
        range_start = last_num;
        fs->state = FPST_RANGE_START;
        continue;
      }
      else if (c == ',') {
        // new field, next should be an int
        push_ints(fs, last_num, last_num);
        fs->state = FPST_INT;
        continue;
      }
      else {
        // error, we should have gotten a das or comma
        fs->state = FPST_ERROR;
        fs->err = &fp_errs[FPERR_EXPECTED_DASH_OR_COMMA];
        return 0;
      }
    }
    case FPST_RANGE_START: {
      // right after -
      last_num = consume_num(&str[pos - 1], &consumed, &pos);
      if (consumed < 1) {
        fs->state = FPST_ERROR;
        fs->err = &fp_errs[FPERR_EXPECTED_INT];
        return 0;
      }
      push_ints(fs, range_start, last_num);
      fs->state = FPST_AFTER_INT;
      continue;
    }
  }
  }

  if (fs->state == FPST_FORWARD_RANGE_START) {
    // look like we ended with an open range
    // an no number. Should this mean all fields?
    fs->all_from = 1;
  }
  if (fs->state == FPST_RANGE_START) {
    // look like we ended with an open range
    fs->all_from = range_start;
  }

  finalize_fields(fs);

  return 1;
}

struct field_range *
field_range_init(struct field_range *fs, void* (*_realloc)(void*, size_t)) {
  _realloc = _realloc ? _realloc : realloc;
  fs->_intmap             = _realloc(NULL, BUCKETS);
  fs->_bytes_allocated    = BUCKETS;
  fs->_realloc_fn         = _realloc;
  fs->state               = FPST_START;
  fs->fields              = 0;
  fs->_highest_set        = 0;
  fs->num_explicit_fields = 0;
  fs->all_to              = -1;
  fs->all_from            = -1;
  fs->err                 = 0;
  memset(fs->_intmap, 0, BUCKETS);
  return fs;
}

int
field_range_is_set(struct field_range *fs, int num) {
  if (fs->all_to   > -1 && num <= fs->all_to) return 1;
  if (fs->all_from > -1 && num >= fs->all_from) return 1;
  if (num > fs->_highest_set) return 0;
  return fs->_intmap[num - 1] == 0x1;
}

void
field_range_free(struct field_range *fs, void (*_free)(void*)) {
  _free = _free ? _free : free;
  _free(fs->_intmap);
  if (fs->fields)
    _free(fs->fields);
}
