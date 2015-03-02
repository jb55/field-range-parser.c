
#include "field-range-parser.h"
#include <assert.h>
#include "tap.h"

int main(int argc, const char **argv) {
  plan(6);

  int i;
  struct field_range fs;

  field_range_init(&fs, NULL);
  field_range_parse(&fs, "3");
  cmp_ok(fs.num_explicit_fields, "==", 1, "should have 1 fields set");
  ok(field_range_is_set(&fs, 3), "field 3 set");
  ok(!fs.err, "shouldnt have err");
  field_range_free(&fs, NULL);

  field_range_init(&fs, NULL);
  field_range_parse(&fs, "-3");
  cmp_ok(fs.num_explicit_fields, "==", 3, "should have 3 fields set");
  ok(field_range_is_set(&fs, 1), "field 1 set");
  ok(field_range_is_set(&fs, 2), "field 2 set");
  ok(field_range_is_set(&fs, 3), "field 3 set");
  ok(!fs.err, "shouldnt have err");
  field_range_free(&fs, NULL);

  field_range_init(&fs, NULL);
  field_range_parse(&fs, "2-4");
  cmp_ok(fs.num_explicit_fields, "==", 3, "should have 3 fields set");
  ok(field_range_is_set(&fs, 2), "field 2 set");
  ok(field_range_is_set(&fs, 3), "field 3 set");
  ok(field_range_is_set(&fs, 4), "field 4 set");
  ok(!fs.err, "shouldnt have err");
  field_range_free(&fs, NULL);

  field_range_init(&fs, NULL);
  field_range_parse(&fs, "11-15-");
  cmp_ok(fs.num_explicit_fields, "==", 5, "should have 111 fields set");
  ok(field_range_is_set(&fs, 11), "field 333 set");
  ok(!field_range_is_set(&fs, 10), "field 332 not set");
  ok(field_range_is_set(&fs, 532), "field 532 set");
  field_range_free(&fs, NULL);

  field_range_init(&fs, NULL);
  field_range_parse(&fs, "3,4,5-11");
  cmp_ok(fs.num_explicit_fields, "==", 9, "should have 9 fields set");
  ok(field_range_is_set(&fs, 3), "field 3 set");
  ok(field_range_is_set(&fs, 4), "field 4 set");
  ok(field_range_is_set(&fs, 5), "field 5 set");

  for (i = 6; i < 12; i++) {
    ok(field_range_is_set(&fs, i), "field %d set", i);
  }

  ok(!fs.err, "shouldnt have err");
  field_range_free(&fs, NULL);

  return 0;
}
