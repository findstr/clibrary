#ifndef _DMEMORY_H
#define	_DMEMORY_H

#ifdef _DEBUG
#define	dminit()		dmemory_init()
#define	dmalloc(a)	        dmemory_alloc(a)
#define	dfree(a)		(dmemory_free(a), a = NULL)
#define	dcheck_leak()	        dmemory_check_leak()

#ifdef __cplusplus
#define	EXTERN	extern "C"
//#define	EXTERN
#else
#define	EXTERN
#endif

EXTERN int dmemory_init(void);
EXTERN void *dmemory_alloc(size_t size);
EXTERN void dmemory_free(void *mem);
EXTERN int dmemory_check_leak(void);
#else
#include <stdlib.h>
#define	dminit()	((void)0)
#define	dmalloc(a)	malloc(a)
#define	dfree(a)	free(a)
#define	dcheck_leak()	((void)0)

#endif // _DEBUG



#endif // !_MEMORY_H
