#pragma once

#include <stddef.h>
#include "common.h"

typedef struct String {
  size_t size;
  char *buffer;
} String_t;

typedef struct StringBuilder {
  size_t size;
  size_t capacity;
  char *buffer;
} StringBuilder_t;

void string_builder_init(StringBuilder_t *builder, size_t capacity);
void string_builder_destroy(StringBuilder_t *builder);
void string_builder_append(StringBuilder_t *builder, char c);
void string_builder_to_string(StringBuilder_t *builder, String_t *string);

void string_destroy(String_t *string);

typedef struct UnicodeString {
  size_t size;
  codepoint_t *buffer;
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

void unicode_string_destroy(UnicodeString_t *string);