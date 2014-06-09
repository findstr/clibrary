#ifndef _ZSTRING_H
#define _ZSTRING_H

#ifdef __cplusplus
extern "C" {
#endif


int zstring_init(void);
int zstring_set_terminalch(int end_char);

#ifdef _UNICODE

#define	zstring_len(str)			zstring_len_w(str)
#define	zstring_sztoul(p, char_cnt, base)	zstring_sztoul_w(p, char_cnt, base)
#define	zstring_copy(dst, src, n)		zstring_copy_w(dst, src, n)

int zstring_len_w(const wchar_t *str);
unsigned long zstring_sztoul_w(const wchar_t *p, unsigned long ch_cnt, int base);
wchar_t *zstring_copy_w(wchar_t *dst, const wchar_t *src, int n);

#else

#define	zstring_len(str)		zstring_len_a(str)
#define	zstring_sztoul(p, char_cnt, base)	zstring_sztoul_a(p, char_cnt, base)
#define	zstring_copy(dst, src, n)	zstring_copy_a(dst, src, n)


int zstring_len_a(const char *str);
unsigned long zstring_sztoul_a(const char *p, unsigned long ch_cnt, int base);
wchar_t *zstring_copy_a(char *dst, const char *src, int n);


#endif // _UNICODE

#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !_STRING_Z_H
