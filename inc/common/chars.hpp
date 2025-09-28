#pragma once

unsigned char chars_length_utf8(unsigned char c);
unsigned char char_is_id_part(unsigned char c);
bool char_is_binary(char c);
bool char_is_octal(char c);
bool char_is_hexa(char c);
unsigned char hexa_char_value(char c);