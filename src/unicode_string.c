#include "includes/unicode_string.h"

#include <stdlib.h>
#include <string.h>

void string_builder_init(StringBuilder_t *builder, size_t capacity) {
  builder->size = 0;
  builder->capacity = capacity;
  builder->buffer = malloc(capacity);
}

void string_builder_destroy(StringBuilder_t *builder) {
  free(builder->buffer);
}

void string_builder_append(StringBuilder_t *builder, char c) {
  if (builder->size == builder->capacity) {
    builder->buffer = realloc(builder->buffer, builder->capacity * 2);
    builder->capacity *= 2;
  }

  builder->buffer[builder->size] = c;
  builder->size++;
}

void string_builder_to_string(StringBuilder_t *builder, String_t *string) {
  string->size = builder->size;
  string->buffer = malloc(builder->size);
  memcpy(string->buffer, builder->buffer, builder->size);
}

void string_destroy(String_t *string) {
  free(string->buffer);
}

void unicode_string_builder_init(UnicodeStringBuilder_t *builder, size_t capacity) {
  builder->size = 0;
  builder->capacity = capacity;
  builder->buffer = malloc(capacity * sizeof(codepoint_t));
}

void unicode_string_builder_destroy(UnicodeStringBuilder_t *builder) {
  free(builder->buffer);
}

void unicode_string_builder_append(UnicodeStringBuilder_t *builder, codepoint_t codepoint) {
  if (builder->size == builder->capacity) {
    builder->buffer = realloc(builder->buffer, builder->capacity * 2 * sizeof(codepoint_t));
    builder->capacity *= 2;
  }
 
  builder->buffer[builder->size] = codepoint;
  builder->size++;
}

void unicode_string_builder_to_string(UnicodeStringBuilder_t *builder, UnicodeString_t *string) {
  string->size = builder->size;
  string->buffer = malloc(string->size * sizeof(codepoint_t));
  memcpy(string->buffer, builder->buffer, string->size * sizeof(codepoint_t));
}

void unicode_string_destroy(UnicodeString_t *string) {
  free(string->buffer);
}