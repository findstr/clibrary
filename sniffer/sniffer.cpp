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

	// ���� SOCK_RAW Ϊ SIO_RCVALL���������е� IP ��
	DWORD dwBufferLen[10];
	DWORD dwBufferInLen = 1;
	DWORD dwBytesReturned = 0;
	err = WSAIoctl(sniffer.skt, SIO_RCVALL, &dwBufferInLen, sizeof(dwBufferInLen), &dwBufferLen,
		sizeof(dwBufferLen), &dwBytesReturned, NULL, NULL);
	if (SOCKET_ERROR == err)
		return -1;

	return 0;
}





struct udphdr        //����UDP�ײ�
{
	unsigned short uh_sport;
	unsigned short uh_dport;
	unsigned short uh_len;
	unsigned short uh_sum;
} UDP_HEADER;

struct icmphdr        //����ICMP�ײ�
{
	BYTE i_type;            //8λ����
	BYTE i_code;            //8λ����
	USHORT i_cksum;            //16λУ���
	USHORT i_id;            //ʶ��ţ�һ���ý��̺���Ϊʶ��ţ�
	USHORT i_seq;            //�������к�
	ULONG timestamp;        //ʱ���
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
		unsigned char h_lenver;        //4λ�ײ�����+4λIP�汾��
		unsigned char tos;            //8λ��������TOS
		unsigned short total_len;    //16λ�ܳ��ȣ��ֽڣ�
		unsigned short ident;        //16λ��ʶ
		unsigned short frag_and_flags; //3λ��־λ
		unsigned char ttl;            //8λ����ʱ�� TTL
		unsigned char proto;        //8λЭ�� (TCP, UDP ������)
		unsigned short checksum;    //16λIP�ײ�У���
		unsigned int sourceIP;        //32λԴIP��ַ
		unsigned int destIP;        //32λĿ��IP��ַ
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
		USHORT th_sport;        //16λԴ�˿�
		USHORT th_dport;        //16λĿ�Ķ˿�
		unsigned int th_seq;
		unsigned int th_ack;
		unsigned char th_lenres;//4λ�ײ�����/6λ������
		unsigned char th_flag;    //6λ��־λ
		USHORT th_win;            //16λ���ڴ�С
		USHORT th_sum;            //16λУ���
		USHORT th_urp;            //16λ��������ƫ����
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