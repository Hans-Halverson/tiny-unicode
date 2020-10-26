#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unicode_string.h"
#include "utf8.h"
#include "utf16.h"
#include "utf32.h"

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

String_t read_file(FILE *file) {
  StringBuilder_t builder;
  string_builder_init(&builder, 1024);

  int c = fgetc(file);
  while (c != EOF) {
    string_builder_append(&builder, c);
    c = fgetc(file);
  }

  String_t string;
  string_builder_to_string(&builder, &string);
  string_builder_destroy(&builder);

  return string;
}

void test_utf8_encode_all(const char *filename) {
  codepoint_t codepoints[0x110000];
  UnicodeString_t unicode_string = { 0, codepoints };
  for (codepoint_t i = 0; i < 0x110000; i++) {
    if (IN_SURROGATE_RANGE(i)) {
      continue;
    }

    codepoints[unicode_string.size++] = i;
  }

  String_t string;
  if (utf8_encode(unicode_string, &string) != 0) {
    fail("encode all utf8_encode failed");
  }

  FILE *file = fopen(filename, "r");
  String_t file_contents = read_file(file);
  fclose(file);

  if (string.size != file_contents.size ||
      memcmp(string.buffer, file_contents.buffer, string.size) != 0) {
    fail("utf-8 encode all strings differ");
  }

  string_destroy(&string);
  string_destroy(&file_contents);
}

void test_utf8_decode_all(const char *filename) {
  FILE *file = fopen(filename, "r");
  String_t file_contents = read_file(file);
  fclose(file);

  UnicodeString_t unicode_string;
  if (utf8_decode(file_contents, &unicode_string) != 0) {
    fail("decode all utf8_decode failed");
  }

  size_t i = 0;
  for (codepoint_t codepoint = 0; codepoint <= 0x10FFFF; codepoint++) {
    if (IN_SURROGATE_RANGE(codepoint)) {
      continue;
    }

    assert_codepoint_equals(unicode_string.buffer[i++], codepoint);
  }

  if (i != unicode_string.size) {
    fail("utf-8 decode all string lengths differ");
  }

  unicode_string_destroy(&unicode_string);
  string_destroy(&file_contents);
}

void test_utf8_encode_decode_all() {
  String_t string;
  codepoint_t codepoint;
  UnicodeString_t unicode_string = {1, &codepoint};
  for (codepoint_t i = 0; i <= 0x10FFFF; i++) {
    codepoint = i;
    utf8_encode(unicode_string, &string);
    utf8_decode(string, &unicode_string);
    string_destroy(&string);

    assert_codepoint_equals(codepoint, i);
  }
}

void test_utf8_decode_invalid() {
  UnicodeString_t unicode_string;
  // Invalid leading bytes
  assert_int_equals(utf8_decode((String_t){4, "\xF8\x80\x80\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xF9\x80\x80\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xFA\x80\x80\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xFB\x80\x80\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xFC\x80\x80\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xFD\x80\x80\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xFE\x80\x80\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xFF\x80\x80\x80"}, &unicode_string), -1);

  // Missing continuation byte in 2-byte code point
  assert_int_equals(utf8_decode((String_t){4, "\xC0\x00"}, &unicode_string), -1);

  // Missing continuation bytes in 3-byte code point
  assert_int_equals(utf8_decode((String_t){4, "\xE0\x00\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xE0\x80\x00"}, &unicode_string), -1);

  // Missing continuation bytes in 4-byte code point
  assert_int_equals(utf8_decode((String_t){4, "\xF0\x00\x80\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xF0\x08\x00\x80"}, &unicode_string), -1);
  assert_int_equals(utf8_decode((String_t){4, "\xF0\x08\x80\x00"}, &unicode_string), -1);

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
    assert_int_equals(utf8_decode((String_t){5, encoded_string}, &unicode_string), -1);
  }
}

void test_utf16_encode_all(const char *filename) {
  codepoint_t codepoints[0x110000];
  UnicodeString_t unicode_string = { 0, codepoints };
  for (codepoint_t i = 0; i < 0x110000; i++) {
    if (IN_SURROGATE_RANGE(i)) {
      continue;
    }

    codepoints[unicode_string.size++] = i;
  }

  String_t string;
  if (utf16_encode(unicode_string, &string) != 0) {
    fail("encode all utf16_encode failed");
  }

  FILE *file = fopen(filename, "r");
  String_t file_contents = read_file(file);
  fclose(file);

  if (string.size != file_contents.size ||
      memcmp(string.buffer, file_contents.buffer, string.size) != 0) {
    fail("utf-16 encode all strings differ");
  }

  string_destroy(&string);
  string_destroy(&file_contents);
}

void test_utf16_decode_all(const char *filename) {
  FILE *file = fopen(filename, "r");
  String_t file_contents = read_file(file);
  fclose(file);

  UnicodeString_t unicode_string;
  if (utf16_decode(file_contents, &unicode_string) != 0) {
    fail("decode all utf16_decode failed");
  }

  size_t i = 0;
  for (codepoint_t codepoint = 0; codepoint <= 0x10FFFF; codepoint++) {
    if (IN_SURROGATE_RANGE(codepoint)) {
      continue;
    }

    assert_codepoint_equals(unicode_string.buffer[i++], codepoint);
  }

  if (i != unicode_string.size) {
    fail("utf-16 decode all string lengths differ");
  }

  unicode_string_destroy(&unicode_string);
  string_destroy(&file_contents);
}

void test_utf16_encode_decode_all() {
  String_t string;
  codepoint_t codepoint;
  UnicodeString_t unicode_string = {1, &codepoint};
  for (codepoint_t i = 0; i <= 0x10FFFF; i++) {
    codepoint = i;
    utf16_encode(unicode_string, &string);
    utf16_decode(string, &unicode_string);
    string_destroy(&string);

    assert_codepoint_equals(codepoint, i);
  }
}

void test_utf16_decode_invalid() {
  UnicodeString_t unicode_string;
  // First byte cannot be a low surrogate
  for (codepoint_t i = 0xDC00; i < 0xDFFF; i++) {
    char encoded_string[4] = {i, i >> 8, 0x00, 0xDC};
    assert_int_equals(utf16_decode((String_t){4, encoded_string}, &unicode_string), -1);
  }

  // Second byte cannot be a high surrogate
  for (codepoint_t i = 0xD800; i < 0xDBFF; i++) {
    char encoded_string[4] = {0x00, 0xD8, i, i >> 8};
    assert_int_equals(utf16_decode((String_t){4, encoded_string}, &unicode_string), -1);
  }
}

void test_utf32_encode_decode_all() {
  String_t string;
  codepoint_t codepoint;
  UnicodeString_t unicode_string = {1, &codepoint};
  for (codepoint_t i = 0; i <= 0x10FFFF; i++) {
    codepoint = i;
    utf32_encode(unicode_string, &string);
    utf32_decode(string, &unicode_string);
    string_destroy(&string);

    assert_codepoint_equals(codepoint, i);
  }
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage: test <utf8-file> <utf16-file>\n");
    exit(1);
  }

  char *utf8_file = argv[1];
  char *utf16_file = argv[2];

  test_utf8_encode_all(utf8_file);
  test_utf8_decode_all(utf8_file);
  test_utf8_encode_decode_all();
  test_utf8_decode_invalid();

  test_utf16_encode_all(utf16_file);
  test_utf16_decode_all(utf16_file);
  test_utf16_encode_decode_all();
  test_utf16_decode_invalid();

  test_utf32_encode_decode_all();
}