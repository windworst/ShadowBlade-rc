#include "rc4.h"

#define S_LEN 256

/* ��ʼ������ */
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
        s[i] = s[j]; //����s[i]��s[j]
        s[j] = tmp;
    }
}

/* �ӽ��� */
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
        s[i] = s[j]; //����s[x]��s[y]
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
    char pData[512] = "����һ���������ܵ�����Data";
    unsigned len = strlen(pData);
    int i;

    printf("pData = %s\n", pData);
    printf("key = %s, length = %d\n\n", key, strlen(key));
    rc4_init(s, (unsigned char *)key, strlen(key)); //�Ѿ�����˳�ʼ��
    printf("��ɶ�S[i]�ĳ�ʼ�������£�\n\n");
    for (i = 0; i < 256; i++)
    {
        printf("%02X ", s[i]);
        if (i && (i + 1) % 16 == 0) putchar('\n');
    }
    printf("\n\n");
    for(i = 0; i < 256; i++) //��s2[i]��ʱ����������ʼ����s[i]������Ҫ�ģ�����
    {
        s2[i] = s[i];
    }
    printf("�Ѿ���ʼ�������ڼ���:\n\n");
    rc4_crypt(s, (unsigned char *)pData, len); //����
    printf("pData = %s\n\n", pData);
    printf("�Ѿ����ܣ����ڽ���:\n\n");
    //rc4_init(s, (unsigned char *)key, strlen(key)); //��ʼ����Կ
    rc4_crypt(s2, (unsigned char *)pData, len); //����
    printf("pData = %s\n\n", pData);
    return 0;
}

#endif
