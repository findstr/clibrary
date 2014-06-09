#include <assert.h>
#include <stdlib.h>
#include "array.h"

#define	ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

static const unsigned char bound_check_val[] =  {
	'F', 'D', 'R',
};

struct mem_header {
	void *start_addr;
	unsigned long size;
};

static struct {
	struct array *header_tbl;
} malloc_mgr;

int dmemory_init()
{
	malloc_mgr.header_tbl = array_create(sizeof(struct mem_header));
	assert(malloc_mgr.header_tbl);

	return 0;
}


void *dmemory_alloc(size_t size)
{	
	int i;
	unsigned char *p;
	struct mem_header	hd;

	hd.size = size;
	hd.start_addr = malloc(size + ARRAY_SIZE(bound_check_val));

	if (hd.start_addr) {
		p = (unsigned char *)hd.start_addr;
		for (i = 0; i < ARRAY_SIZE(bound_check_val); i++)
			p[size + i] = bound_check_val[i];

		array_push(malloc_mgr.header_tbl, &hd);
	}

	return hd.start_addr;
}

void dmemory_free(void *mem)
{
	int i;
	int j;
	unsigned char *p;
	assert(mem);

	for (i = 0; i < array_size(malloc_mgr.header_tbl); i++) {
		if (array_at(malloc_mgr.header_tbl, i, struct mem_header).start_addr == mem)
			break;
	}
	assert(i < (int)array_size(malloc_mgr.header_tbl));

	p = (unsigned char *)array_at(malloc_mgr.header_tbl, i, struct mem_header).start_addr;
	for (j = 0; j < ARRAY_SIZE(bound_check_val); j++) {
		assert(p[array_at(malloc_mgr.header_tbl, i, struct mem_header).size + j] == bound_check_val[j]);
	}

	free(mem);
	
	array_erase(malloc_mgr.header_tbl, i);
}

int dmemory_check_leak()
{
	return array_size(malloc_mgr.header_tbl);
}