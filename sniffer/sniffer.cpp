#include <assert.h>
#include <Winsock2.h>
#include <mstcpip.h>
#include <stdio.h>
#include "sniffer.h"

#pragma comment(lib,"WS2_32.lib")

static struct {
	SOCKET	skt;
	unsigned long ip;
	unsigned long port;
	unsigned char buff[65536];
} sniffer;

int sniffer_init()
{
	int err;
	WSADATA wsaData;
	err = WSAStartup(MAKEWORD(2, 1), &wsaData);
	if (SOCKET_ERROR == err)
		return err;

	sniffer.ip = inet_addr("127.0.0.1");
	sniffer.port = 23456;
	sniffer.skt = INVALID_SOCKET;

	return 0;
}

int sniffer_dst(const char *ip, const char *port)
{
	int err;
	SOCKADDR_IN sa;

	assert(ip);
	assert(port);

	sniffer.ip = inet_addr(ip);

	if (port[0] == '0' && (port[1] == 'x' || port[1] == 'X'))
		sniffer.port = strtoul(port, NULL, 16);
	else
		sniffer.port = strtoul(port, NULL, 10);
	
	if (sniffer.skt != INVALID_SOCKET)
		closesocket(sniffer.skt);

	sniffer.skt = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
	if (INVALID_SOCKET == sniffer.skt)
		return SOCKET_ERROR;

	sa.sin_family = AF_INET;
	sa.sin_addr.S_un.S_addr = sniffer.ip;
	sa.sin_port = htons(8989);

	err = bind(sniffer.skt, (PSOCKADDR)&sa, sizeof(sa));
	if (SOCKET_ERROR == err)
		return -1;

	// 设置 SOCK_RAW 为 SIO_RCVALL，接收所有的 IP 包
	DWORD dwBufferLen[10];
	DWORD dwBufferInLen = 1;
	DWORD dwBytesReturned = 0;
	err = WSAIoctl(sniffer.skt, SIO_RCVALL, &dwBufferInLen, sizeof(dwBufferInLen), &dwBufferLen,
		sizeof(dwBufferLen), &dwBytesReturned, NULL, NULL);
	if (SOCKET_ERROR == err)
		return -1;

	return 0;
}





struct udphdr        //定义UDP首部
{
	unsigned short uh_sport;
	unsigned short uh_dport;
	unsigned short uh_len;
	unsigned short uh_sum;
} UDP_HEADER;

struct icmphdr        //定义ICMP首部
{
	BYTE i_type;            //8位类型
	BYTE i_code;            //8位代码
	USHORT i_cksum;            //16位校验和
	USHORT i_id;            //识别号（一般用进程号作为识别号）
	USHORT i_seq;            //报文序列号
	ULONG timestamp;        //时间戳
};

static int rcv_ip(SOCKET skt, unsigned char *buff, unsigned long size)
{
	int err;

	assert(buff);
	assert(size >= 65536);

	err = recv(skt, (char *)buff, size, 0);
	if (err < 0)
		return -1;

	return 0;
}

static int check_iphdr(const unsigned char *buff, unsigned long *src_ip, unsigned long *dst_ip, unsigned long *data_len)
{
	struct iphdr {
		unsigned char h_lenver;        //4位首部长度+4位IP版本号
		unsigned char tos;            //8位服务类型TOS
		unsigned short total_len;    //16位总长度（字节）
		unsigned short ident;        //16位标识
		unsigned short frag_and_flags; //3位标志位
		unsigned char ttl;            //8位生存时间 TTL
		unsigned char proto;        //8位协议 (TCP, UDP 或其他)
		unsigned short checksum;    //16位IP首部校验和
		unsigned int sourceIP;        //32位源IP地址
		unsigned int destIP;        //32位目的IP地址
	};

	unsigned long hdr_len;
	struct iphdr *ip;
	
	assert(buff);
	assert(src_ip);
	assert(dst_ip);
	assert(data_len);
	
	ip = (struct iphdr *)buff;

	if (ip->destIP == sniffer.ip || ip->sourceIP == sniffer.ip) {

		hdr_len = (ip->h_lenver & 0x0f) * sizeof(unsigned long);

		*src_ip = ip->sourceIP;
		*dst_ip = ip->destIP;
		*data_len = ntohs(ip->total_len);

		return hdr_len;
	}

	return -1;
}

static int check_tcphdr(const unsigned char *buff, unsigned long src_ip, unsigned long dst_ip, unsigned long *src_port, unsigned long *dst_port)
{
	struct tcphdr {
		USHORT th_sport;        //16位源端口
		USHORT th_dport;        //16位目的端口
		unsigned int th_seq;
		unsigned int th_ack;
		unsigned char th_lenres;//4位首部长度/6位保留字
		unsigned char th_flag;    //6位标志位
		USHORT th_win;            //16位窗口大小
		USHORT th_sum;            //16位校验和
		USHORT th_urp;            //16位紧急数据偏移量
	};
	struct tcphdr *tcp;

	assert(buff);
	assert(src_port);
	assert(dst_port);

	tcp = (struct tcphdr *)buff;

	if (src_ip == sniffer.ip && ntohs(tcp->th_sport) == sniffer.port || 
		dst_ip == sniffer.ip && ntohs(tcp->th_dport) == sniffer.port) {
		*src_port = ntohs(tcp->th_sport);
		*dst_port = ntohs(tcp->th_dport);
		return sizeof(unsigned long) * (tcp->th_lenres & 0x0f);
	}

	return -1;
}

const unsigned char *sniffer_get(	
					char sz_src_ip[32],
					char sz_src_port[8],
					char sz_dst_ip[32],
					char sz_dst_port[8],
					unsigned long *size
					)
{
	int err;
	unsigned long src_ip, dst_ip;
	unsigned long src_port, dst_port;
	unsigned long data_len;
	unsigned char *tmp;

	assert(size);
	assert(sz_src_ip);
	assert(sz_src_port);
	assert(sz_dst_ip);
	assert(sz_dst_port);
	
	do {
		err = rcv_ip(sniffer.skt, sniffer.buff, sizeof(sniffer.buff));
		if (err < 0)
			continue;
		
		err = check_iphdr(sniffer.buff, &src_ip, &dst_ip, &data_len);
		if (err < 0)
			continue;
		tmp = sniffer.buff + err;
		data_len -= err;

		err = check_tcphdr(tmp, src_ip, dst_ip, &src_port, &dst_port);
		if (err < 0)
			continue;
		tmp = sniffer.buff + err;
		data_len -= err;
		break;
		Sleep(0);
	} while(1);
	
	*size = data_len;
	strncpy(sz_src_ip, inet_ntoa(((in_addr *)&src_ip)[0]), 32);
	strncpy(sz_dst_ip, inet_ntoa(((in_addr *)&dst_ip)[0]), 32);
	sprintf(sz_src_port, "%d", src_port);
	sprintf(sz_dst_port, "%d", dst_port);

	return tmp;
}

int sniffer_exit()
{
	closesocket(sniffer.skt);
	WSACleanup();

	return 0;
}