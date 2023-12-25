/**
 * #FujiNet ISS tracker for C64
 *
 * @author  Thomas Cherryhomes, Leif Bloomquist
 * @email   thom dot cherryhomes at gmail dot com
 * @license gpl v. 3
 *
 * Satellite display
 * 
 * LB: Added Added lookup tables for lat/lon
 */

#include <c64.h>
#include <6502.h>
#include <string.h>
#include <stdio.h>

#define PPORT     ((unsigned char *)0x0001) // Processor port (RAM selection)
#define SPRD      ((unsigned char *)0xC000) // Sprite Data location
#define SPRDP     ((unsigned char *)0xD3F8) // Sprite data pointer relative to SPRD
#define SPRX      ((unsigned char *)0xD000) // X position (lower 8 bits)
#define SPRY      ((unsigned char *)0xD001) // Y position (lower 8 bits)
#define SPRXX     ((unsigned char *)0xD010) // X position (upper 1 bit)
#define SPRENABLE ((unsigned char *)0xD015) // Sprite enable bits
#define SPREX     ((unsigned char *)0xD017) // Sprite expansion bits for X
#define SPREY     ((unsigned char *)0xD01D) // Sprite expansion bits for Y
#define SPRCOLR   ((unsigned char *)0xD027) // Sprite color

const unsigned char _satellite[63] = {
  0x20, 0x00, 0x00, /* ..X.|.... */
  0x50, 0x00, 0x00, /* .X.X|.... */
  0xA4, 0x00, 0x00, /* X.X.|.X.. */
  0x58, 0x00, 0x00, /* .X.X|X... */
  0x1A, 0x00, 0x00, /* ...X|X.X. */
  0x05, 0x00, 0x00, /* ....|.X.X */
  0x0A, 0x00, 0x00, /* ....|X.X. */
  0x04, 0x00, 0x00, /* ....|.X.. */
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00,
  0x00, 0x00, 0x00
};

const unsigned short longitude_to_x[361] = { 16, 16, 17, 18, 19, 20, 21, 22, 23, 23, 24, 25, 26, 27, 28, 29, 30, 31, 31, 32, 33, 34, 35, 36, 37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 47, 48, 49, 50, 51, 52, 53, 54, 55, 55, 56, 57, 58, 59, 60, 61, 62, 62, 63, 64, 65, 66, 67, 68, 69, 70, 70, 71, 72, 73, 74, 75, 76, 77, 78, 78, 79, 80, 81, 82, 83, 84, 85, 86, 86, 87, 88, 89, 90, 91, 92, 93, 94, 94, 95, 96, 97, 98, 99, 100, 101, 101, 102, 103, 104, 105, 106, 107, 108, 109, 109, 110, 111, 112, 113, 114, 115, 116, 117, 117, 118, 119, 120, 121, 122, 123, 124, 125, 125, 126, 127, 128, 129, 130, 131, 132, 133, 133, 134, 135, 136, 137, 138, 139, 140, 140, 141, 142, 143, 144, 145, 146, 147, 148, 148, 149, 150, 151, 152, 153, 154, 155, 156, 156, 157, 158, 159, 160, 161, 162, 163, 164, 164, 165, 166, 167, 168, 169, 170, 171, 172, 172, 173, 174, 175, 176, 177, 178, 179, 179, 180, 181, 182, 183, 184, 185, 186, 187, 187, 188, 189, 190, 191, 192, 193, 194, 195, 195, 196, 197, 198, 199, 200, 201, 202, 203, 203, 204, 205, 206, 207, 208, 209, 210, 211, 211, 212, 213, 214, 215, 216, 217, 218, 218, 219, 220, 221, 222, 223, 224, 225, 226, 226, 227, 228, 229, 230, 231, 232, 233, 234, 234, 235, 236, 237, 238, 239, 240, 241, 242, 242, 243, 244, 245, 246, 247, 248, 249, 250, 250, 251, 252, 253, 254, 255, 256, 257, 257, 258, 259, 260, 261, 262, 263, 264, 265, 265, 266, 267, 268, 269, 270, 271, 272, 273, 273, 274, 275, 276, 277, 278, 279, 280, 281, 281, 282, 283, 284, 285, 286, 287, 288, 289, 289, 290, 291, 292, 293, 294, 295, 296, 296, 297, 298, 299, 300, 301, 302, 303, 304, 304, 305, 306, 307, 308, 309, 310, 311, 312, 312, 313, 314, 315, 316, 317, 318, 319, 320, 320, 321, 322, 323, 324, 325, 326, 327, 328, 328, 329, 330, 331, 332, 333, 334, 335 };
const unsigned char  latitude_to_y[181] = { 205, 204, 203, 202, 201, 200, 199, 198, 197, 196, 196, 195, 194, 193, 192, 191, 190, 189, 188, 187, 186, 186, 185, 184, 183, 182, 181, 180, 179, 178, 177, 177, 176, 175, 174, 173, 172, 171, 170, 169, 168, 167, 167, 166, 165, 164, 163, 162, 161, 160, 159, 158, 157, 157, 156, 155, 154, 153, 152, 151, 150, 149, 148, 148, 147, 146, 145, 144, 143, 142, 141, 140, 139, 138, 138, 137, 136, 135, 134, 133, 132, 131, 130, 129, 128, 128, 127, 126, 125, 124, 123, 122, 121, 120, 119, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 109, 108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 42 };

#define LONGITUDE_LIMIT 180
#define LATITUDE_LIMIT  90

/**
 * @brief Convert longitude to X coordinate
 * @param lon Longitude
 * @return X coordinate
 */
unsigned short lon_to_x(int lon)
{
  if (lon < -LONGITUDE_LIMIT) lon = -LONGITUDE_LIMIT;
  if (lon >  LONGITUDE_LIMIT) lon =  LONGITUDE_LIMIT;

  return longitude_to_x[LONGITUDE_LIMIT + lon];
}

/**
 * @brief Convert latitude to Y coordinate
 * @param lat latitude
 * @return Y coordinate
 */
unsigned char lat_to_y(int lat)
{
  if (lat < -LATITUDE_LIMIT) lat = -LATITUDE_LIMIT;
  if (lat >  LATITUDE_LIMIT) lat =  LATITUDE_LIMIT;

  return latitude_to_y[LATITUDE_LIMIT + lat];
}

/**
 * @brief Display satellite at lon, lat
 * @param lon Integer longitude (-180, 180 deg)
 * @param lat Integer latitude  (-90, 90 deg)
 */
void satellite(int lon, int lat)
{
  unsigned short x = lon_to_x(lon);
  unsigned char  y = lat_to_y(lat);
  unsigned char  b = *PPORT;

  // Copy sprite to $C000
  memcpy(SPRD,_satellite,sizeof(_satellite));

  // Set sprite position, and expansions
  *SPRY=y;
  *SPRX=x&0xFF;
  *SPREX=0;
  *SPREY=0;

  // Set high bit if needed.
  if (x>255)
    *SPRXX=0x01;

  // Enable sprite
  *SPRENABLE=0x01;
  *SPRCOLR=0x01;

  // Swap in RAM, so that we can set the sprite pointer
  SEI();
  *PPORT = 0b00110000;
  *SPRDP = 0x00;
  *PPORT = b;
  CLI();
}
