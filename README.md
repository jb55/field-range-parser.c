
# field-range-parser.c

  Parse field ranges

```
  N     N'th field, counted from 1
  N-    from N'th field, to end of line
  N-M   from N'th to M'th (included) field
  -M    from first to M'th (included) field
```

## Parsable string examples

  Field `1`, `2`, `3 to 15`, and everything above `35`

    1,2,3-15,35-

  `1` to `12`

    -12

  All fields

    `-`

## API Example

```c
#include "field-range-parser.h"

int main(int argc, const char **argv) {
  struct field_range fs;
  int i;
  int field;

  field_range_init(&fs, NULL);
  field_range_parse(&fs, "11-15-");

  field_range_is_set(&fs, 11); // field 11 is set
  !field_range_is_set(&fs, 10); // field 10 not set
  field_range_is_set(&fs, 532); // field 532 is set

  for (i = 0; i < fs.num_explicit_fields; ++i) {
    printf("field %d is set\n", fs.fields[i]);
  }

  printf("everything after %d is set\n", fs.all_from);

  field_range_free(&fs, NULL);

  return 0;
}
```

## Installation

  Install with clib

    $ clib install jb55/field-range-parser.c

## API

```c
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
};

struct field_range *
field_range_init(struct field_range *fs, void* (*_realloc)(void*, size_t));

int
field_range_is_set(struct field_range *fs, int field);

void
field_range_free(struct field_range *fs, void (*_free)(void*));

int
field_range_parse(struct field_range *fs, const char *str);
```

## License

  The MIT License (MIT)

  Copyright (c) 2015 William Casarin

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
