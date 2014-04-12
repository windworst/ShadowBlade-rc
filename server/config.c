#include "config.h"
#include <stdio.h>
raw_config g_raw_config=
{
	CONFIG_FLAG,
	"127.0.0.1",
	"ShadowBlade",
	"firstblood",
	"10",
	"10",
};
config g_config;

void read_config(raw_config* raw,config* conf)
{
	conf->url = raw->url;
	conf->name = raw->name;
	conf->passwd = raw->passwd;
	sscanf(raw->timewait,"%d",&conf->timewait);
	sscanf(raw->timeout,"%d",&conf->timeout);
}
