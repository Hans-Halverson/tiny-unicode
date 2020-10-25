#include "includes/utf8.h"
#include <stdio.h>

#define INITIAL_BUILDER_CAPACITY 10

#define IS_CONTINUATION_BYTE(BYTE) (((BYTE) & 0xC0) == 0x80)

#define INVALID_CODEPOINT ((codepoint_t)-1)

int utf8_decode(const char *bytes, UnicodeString_t *output) {
  // Build up string one codepoint at a time
  UnicodeStringBuilder_t builder;
  unicode_string_builder_init(&builder, INITIAL_BUILDER_CAPACITY);

  while (*bytes != 0) {
    codepoint_t codepoint = utf8_decode_codepoint(&bytes);
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

codepoint_t utf8_decode_codepoint(const char **pointer) {
  const char *bytes = *pointer;
  char c1 = *bytes;
  char c2, c3, c4;

  codepoint_t codepoint;
  if ((c1 & 0x80) == 0) {
    // ASCII characters < 128
    codepoint = c1;

    (*pointer)++;
  } else if ((c1 & 0xE0) == 0xC0) {
    // Two byte sequence
    c2 = bytes[1];
    if (!IS_CONTINUATION_BYTE(c2)) {
      codepoint = INVALID_CODEPOINT;
    }

    codepoint = (c1 & 0x1F) << 6;
    codepoint |= (c2 & 0x3F);

    (*pointer) += 2;
  } else if ((c1 & 0xF0) == 0xE0) {
    // Three byte sequence
    c2 = bytes[1];
    c3 = bytes[2];
    if (!IS_CONTINUATION_BYTE(c2) || !IS_CONTINUATION_BYTE(c3)) {
      codepoint = INVALID_CODEPOINT;
    }

    codepoint = (c1 & 0x0F) << 12;
    codepoint |= (c2 & 0x3F) << 6;
    codepoint |= (c3 & 0x3F);

    (*pointer) += 3;
  } else if ((c1 & 0xF8) == 0xF0) {
    // Four byte sequence
    c2 = bytes[1];
    c3 = bytes[2];
    c4 = bytes[3];
    if (!IS_CONTINUATION_BYTE(c2) || !IS_CONTINUATION_BYTE(c3) || !IS_CONTINUATION_BYTE(c4)) {
      codepoint = INVALID_CODEPOINT;
    }

    codepoint = (c1 & 0x07) << 18;
    codepoint |= (c2 & 0x3F) << 12;
    codepoint |= (c3 & 0x3F) << 6;
    codepoint |= (c4 & 0x3F);

    if (codepoint > 0x10FFFF) {
      codepoint = INVALID_CODEPOINT;
    }

    (*pointer) += 4;
  } else {
    codepoint = INVALID_CODEPOINT;
  }

  return codepoint;
}

int utf8_encode(UnicodeString_t *string, char **output) {
  // Build up string one byte at a time
  CStringBuilder_t builder;
  c_string_builder_init(&builder, string->size);

  codepoint_t *codepoints = string->codepoints;
  for (size_t i = 0; i < string->size; i++) {
    codepoint_t codepoint = codepoints[i];
    if (codepoint < 0x80) {
      c_string_builder_append(&builder, (char)codepoint);                    // 0xxxxxxx
    } else if (codepoint <= 0x7FF) {
      c_string_builder_append(&builder, 0xC0 | (codepoint >> 6));            // 110xxxxx
      c_string_builder_append(&builder, 0x80 | (codepoint & 0x3F));          // 10xxxxxx
    } else if (codepoint <= 0xFFFF) {
      c_string_builder_append(&builder, 0xE0 | (codepoint >> 12));           // 1110xxxx
      c_string_builder_append(&builder, 0x80 | ((codepoint >> 6) & 0x3F));   // 10xxxxxx
      c_string_builder_append(&builder, 0x80 | (codepoint & 0x3F));          // 10xxxxxx
    } else if (codepoint <= 0x10FFFF) {
      c_string_builder_append(&builder, 0xF0 | (codepoint >> 18));           // 11110xxx
      c_string_builder_append(&builder, 0x80 | ((codepoint >> 12) & 0x3F));  // 10xxxxxx
      c_string_builder_append(&builder, 0x80 | ((codepoint >> 6) & 0x3F));   // 10xxxxxx
      c_string_builder_append(&builder, 0x80 | (codepoint & 0x3F));          // 10xxxxxx
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
