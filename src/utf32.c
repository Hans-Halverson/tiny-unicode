#include "utf16.h"

#include <stdlib.h>

#define INITIAL_BUILDER_CAPACITY 10
#define INVALID_CODEPOINT ((codepoint_t)-1)

int utf32_decode(String_t string, UnicodeString_t *output) {
  // Check for valid string length
  if (string.size % 4 != 0) {
    return -1;
  }

  // No need for UnicodeStringBuilder as we know size of buffer
  codepoint_t *buffer = malloc(string.size);

  for (size_t i = 0; i < string.size / 4; i++) {
    codepoint_t codepoint = ((uint32_t *)string.buffer)[i];
    if (codepoint > 0x10FFFF) {
      // Error if outside range of unicode code points
      free(buffer);
      return -1;
    }

    buffer[i] = codepoint;
  }

  (*output) = (UnicodeString_t){string.size / 4, buffer};

  return 0;
}

int utf32_encode(UnicodeString_t string, String_t *output) {
  // No need for StringBuilder as we know size of buffer
  uint32_t *buffer = malloc(string.size * 4);

  for (size_t i = 0; i < string.size; i++) {
    codepoint_t codepoint = string.buffer[i];
    if (codepoint > 0x10FFFF) {
      // Error if outside range of unicode code points
      free(buffer);
      return -1;
    }

    buffer[i] = codepoint;
  }

  (*output) = (String_t){string.size * 4, (char *)buffer};

  return 0;
}
