
#include "field-range-parser.h"
#include <assert.h>
#include "tap.h"

int main(int argc, const char **argv) {
  plan(2);

  struct field_range fs;
  field_range_init(&fs, NULL);
  field_range_parse(&fs, "-23");

  cmp_ok(fs.range_start, "==", 1, "range start of -23 is 1");
  cmp_ok(fs.range_end, "==", 23, "range end of -23 is 23");

  field_range_free(&fs, NULL);
  return 0;
}
