#pragma once

#include <stddef.h>
#include "common.h"

typedef struct CStringBuilder {
  size_t size;
  size_t capacity;
  char *buffer;
} CStringBuilder_t;

void c_string_builder_init(CStringBuilder_t *builder, size_t capacity);
void c_string_builder_destroy(CStringBuilder_t *builder);
void c_string_builder_append(CStringBuilder_t *builder, char c);
void c_string_builder_to_string(CStringBuilder_t *builder, char **string);

typedef struct UnicodeString {
  size_t size;
  codepoint_t *codepoints;
} UnicodeString_t;

typedef struct UnicodeStringBuilder {
  size_t size;
  size_t capacity;
  codepoint_t *buffer;
} UnicodeStringBuilder_t;

void unicode_string_builder_init(UnicodeStringBuilder_t *builder, size_t capacity);
void unicode_string_builder_destroy(UnicodeStringBuilder_t *builder);
void unicode_string_builder_append(UnicodeStringBuilder_t *builder, codepoint_t codepoint);
void unicode_string_builder_to_string(UnicodeStringBuilder_t *builder, UnicodeString_t *string);

void unicode_string_destroy(UnicodeString_t *builder);