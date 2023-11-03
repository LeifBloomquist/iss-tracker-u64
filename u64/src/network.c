#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <c64.h>
#include <peekpoke.h>
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

int http_fetch(char* host, char* path, int port, char* result)
{
	char query[100];
	int status = 0;
	int received = 0;
	char c = 0;
	byte socketnr = 0;

	sprintf(query, "GET %s HTTP/1.1\nHost: %s\nConnection: close\n\n", path, host);

	//printf("Query=%s", query);

	socketnr = uii_tcpconnect(host, port);

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
		POKE(BORDER, COLOR_GRAY1);
		received = uii_socketread(socketnr, MAX_DATA_SIZE);
		//printf("Received %d bytes\n", received);

		if (received == -1) continue;  // No data yet
		if (received == 0)  break;     // End of stream

		memcpy(result, uii_data + 2, received);   // +2 because first two bytes are length of received data
		uii_socketclose(socketnr);
		POKE(BORDER, COLOR_BLACK);
		return received;
	}

	// Should never reach here with the current implementation, as we return as soon as any data is received above
	POKE(BORDER, COLOR_YELLOW);
	uii_socketclose(socketnr);
}