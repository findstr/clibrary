// test socket sniffer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

//#include <Winsock2.h>
//#include <mstcpip.h>
//
//#pragma comment(lib,"WS2_32.lib")
//
//typedef struct _iphdr
//{
//	unsigned char h_lenver;        //4λ�ײ�����+4λIP�汾��
//	unsigned char tos;            //8λ��������TOS
//	unsigned short total_len;    //16λ�ܳ��ȣ��ֽڣ�
//	unsigned short ident;        //16λ��ʶ
//	unsigned short frag_and_flags; //3λ��־λ
//	unsigned char ttl;            //8λ����ʱ�� TTL
//	unsigned char proto;        //8λЭ�� (TCP, UDP ������)
//	unsigned short checksum;    //16λIP�ײ�У���
//	unsigned int sourceIP;        //32λԴIP��ַ
//	unsigned int destIP;        //32λĿ��IP��ַ
//}IP_HEADER;
//
//typedef struct _tcphdr        //����TCP�ײ�
//{
//	USHORT th_sport;        //16λԴ�˿�
//	USHORT th_dport;        //16λĿ�Ķ˿�
//	unsigned int th_seq;
//	unsigned int th_ack;
//	unsigned char th_lenres;//4λ�ײ�����/6λ������
//	unsigned char th_flag;    //6λ��־λ
//	USHORT th_win;            //16λ���ڴ�С
//	USHORT th_sum;            //16λУ���
//	USHORT th_urp;            //16λ��������ƫ����
//}TCP_HEADER;
//
//typedef struct _udphdr        //����UDP�ײ�
//{
//	unsigned short uh_sport;
//	unsigned short uh_dport;
//	unsigned short uh_len;
//	unsigned short uh_sum;
//} UDP_HEADER;
//
//typedef struct _icmphdr        //����ICMP�ײ�
//{
//	BYTE i_type;            //8λ����
//	BYTE i_code;            //8λ����
//	USHORT i_cksum;            //16λУ���
//	USHORT i_id;            //ʶ��ţ�һ���ý��̺���Ϊʶ��ţ�
//	USHORT i_seq;            //�������к�
//	ULONG timestamp;        //ʱ���
//}ICMP_HEADER;
//
//bool DecodeIpPack(const char *, int);    // IP ���
//bool DecodeTcpPack(const char *);        // TCP ���
//bool DecodeUdpPack(const char *);        // UDP ���
//bool DecodeIcmpPack(const char *);        // ICMP ���
//const char * CheckProtocol(int);// ��ѯЭ��
//

#include "sniffer.h"
int _tmain(int argc, _TCHAR* argv[])
{
	const unsigned char *buff;
	unsigned long size;
	char src_ip[32];
	char dst_ip[32];
	char src_port[32];
	char dst_port[32];
	sniffer_init();

	sniffer_dst("127.0.0.1", "8989");

	while (1) {
		buff = sniffer_get(src_ip, src_port, dst_ip, dst_port, &size);
		if (buff) 
			printf("src:%s:%s-->dst:%s:%s, data_len:%d\n", src_ip, src_port, dst_ip, dst_port, size);
	}
}