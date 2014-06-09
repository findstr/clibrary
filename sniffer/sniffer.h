#ifndef _SNIFFER_H
#define	_SNIFFER_H

int sniffer_init(void);
int sniffer_dst(const char *ip, const char *port);
const unsigned char *sniffer_get(
	char sz_src_ip[32],
	char sz_src_port[8],
	char sz_dst_ip[32],
	char sz_dst_port[8],
	unsigned long *size
	);


#endif // !_SNIFFER_H
