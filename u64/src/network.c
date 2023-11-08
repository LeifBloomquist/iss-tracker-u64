#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c64.h>
#include <peekpoke.h>
#include "network.h"
#include "ultimate_lib.h"
#include "defines.h"

typedef unsigned char byte;

int get_uii_status()
{
	int status = -1;
	char* ptr = NULL;
	char temp[100];
	strcpy(temp, uii_status);

	ptr = strchr(temp, ',');
	*ptr = '\0';

	status = atoi(temp);
	//printf("\n%cDEBUG: [%s] [%s] [%d]\n", CG_PUR, uii_status, temp, status);
	return status;
}

void network_init()
{
	int status = 0;

	if (uii_isdataavailable())
	{
		printf("Aborting a previous U64 command...\n");
		uii_abort();
	}

	printf("Identifying...");
	uii_identify();

	status = get_uii_status();
	if (status == 0)
	{
		printf("%s\n", uii_data);
	}
	else
	{
		printf("\nError! Status: %s (%s)\n", uii_data, uii_status);
	}

	printf("\nInitializing Network...");

	uii_getinterfacecount();
	uii_getipaddress();

	status = get_uii_status();
	if (status == 0)
	{
		printf("%s\n", uii_status);
	}
	else
	{
		printf("\nStatus: %s (%s)\n", uii_data, uii_status);
	}

	printf("\nIP Address: %d.%d.%d.%d\n", uii_data[0], uii_data[1], uii_data[2], uii_data[3]);
	printf("   Netmask: %d.%d.%d.%d\n", uii_data[4], uii_data[5], uii_data[6], uii_data[7]);
	printf("   Gateway: %d.%d.%d.%d\n", uii_data[8], uii_data[9], uii_data[10], uii_data[11]);
}

int http_fetch(const char* host, const char* path, int port, char* result)
{
	char query[100];
	int status = 0;
	int received = 0;
	char c = 0;
	byte socketnr = 0;
	char* start_of_data;

	sprintf(query, "GET %s HTTP/1.1\nHost: %s\nConnection: close\n\n", path, host);

	socketnr = uii_tcpconnect((char*)host, port);

	status = get_uii_status();
	if (status != 0)
	{
		POKE(BORDER, COLOR_RED);
		printf("\nStatus: %s\n\n", uii_status);
		printf("*** Failed to Connect to Server ***\n");
		return -2;
	}
	uii_socketwrite_ascii(socketnr, query);

	POKE(BORDER, COLOR_BLACK);

	// TODO - for longer return data, need to be more clever about copying received data to a local buffer
	// This works for small messages under MAX_DATA_SIZE
	while (1)
	{
		int header_length=0;

		POKE(BORDER, COLOR_GRAY1);
		received = uii_socketread(socketnr, MAX_DATA_SIZE);
		//printf("Received %d bytes\n", received);

		if (received == -1) continue;  // No data yet
		if (received == 0)  break;     // End of stream

		// Skip over headers
		start_of_data = strstr(uii_data+2, "\n\r\n\r");  // +2 because first two bytes are length of received data

		if (start_of_data == NULL)
		{
			printf("Can't find end of header!\n");
		}
		
		start_of_data += 4;   // Skip over CR and LFs
		header_length = start_of_data - uii_data;
		memcpy(result, start_of_data, strlen(start_of_data));

		uii_socketclose(socketnr);
		POKE(BORDER, COLOR_BLACK);
		return received - header_length;
	}

	// Should never reach here with the current implementation, as we return as soon as any data is received above
	POKE(BORDER, COLOR_YELLOW);
	uii_socketclose(socketnr);
}

// Sanitize the string to remove any non-ASCII characters (replaced by a space) and fix upper/lowercase
// Invalid characters can appear in raw data received over network, and the result messes up strstr()
void sanitize_ascii(char* string)
{
	int i=0;
	char c= ' ';

	for (i = 0; i < strlen(string); i++)
	{
		c = string[i];

		if ((c >= 32) && (c <= 63)) continue;  // Numbers and standard punctuation
		
		if ((c >= 65) && (c <= 90))   // Uppercase to Lowercase
		{
			string[i] = c + 32;
			continue;
		}

		if ((c >= 97) && (c <= 122))   // Lowercase to Uppercase
		{
			string[i] = c - 32;
			continue;
		}

		// Unknown or invalid characters
		string[i] = ' ';
	}
}
