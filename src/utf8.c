#include "includes/utf8.h"
#include <stdio.h>

#define INITIAL_BUILDER_CAPACITY 10

#define IS_CONTINUATION_BYTE(BYTE) (((BYTE) & 0xC0) == 0x80)

#define INVALID_CODEPOINT ((codepoint_t)-1)

codepoint_t utf8_decode_codepoint(const char **pointer, const char *buffer_end);

int utf8_decode(String_t string, UnicodeString_t *output) {
  // Build up string one codepoint at a time
  UnicodeStringBuilder_t builder;
  unicode_string_builder_init(&builder, INITIAL_BUILDER_CAPACITY);

  const char *pointer = string.buffer;
  const char *buffer_end = string.buffer + string.size;

  while (pointer < buffer_end) {
    codepoint_t codepoint = utf8_decode_codepoint(&pointer, buffer_end);
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

codepoint_t utf8_decode_codepoint(const char **pointer, const char *buffer_end) {
  const char *bytes = *pointer;
  char c1 = *bytes;
  char c2, c3, c4;

  codepoint_t codepoint;
  if ((c1 & 0x80) == 0) {
    // ASCII characters < 128
    (*pointer)++;
    codepoint = c1;
  } else if ((c1 & 0xE0) == 0xC0 && bytes + 1 < buffer_end) {
    // Two byte sequence
    (*pointer) += 2;

    c2 = bytes[1];
    if (!IS_CONTINUATION_BYTE(c2)) {
      return INVALID_CODEPOINT;
    }

    codepoint = (c1 & 0x1F) << 6;
    codepoint |= (c2 & 0x3F);

  } else if ((c1 & 0xF0) == 0xE0 && bytes + 2 < buffer_end) {
    // Three byte sequence
    (*pointer) += 3;

    c2 = bytes[1];
    c3 = bytes[2];
    if (!IS_CONTINUATION_BYTE(c2) || !IS_CONTINUATION_BYTE(c3)) {
      return INVALID_CODEPOINT;
    }

    codepoint = (c1 & 0x0F) << 12;
    codepoint |= (c2 & 0x3F) << 6;
    codepoint |= (c3 & 0x3F);
  } else if ((c1 & 0xF8) == 0xF0 && bytes + 3 < buffer_end) {
    // Four byte sequence
    (*pointer) += 4;

    c2 = bytes[1];
    c3 = bytes[2];
    c4 = bytes[3];
    if (!IS_CONTINUATION_BYTE(c2) || !IS_CONTINUATION_BYTE(c3) || !IS_CONTINUATION_BYTE(c4)) {
      return INVALID_CODEPOINT;
    }

    codepoint = (c1 & 0x07) << 18;
    codepoint |= (c2 & 0x3F) << 12;
    codepoint |= (c3 & 0x3F) << 6;
    codepoint |= (c4 & 0x3F);

    if (codepoint > 0x10FFFF) {
      return INVALID_CODEPOINT;
    }
  } else {
    return INVALID_CODEPOINT;
  }

  return codepoint;
}

int utf8_encode(UnicodeString_t string, String_t *output) {
  // Build up string one byte at a time
  StringBuilder_t builder;
  string_builder_init(&builder, string.size);

  for (size_t i = 0; i < string.size; i++) {
    codepoint_t codepoint = string.buffer[i];
    if (codepoint < 0x80) {
      string_builder_append(&builder, (char)codepoint);                    // 0xxxxxxx
    } else if (codepoint <= 0x7FF) {
      string_builder_append(&builder, 0xC0 | (codepoint >> 6));            // 110xxxxx
      string_builder_append(&builder, 0x80 | (codepoint & 0x3F));          // 10xxxxxx
    } else if (codepoint <= 0xFFFF) {
      string_builder_append(&builder, 0xE0 | (codepoint >> 12));           // 1110xxxx
      string_builder_append(&builder, 0x80 | ((codepoint >> 6) & 0x3F));   // 10xxxxxx
      string_builder_append(&builder, 0x80 | (codepoint & 0x3F));          // 10xxxxxx
    } else if (codepoint <= 0x10FFFF) {
      string_builder_append(&builder, 0xF0 | (codepoint >> 18));           // 11110xxx
      string_builder_append(&builder, 0x80 | ((codepoint >> 12) & 0x3F));  // 10xxxxxx
      string_builder_append(&builder, 0x80 | ((codepoint >> 6) & 0x3F));   // 10xxxxxx
      string_builder_append(&builder, 0x80 | (codepoint & 0x3F));          // 10xxxxxx
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
