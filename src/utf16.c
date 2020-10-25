#include "utf16.h"

#define INITIAL_BUILDER_CAPACITY 10
#define INVALID_CODEPOINT ((codepoint_t)-1)

codepoint_t utf16_decode_codepoint(uint16_t **pointer);

int utf16_decode(const char *bytes, UnicodeString_t *output) {
  // Build up string one codepoint at a time
  UnicodeStringBuilder_t builder;
  unicode_string_builder_init(&builder, INITIAL_BUILDER_CAPACITY);

  while (*bytes != 0) {
    codepoint_t codepoint = utf16_decode_codepoint((uint16_t **)&bytes);
    if (codepoint == INVALID_CODEPOINT) {
      unicode_string_builder_destroy(&builder);
      return -1;
    }

    unicode_string_builder_append(&builder, codepoint);
  }

  // Create unicode string and free string builder
  unicode_string_builder_to_string(&builder, output);
  unicode_string_builder_destroy(&builder);

  return 0;
}

codepoint_t utf16_decode_codepoint(uint16_t **pointer) {
  uint16_t *bytes = *pointer;
  uint16_t c1 = *bytes;

  if (IS_HIGH_SURROGATE(c1)) {
    uint16_t c2 = bytes[1];
    if (!IS_LOW_SURROGATE(c2)) {
      return INVALID_CODEPOINT;
    }

    codepoint_t codepoint = c2 - 0xDC00;
    codepoint |= (c1 - 0xD800) << 10;

    if (codepoint > 0x10FFFF) {
      return INVALID_CODEPOINT;
    }

    (*pointer) += 2;
    return codepoint;
  }

  (*pointer)++;
  return c1;
}

int utf16_encode(UnicodeString_t *string, char **output) {
  // Build up string one byte at a time
  CStringBuilder_t builder;
  c_string_builder_init(&builder, string->size);

  codepoint_t *codepoints = string->codepoints;
  for (size_t i = 0; i < string->size; i++) {
    codepoint_t codepoint = codepoints[i];
    if (codepoint <= 0xFFFF) {
      c_string_builder_append(&builder, codepoint >> 8);
      c_string_builder_append(&builder, codepoint);
    } else if (codepoint <= 0x10FFFF){
      codepoint -= 0x10000;
      uint16_t unit1 = (codepoint >> 10) + 0xD800;
      uint16_t unit2 = (codepoint & 0x3FF) + 0xDC00;

      c_string_builder_append(&builder, unit1 >> 8);
      c_string_builder_append(&builder, unit1);
      c_string_builder_append(&builder, unit2 >> 8);
      c_string_builder_append(&builder, unit2);
    } else {
      // Error if outside range of unicode code points
      c_string_builder_destroy(&builder);
      return -1;
    }
  }

  c_string_builder_to_string(&builder, output);
  c_string_builder_destroy(&builder);

  return 0;
}
