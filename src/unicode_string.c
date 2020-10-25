#include "includes/unicode_string.h"

#include <stdlib.h>
#include <string.h>

void c_string_builder_init(CStringBuilder_t *builder, size_t capacity) {
  builder->size = 0;
  builder->capacity = capacity;
  builder->buffer = malloc(capacity);
}

void c_string_builder_destroy(CStringBuilder_t *builder) {
  free(builder->buffer);
}

void c_string_builder_append(CStringBuilder_t *builder, char c) {
  if (builder->size == builder->capacity) {
    builder->buffer = realloc(builder->buffer, builder->capacity * 2);
    builder->capacity *= 2;
  }

  builder->buffer[builder->size] = c;
  builder->size++;
}

void c_string_builder_to_string(CStringBuilder_t *builder, char **string) {
  // Copy to null terminated string of exact size
  char *c_string = malloc(builder->size + 1);
  memcpy(c_string, builder->buffer, builder->size);
  c_string[builder->size] = '\0';

  (*string) = c_string;
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
  string->codepoints = malloc(string->size * sizeof(codepoint_t));
  memcpy(string->codepoints, builder->buffer, string->size * sizeof(codepoint_t));
}

void unicode_string_destroy(UnicodeString_t *string) {
  free(string->codepoints);
}