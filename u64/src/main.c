/**
 *  ISS Tracker for Ultimate 64.  Based on  #FujiNet ISS tracker for C64
 * 
 * @author Leif Bloomquist, Thomas Cherryhomes
 * @license gpl v. 3
 */

#include <c64.h>
#include <tgi.h>
#include <time.h>
#include <stdbool.h>
#include <stdio.h>
#include <conio.h>
#include <unistd.h>
#include "map.h"
#include "osd.h"
#include "satellite.h"
#include "network.h"
#include "fetch.h"

struct timespec tm;
unsigned long ts;

char lon_s[16], lat_s[16];
int lon, lat;

void main(void)
{
  int seconds = 0;

  clrscr();
  printf("ISS Tracker for Ultimate 64\n");
  printf("Built: % s % s\n\n", __DATE__, __TIME__);

  network_init();

  printf("\nPress any key...\n\n");
  cgetc();

  // tgi_install(c64_hi_tgi);
  // tgi_init();
  // tgi_clear();

  while (true)
  {
    tm.tv_sec = tm.tv_nsec = 0;
    clock_settime(CLOCK_REALTIME, &tm);

    //   map();
    fetch(&lon,&lat,lon_s,lat_s,&ts);
    //  osd(lon_s,lat_s,ts);
    //  satellite(lon,lat);

    sleep(10);
  }
}