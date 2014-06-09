#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "module.h"

struct mod_item {
        int     inited_cnt;
        int     (*init)(void);
        char    caller[64];
        char    called[64];
        struct  mod_item *next;
};

struct mod_item *header = NULL;

static void list_add(struct mod_item *mode)
{
        mode->next = header;
        header = mode;
}

static mod_item *is_inited(int (*init)(void))
{
        struct mod_item *p;

        for (p = header; p != NULL; p = p->next) {
                if (p->init == init)
                        return p;
        }

        return NULL;
}

static struct mod_item *mod_gen(const char *caller, const char *m, int (*init)(void))
{
        int n;
        struct mod_item *p;

        p = (struct mod_item *)malloc(sizeof(*p));
        if (!p)
                return NULL;

        memset(p, 0, sizeof(*p));

        strncpy(p->called, m, sizeof(p->called) / sizeof(p->called[0]));
        strncpy(p->caller, caller, sizeof(p->caller) / sizeof(p->caller[0]));
        n = strlen(p->caller);
        while (n--) {
                if (p->caller[n] == '_') {
                        p->caller[n] = '\0';
                        break;
                }
        }
                        
        
        assert(init);
        p->init = init;
        p->inited_cnt = 1;

        return p;
}

int mod_using(const char *caller, const char *m, int (*init)(void))
{
        struct mod_item *mode;
        int ret;

        assert(init);

        mode = is_inited(init);
        if (mode) {
		mode->inited_cnt++;
                printf("%s --> %s have inited\n", caller, m);
                return 0;
        }
        
        mode = mod_gen(caller, m, init);
        if (!mode)
                return -1;

        list_add(mode);
        printf("%s --> %s\n", mode->caller, mode->called);

        ret = init();

        return ret;
}