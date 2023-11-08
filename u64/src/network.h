int get_uii_status();
void network_init();
int http_fetch(const char* host, const char* path, int port, char* result);
void sanitize_ascii(char* string);