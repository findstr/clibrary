#ifndef _ZSTRING_H
#define _ZSTRING_H

#ifdef __cplusplus
extern "C" {
#endif

typedef const wchar_t * (zstring_skip_w_t)(const wchar_t *);
typedef const char * (zstring_skip_a_t)(const char *);



#ifdef _UNICODE

#define	zstring_len(str, terminal)			zstring_len_w(str)
#define	zstring_sztoul(p, char_cnt, base, terminal)	zstring_sztoul_w(p, char_cnt, base)
#define	zstring_copy(dst, src, n, terminal)		zstring_copy_w(dst, src, n)
#define	zstring_token(dst, src, n, sep, skip, termi)	zstring_token_w(dsr, src, n, sep, skip, termi)
#define zstring_str(src, dst, terminal)			zstring_str_w(src, dst, terminal)


#else

#define	zstring_len(str)		zstring_len_a(str)
#define	zstring_sztoul(p, char_cnt, base)	zstring_sztoul_a(p, char_cnt, base)
#define	zstring_copy(dst, src, n)	zstring_copy_a(dst, src, n)
#define	zstring_set_skip(skip)		zstring_set_skip_a(skip)

#endif // _UNICODE

int zstring_len_w(const wchar_t *str, int terminal_ch);
unsigned long zstring_sztoul_w(const wchar_t *ptr, unsigned long ch_cnt, int base, int terminal_ch);
unsigned long long zstring_autotoull_w(const wchar_t *ptr, int terminal_ch);
wchar_t *zstring_copy_w(wchar_t *dst, const wchar_t *src, int n, int terminal_ch);
wchar_t *zstring_token_w(wchar_t *dst, const wchar_t *src, int n, int sep, zstring_skip_w_t *skip, int terminal_ch);
wchar_t *zstring_str_w(const wchar_t *src, const wchar_t *dst, int terminal_ch);

int zstring_len_a(const char *str, int terminal_ch);
char *zstring_token_a(char *dst, const char *src, int n, int sep, zstring_skip_a_t *skip, int terminal_ch);
char *zstring_copy_a(char *dst, const char *src, int n, int terminal_ch);



#ifdef __cplusplus
}
#endif // __cplusplus


#endif // !_STRING_Z_H
