#include "rc4.h"

#define S_LEN 256

/* 初始化函数 */
void rc4_init(unsigned char *s, unsigned char *key, unsigned long key_len)
{
    int i = 0, j = 0;
    char k[S_LEN] = {0};
    unsigned char tmp = 0;
    for(i = 0; i < S_LEN; i++)
    {
        s[i] = i;
        k[i] = key[i % key_len];
    }
    for (i = 0; i < S_LEN; i++)
    {
        j = (j + s[i] + k[i]) % S_LEN;
        tmp = s[i];
        s[i] = s[j]; //交换s[i]和s[j]
        s[j] = tmp;
    }
}

/* 加解密 */
void rc4_crypt(unsigned char *s, unsigned char *data, unsigned long data_len)
{
    int i = 0, j = 0, t = 0;
    unsigned long k = 0;
    unsigned char tmp;
    for(k = 0; k < data_len; k++)
    {
        i = (i + 1) % S_LEN;
        j = (j + s[i]) % S_LEN;
        tmp = s[i];
        s[i] = s[j]; //交换s[x]和s[y]
        s[j] = tmp;
        t = (s[i] + s[j]) % S_LEN;
        data[k] ^= s[t];
    }
}

#ifdef RC4_TEST_MAIN

#include <stdio.h>
#include <string.h>

int main()
{
    unsigned char s[256] = {0}, s2[256] = {0}; //S-box
    char key[256] = {"just for test"};
    char pData[512] = "这是一个用来加密的数据Data";
    unsigned len = strlen(pData);
    int i;

    printf("pData = %s\n", pData);
    printf("key = %s, length = %d\n\n", key, strlen(key));
    rc4_init(s, (unsigned char *)key, strlen(key)); //已经完成了初始化
    printf("完成对S[i]的初始化，如下：\n\n");
    for (i = 0; i < 256; i++)
    {
        printf("%02X ", s[i]);
        if (i && (i + 1) % 16 == 0) putchar('\n');
    }
    printf("\n\n");
    for(i = 0; i < 256; i++) //用s2[i]暂时保留经过初始化的s[i]，很重要的！！！
    {
        s2[i] = s[i];
    }
    printf("已经初始化，现在加密:\n\n");
    rc4_crypt(s, (unsigned char *)pData, len); //加密
    printf("pData = %s\n\n", pData);
    printf("已经加密，现在解密:\n\n");
    //rc4_init(s, (unsigned char *)key, strlen(key)); //初始化密钥
    rc4_crypt(s2, (unsigned char *)pData, len); //解密
    printf("pData = %s\n\n", pData);
    return 0;
}

#endif
