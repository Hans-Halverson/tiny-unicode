#include "utf16.h"

#define INITIAL_BUILDER_CAPACITY 10
#define INVALID_CODEPOINT ((codepoint_t)-1)

codepoint_t utf16_decode_codepoint(const char **pointer, const char *buffer_end);

int utf16_decode(String_t string, UnicodeString_t *output) {
  // Build up string one codepoint at a time
  UnicodeStringBuilder_t builder;
  unicode_string_builder_init(&builder, INITIAL_BUILDER_CAPACITY);

  const char *pointer = string.buffer;
  const char *buffer_end = string.buffer + string.size;

  while (pointer < buffer_end - 1) {
    codepoint_t codepoint = utf16_decode_codepoint(&pointer, buffer_end);
    if (codepoint == INVALID_CODEPOINT) {
      unicode_string_builder_destroy(&builder);
      return -1;
    }

    unicode_string_builder_append(&builder, codepoint);
  }

  // Check if there are dangling characters left in buffer
  if (pointer != buffer_end) {
    unicode_string_builder_destroy(&builder);
    return -1;
  }

  // Create unicode string and free string builder
  unicode_string_builder_to_string(&builder, output);
  unicode_string_builder_destroy(&builder);

  return 0;
}

codepoint_t utf16_decode_codepoint(const char **pointer, const char *buffer_end) {
  uint16_t *bytes = (uint16_t *)*pointer;
  uint16_t c1 = *bytes;

  if (IS_HIGH_SURROGATE(c1)) {
    // Check if there is enough room for second code unit
    if (((char *)bytes) + 3 >= buffer_end) {
      return INVALID_CODEPOINT;
    }

    uint16_t c2 = bytes[1];
    if (!IS_LOW_SURROGATE(c2)) {
      return INVALID_CODEPOINT;
    }

    codepoint_t codepoint = c2 - 0xDC00;
    codepoint |= (c1 - 0xD800) << 10;
    codepoint += 0x10000;

    (*pointer) += 4;
    return codepoint;
  } else if (IS_LOW_SURROGATE(c1)) {
    return INVALID_CODEPOINT;
  }

  (*pointer) += 2;
  return c1;
}

int utf16_encode(UnicodeString_t string, String_t *output) {
  // Build up string one byte at a time
  StringBuilder_t builder;
  string_builder_init(&builder, string.size * 2);

  for (size_t i = 0; i < string.size; i++) {
    codepoint_t codepoint = string.buffer[i];
    if (codepoint <= 0xFFFF) {
      string_builder_append2(&builder, codepoint);
    } else if (codepoint <= 0x10FFFF){
      codepoint -= 0x10000;
      uint16_t unit1 = (codepoint >> 10) + 0xD800;
      uint16_t unit2 = (codepoint & 0x3FF) + 0xDC00;

      string_builder_append2(&builder, unit1);
      string_builder_append2(&builder, unit2);
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
