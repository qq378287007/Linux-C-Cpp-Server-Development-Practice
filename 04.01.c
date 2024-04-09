#include <stdio.h>
#include <arpa/inet.h>

int main()
{
	struct in_addr ia;
	ia.s_addr = inet_addr("172.16.2.6");

	printf("ia.s_addr = 0x%x\n", ia.s_addr);
	printf("real_ip = %s\n", inet_ntoa(ia));

	return 0;
}
