#ifndef _MODULE_H
#define _MODULE_H

#define MOD_USING(m)    do { if (mod_using(__FUNCTION__, #m, m##_init)) {return -1;} } while (0)

int mod_using(const char *caller, const char *m, int (*init)(void));



#endif // !_MODULE_H
