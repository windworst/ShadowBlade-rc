#pragma once

#define CONFIG_FLAG "IHGFEDCBA"
#define CONFIG_FLAG_LEN 16
#define CONFIG_URL_LEN 256
#define CONFIG_LISTENPORT 16
#define CONFIG_NAME_LEN 16
#define CONFIG_CONNECT_TIMEWAIT 16
#define CONFIG_CONNECT_TIMEOUT 16
#define CONFIG_PASSWD_LEN 16

typedef struct
{
	char flag[CONFIG_FLAG_LEN];
	char url[CONFIG_URL_LEN];
	char listenport[CONFIG_LISTENPORT];
	char name[CONFIG_NAME_LEN];
	char passwd[CONFIG_PASSWD_LEN];
	char timewait[CONFIG_CONNECT_TIMEWAIT];
	char timeout[CONFIG_CONNECT_TIMEOUT];
}raw_config;

typedef struct
{
	char *url;
	char *name;
	char *passwd;
	long listenport;
	long timewait;
	long timeout;
}config;

extern config g_config;
extern raw_config g_raw_config;

void read_config(raw_config* raw,config* conf);
