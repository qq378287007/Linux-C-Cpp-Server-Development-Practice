#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

typedef struct _IP_HEADER // IP头定义，共20个字节
{
	char m_cVersionAndHeaderLen; // 版本信息(前4位)，头长度(后4位)
	char m_cTypeOfService;		 // 服务类型8位
	short m_sTotalLenOfPacket;	 // 数据包长度
	short m_sPacketID;			 // 数据包标识
	short m_sSliceinfo;			 // 分片使用
	char m_cTTL;				 // 存活时间
	char m_cTypeOfProtocol;		 // 协议类型
	short m_sCheckSum;			 // 校验和
	unsigned int m_uiSourIp;	 // 源IP地址
	unsigned int m_uiDestIp;	 // 目的IP地址
} IP_HEADER, *PIP_HEADER;

typedef struct _UDP_HEADER // UDP头定义，共8个字节
{
	unsigned short m_usSourPort; // 源端口号16bit
	unsigned short m_usDestPort; // 目的端口号16bit
	unsigned short m_usLength;	 // 数据包长度16bit
	unsigned short m_usCheckSum; // 校验和16bit
} UDP_HEADER, *PUDP_HEADER;

int main()
{
	// 设置地址信息，ip信息
	int size = sizeof(struct sockaddr_in);
	struct sockaddr_in saddr;
	memset(&saddr, 0, size);
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = inet_addr("192.168.0.153"); // 本机的IP地址，但和发送端设定的目的IP地址不同。
	saddr.sin_port = htons(8888);

	// 创建udp 的套接字
	int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); // 该原始套接字使用ICMP协议
	if (sockfd < 0)
	{
		perror("socket failed");
		return -1;
	}

	// 设置端口复用
	char on = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	// 绑定地址信息，ip信息
	int ret = bind(sockfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
	if (ret < 0)
	{
		perror("bind failed");
		return -1;
	}

	struct sockaddr_in raddr;
	int val = sizeof(struct sockaddr);
	char rbuf[500];
	IP_HEADER iph;
	UDP_HEADER udph;
	while (1) // 接收客户端发来的消息
	{
		puts("waiting data");
		ret = recvfrom(sockfd, rbuf, 500, 0, (struct sockaddr *)&raddr, (socklen_t *)&val);
		if (ret < 0)
		{
			printf("recvfrom failed:%d", errno);
			return -1;
		}
		memcpy(&iph, rbuf, 20);
		memcpy(&udph, rbuf + 20, 8);

		int srcp = ntohs(udph.m_usSourPort);
		struct in_addr ias, iad;
		ias.s_addr = iph.m_uiSourIp;
		iad.s_addr = iph.m_uiDestIp;
		char strDip[50] = "";
		strcpy(strDip, inet_ntoa(iad));
		printf("(sIp=%s, sPort=%d), \n(dIp=%s, dPort=%d)\n", inet_ntoa(ias), ntohs(udph.m_usSourPort), strDip, ntohs(udph.m_usDestPort));
		printf("recv data : %s\n", rbuf + 28);
	}

	close(sockfd); // 关闭原始套接字

	return 0;
}
