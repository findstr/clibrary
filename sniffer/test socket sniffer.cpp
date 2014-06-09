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
//	unsigned char h_lenver;        //4位首部长度+4位IP版本号
//	unsigned char tos;            //8位服务类型TOS
//	unsigned short total_len;    //16位总长度（字节）
//	unsigned short ident;        //16位标识
//	unsigned short frag_and_flags; //3位标志位
//	unsigned char ttl;            //8位生存时间 TTL
//	unsigned char proto;        //8位协议 (TCP, UDP 或其他)
//	unsigned short checksum;    //16位IP首部校验和
//	unsigned int sourceIP;        //32位源IP地址
//	unsigned int destIP;        //32位目的IP地址
//}IP_HEADER;
//
//typedef struct _tcphdr        //定义TCP首部
//{
//	USHORT th_sport;        //16位源端口
//	USHORT th_dport;        //16位目的端口
//	unsigned int th_seq;
//	unsigned int th_ack;
//	unsigned char th_lenres;//4位首部长度/6位保留字
//	unsigned char th_flag;    //6位标志位
//	USHORT th_win;            //16位窗口大小
//	USHORT th_sum;            //16位校验和
//	USHORT th_urp;            //16位紧急数据偏移量
//}TCP_HEADER;
//
//typedef struct _udphdr        //定义UDP首部
//{
//	unsigned short uh_sport;
//	unsigned short uh_dport;
//	unsigned short uh_len;
//	unsigned short uh_sum;
//} UDP_HEADER;
//
//typedef struct _icmphdr        //定义ICMP首部
//{
//	BYTE i_type;            //8位类型
//	BYTE i_code;            //8位代码
//	USHORT i_cksum;            //16位校验和
//	USHORT i_id;            //识别号（一般用进程号作为识别号）
//	USHORT i_seq;            //报文序列号
//	ULONG timestamp;        //时间戳
//}ICMP_HEADER;
//
//bool DecodeIpPack(const char *, int);    // IP 解包
//bool DecodeTcpPack(const char *);        // TCP 解包
//bool DecodeUdpPack(const char *);        // UDP 解包
//bool DecodeIcmpPack(const char *);        // ICMP 解包
//const char * CheckProtocol(int);// 查询协议
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