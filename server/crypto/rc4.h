#pragma once

void rc4_init(unsigned char *s, unsigned char *key, unsigned long key_len);
void rc4_crypt(unsigned char *s, unsigned char *data, unsigned long data_len);
