#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The world's crudest JSON parser, doesn't know anything about structure

// Find a string value in JSON and return it
// Returns 1 on success or 0 on failure
// If found, value copied to a string in value_out
int get_json_value_string(char *json_str, char *name, char *value_out)
{
  char* ptr = json_str;
  char* end;
  int len;

  while (ptr)
  {
    ptr = strstr(ptr, name);     // Find the name

    if (ptr == NULL) {
      break;
    }

    ptr = strstr(ptr, "\": \""); // Find the end of the name

    if (ptr == NULL) {
      break;
    }

    ptr+=4;                      // Skip over to the first character of the value string
    end = strchr(ptr, '\"');     // Find the second "

    len = end-ptr;
    strncpy(value_out, ptr, len);
    return 1;
  }

  return 0;
}

// Find a long value in JSON and return it
// Returns 1 on success or 0 on failure
// If found, value copied in ulong pointed to by value_out
int get_json_value_long(char* json_str, char* name, unsigned long* value_out)
{
  char* ptr = json_str;
  unsigned long value;

  while (ptr) 
  {
    ptr = strstr(ptr, name);  // Find the name

    if (ptr == NULL) {
      break;
    }

    ptr = strchr(ptr, ':');   // Find the next ":"

    if (ptr == NULL) {
      break;
    }

    ptr+=2;   // ": "         // Skip over to the start of the value
    value = atol(ptr);        // Stops at first invalid character
    
    *value_out = value;
    return 1;
  }

  return 0;
}
