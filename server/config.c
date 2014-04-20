#include "config.h"
#include <stdio.h>
raw_config g_raw_config=
{
	CONFIG_FLAG,
	"127.0.0.1:5555",
	"5556",
	"ShadowBlade",
	"firstblood",
	"10",
	"10",
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
	sscanf(raw->timewait,"%d",&conf->timewait);
	sscanf(raw->timeout,"%d",&conf->timeout);
	sscanf(raw->listenport,"%d",&conf->listenport);
}
