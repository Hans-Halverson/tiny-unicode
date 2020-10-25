#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unicode_string.h"
#include "utf8.h"
#include "utf16.h"

void assert_int_equals(int actual, int expected) {
  if (actual != expected) {
    printf("FAILED: Expected %d but found %d\n", expected, actual);
  }
}

void assert_codepoint_equals(codepoint_t actual, codepoint_t expected) {
  if (actual != expected) {
    printf("FAILED: Expected %d but found %d\n", expected, actual);
  }
}

void fail(const char *message) {
  if (message) {
    printf("FAILED: %s\n", message);
  } else {
    printf("FAILED\n");
  }
}

char *read_file(FILE *file) {
  CStringBuilder_t builder;
  c_string_builder_init(&builder, 1024);

  char c = fgetc(file);
  while (c != EOF) {
    c_string_builder_append(&builder, c);
    c = fgetc(file);
  }

  char *string;
  c_string_builder_to_string(&builder, &string);
  c_string_builder_destroy(&builder);

  return string;
}

void test_utf8_encode_all(char *code_points_file) {
  codepoint_t codepoints[0x110000];
  UnicodeString_t unicode_string = { 0, codepoints };
  for (codepoint_t i = 0; i < 0x110000; i++) {
    if (IN_SURROGATE_RANGE(i) || IS_NON_CHARACTER(i)) {
      continue;
    }

    codepoints[unicode_string.size++] = i;
  }

  char *c_string;
  if (utf8_encode(&unicode_string, &c_string) != 0) {
    fail("encode all utf8_encode failed");
  }

  FILE *file = fopen(code_points_file, "r");
  char *file_contents = read_file(file);
  fclose(file);

  if (memcmp(c_string, file_contents, strlen(file_contents + 1) + 2) != 0) {
    fail("encode all strings differ");
  }

  free(c_string);
  free(file_contents);
}

void test_utf8_decode_all(char *code_points_file) {
  FILE *file = fopen(code_points_file, "r");
  char *file_contents = read_file(file);
  fclose(file);

  UnicodeString_t unicode_string;
  if (utf8_decode(file_contents + 1, &unicode_string) != 0) {
    fail("decode all utf8_decode failed");
  }

  size_t i = 0;
  for (codepoint_t codepoint = 1; codepoint <= 0x10FFFF; codepoint++) {
    if (IN_SURROGATE_RANGE(codepoint) || IS_NON_CHARACTER(codepoint)) {
      continue;
    }

    assert_codepoint_equals(unicode_string.codepoints[i++], codepoint);
  }

  if (i != unicode_string.size) {
    fail("decode all string lengths differ");
  }

  unicode_string_destroy(&unicode_string);
  free(file_contents);
}

void test_utf8_encode_decode_all() {
  char *c_string;
  codepoint_t codepoint;
  UnicodeString_t unicode_string = {1, &codepoint};
  for (codepoint_t i = 0; i <= 0x10FFFF; i++) {
    codepoint = i;
    utf8_encode(&unicode_string, &c_string);
    utf8_decode(c_string, &unicode_string);
    free(c_string);

    assert_codepoint_equals(codepoint, i);
  }
}

void test_utf8_decode_invalid() {
  UnicodeString_t unicode_string;
  // Invalid leading bytes
  assert_int_equals(utf8_decode("\xF8\x80\x80\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xF9\x80\x80\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xFA\x80\x80\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xFB\x80\x80\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xFC\x80\x80\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xFD\x80\x80\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xFE\x80\x80\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xFF\x80\x80\x80", &unicode_string), -1);

  // Missing continuation byte in 2-byte code point
  assert_int_equals(utf8_decode("\xC0\x00", &unicode_string), -1);

  // Missing continuation bytes in 3-byte code point
  assert_int_equals(utf8_decode("\xE0\x00\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xE0\x80\x00", &unicode_string), -1);

  // Missing continuation bytes in 4-byte code point
  assert_int_equals(utf8_decode("\xF0\x00\x80\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xF0\x08\x00\x80", &unicode_string), -1);
  assert_int_equals(utf8_decode("\xF0\x08\x80\x00", &unicode_string), -1);

  // Outside range of code points
  for (codepoint_t i = 0x110000; i < (1 << 21); i++) {
    char encoded_string[5];
    sprintf(
      encoded_string, 
      "%c%c%c%c", 
      0xF0 | (i >> 18),
      0x80 | ((i >> 12) & 0x3F),
      0x80 | ((i >> 6) & 0x3F),
      0x80 | (i & 0x3F)
    );
    assert_int_equals(utf8_decode(encoded_string, &unicode_string), -1);
  }
}

void test_utf16_encode_decode_all() {
  char *c_string;
  codepoint_t codepoint;
  UnicodeString_t unicode_string = {1, &codepoint};
  for (codepoint_t i = 0; i <= 0x10FFFF; i++) {
    codepoint = i;
    utf16_encode(&unicode_string, &c_string);
    utf16_decode(c_string, &unicode_string);
    free(c_string);

    assert_codepoint_equals(codepoint, i);
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    printf("Usage: test <utf8-file>\n");
    exit(1);
  }

  char *code_points_file = argv[1];

  test_utf8_encode_all(code_points_file);
  test_utf8_decode_all(code_points_file);
  test_utf8_encode_decode_all();
  test_utf8_decode_invalid();

  test_utf16_encode_decode_all();
}