#include "utf16.h"

#define INITIAL_BUILDER_CAPACITY 10
#define INVALID_CODEPOINT ((codepoint_t)-1)

codepoint_t utf16_decode_codepoint(uint16_t **pointer);

int utf16_decode(String_t string, UnicodeString_t *output) {
  // Build up string one codepoint at a time
  UnicodeStringBuilder_t builder;
  unicode_string_builder_init(&builder, INITIAL_BUILDER_CAPACITY);

  const char *pointer = string.buffer;
  const char *buffer_end = string.buffer + string.size;

  while (pointer < buffer_end) {
    codepoint_t codepoint = utf16_decode_codepoint((uint16_t **)&pointer);
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
    codepoint += 0x10000;

    (*pointer) += 2;
    return codepoint;
  } else if (IS_LOW_SURROGATE(c1)) {
    return INVALID_CODEPOINT;
  }

  (*pointer)++;
  return c1;
}

int utf16_encode(UnicodeString_t string, String_t *output) {
  // Build up string one byte at a time
  StringBuilder_t builder;
  string_builder_init(&builder, string.size);

  for (size_t i = 0; i < string.size; i++) {
    codepoint_t codepoint = string.buffer[i];
    if (codepoint <= 0xFFFF) {
      string_builder_append(&builder, codepoint);
      string_builder_append(&builder, codepoint >> 8);
    } else if (codepoint <= 0x10FFFF){
      codepoint -= 0x10000;
      uint16_t unit1 = (codepoint >> 10) + 0xD800;
      uint16_t unit2 = (codepoint & 0x3FF) + 0xDC00;

      string_builder_append(&builder, unit1);
      string_builder_append(&builder, unit1 >> 8);
      string_builder_append(&builder, unit2);
      string_builder_append(&builder, unit2 >> 8);
    } else {
      // Error if outside range of unicode code points
      string_builder_destroy(&builder);
      return -1;
    }
  }

  string_builder_to_string(&builder, output);
  string_builder_destroy(&builder);

  return 0;
}
