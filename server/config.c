#include "config.h"
#include <stdio.h>
raw_config g_raw_config=
{
	CONFIG_FLAG,
	"127.0.0.1:5555",
	"0",
	"ShadowBlade",
	"firstblood",
	"5",
	"5",
};
config g_config;

/*!
 * @brief:			read config from raw_config
 * @author:			xbw
 * @date:			2014_4_12
 * @args:			raw_config(in), config(out)
 */
void read_config(raw_config* raw,config* conf)
{
	conf->url = raw->url;
	conf->name = raw->name;
	conf->passwd = raw->passwd;
	conf->timewait = 0;
	conf->timeout = 0;
	conf->listenport = 0;
	sscanf(raw->timewait,"%d",&conf->timewait);
	sscanf(raw->timeout,"%d",&conf->timeout);
	sscanf(raw->listenport,"%d",&conf->listenport);
}
