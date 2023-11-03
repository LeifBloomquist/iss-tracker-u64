/**
 * ISS tracker for Ultimate 64
 * @brief Satellite position fetch functions
 */

#include <c64.h>
#include <cbm.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "network.h"
#include "json.h"
#include "defines.h"

const char host[]="api.open-notify.org";
const char path[] ="/iss-now.json";
const int port = 80;

static char tmp[MAX_DATA_SIZE];

/**
 * @brief fetch satellite telemetry, and return as ints and strings
 * @param lon pointer to integer longitude
 * @param lat pointer to integer latitude
 * @param lon_s pointer to longitude string (for showing floating point val)
 * @param lat_s pointer to latitude string (for showing floating point val)
 * @param ts pointer to timestamp
 */
void fetch(int *lon, int *lat, char *lon_s, char *lat_s, unsigned long *ts)
{
  int received = 0;

  memset(tmp,0,sizeof(tmp));
  memset(lon_s,0, MAX_RESULT_SIZE);
  memset(lat_s,0, MAX_RESULT_SIZE);

  received = http_fetch(host, path, port, tmp);

  if (received <= 0)
  {
    return;
  }

  printf("Received %d bytes:\n%s\n\n", received, tmp);

  return;

  get_json_value_string(tmp, "\"latitude\"", lat_s);
  get_json_value_string(tmp, "\"longitude\"", lon_s);
  get_json_value_long(tmp, "\"timestamp\"", (unsigned long*)ts);

  *lat = atoi(lat_s);
  *lon = atoi(lon_s);

  return;

  /* Old Fujinet code for reference
  
  cbm_open(CMD,DEV,CMD,"");
  cbm_open(LFN,DEV,SAN,url);

  cbm_write(CMD,cmd_parse,sizeof(cmd_parse));

  cbm_write(CMD,ts_query,sizeof(ts_query));
  memset(tmp,0,sizeof(tmp));
  cbm_read(LFN,tmp,sizeof(tmp));
  *ts = atol(tmp);

  cbm_write(CMD,lon_query,sizeof(lon_query));
  memset(tmp,0,sizeof(tmp));
  cbm_read(LFN,tmp,sizeof(tmp));
  strcpy(lon_s,tmp);
  *lon = atoi(tmp);

  cbm_write(CMD,lat_query,sizeof(lat_query));
  memset(tmp,0,sizeof(tmp));
  cbm_read(LFN,tmp,sizeof(tmp));
  strcpy(lat_s,tmp);
  *lat = atoi(tmp);

  cbm_close(LFN);
  cbm_close(CMD);

  */
}
