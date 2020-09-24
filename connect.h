#pragma once
#include <stdlib.h>
#include <arpa/inet.h>

void logError(const char *msg);
int init_server_sockaddr(int iPversion, const char *portstr, struct sockaddr_storage *storage);

int init_server(char *port, struct sockaddr_storage *saddr_storage);

int addr_parse(const char *addrstr, const char *portstr, struct sockaddr_storage *storage);

void addrtostr(const struct sockaddr *addr);