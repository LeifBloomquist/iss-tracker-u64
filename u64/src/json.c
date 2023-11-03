#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The world's crudest JSON parser, doesn't know anything about structure

// Find a string value in JSON and return it
// name must include the "quotes"
// Returns 1 on success or 0 on failure
// If found, value copied to a string in value_out

int get_json_value_string(char *json_str, char *name, char *value_out)
{
    char* ptr = json_str;
    long value;

    while (ptr) 
    {
        ptr = strstr(ptr, name);
        if (ptr == NULL) {
            break;
        }
        ptr = strchr(ptr, ':');
        if (ptr == NULL) {
            break;
        }
        ptr++;
        value = strtol(ptr, &ptr, 10);
        if (*ptr != '}') {
            break;
        }
        ptr++;
        printf("%lu\n", value);
        return 1;
    }
    
    return 0;
}

// Find a string value in JSON and return it
// name must include the "quotes"
// Returns 1 on success or 0 on failure
// If found, value copies to a string in value_out

int get_json_value_long(char* json_str, char* name, unsigned long *value_out)
{
  char* ptr = json_str;
  long value;

  while (ptr) 
  {
    ptr = strstr(ptr, name);
    if (ptr == NULL) {
      break;
    }
    ptr = strchr(ptr, ':');
    if (ptr == NULL) {
      break;
    }
    ptr+=2;   // ": "
    value = atol(ptr);  // Stops at first invalid character, TODO test
    
    *value_out = value;
    return 1;
  }

  return 0;
}