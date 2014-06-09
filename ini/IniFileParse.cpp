#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "IniFileParse.h"

struct key {
        char *key_name;
        char *key_val;
};

struct section {
        int     key_nr;
        char    *name;
        struct  key *key_tbl;
};

struct ini_config {
        int             section_nr;
        struct  section *section_tbl;
};

static char *get_line(char *buff, char *line, int n)
{
	while (*buff != '\0' && *buff!= '\n' && n > 0) {
		if (*buff == '\r')
			buff++;
		else
			*line++ = *buff++;
		n--;
	}
	*line = '\0';
	buff++;
	return buff;
}

static int get_section_nr(char *buff, unsigned long size)
{
	int nr = 0;
	while (size--)
		if (buff[size] == '[')
			nr++;
	return nr;
}

static int get_section_key_nr(char *section_begin, int size, int *section_nr, int *key_nr)
{
	char *orig = section_begin;
	*key_nr = 0;
	/* skip the header */
	while (size--) {
		if (*section_begin == '[')
			break;
		section_begin++;
	}
	if (*section_begin != '[')
		return -1;
	/* caculate the section_size */
	section_begin++;
	*section_nr = 0;
	while (size--) {
		if (*section_begin == ']')
			break;
		section_begin++;
		(*section_nr)++;
	}
	if (*section_begin != ']')
		return -1;
	/* skip this line */
	section_begin++;
	while (size--) {
		if (*section_begin == '\n')
			break;
		section_begin++;
	}
	if (*section_begin != '\n')
		return -1;
	/* now caculate the key count */
	section_begin++;
	while (size--) {
		if (*section_begin == '\n')
			(*key_nr)++;
		else if (*section_begin == '[' || *section_begin == '\0')
			break;
		section_begin++;
	}
	return section_begin - orig;
}

int get_key_val_nr(char *buff, int size, int *key_nr, int *val_nr)
{
	while (*buff == ' ' || *buff == '\t') {
		size--;
		buff++;
	}
	if (*buff == '\0')
		return 0;

	buff++;
	*key_nr = 0;
	while (size--) {
		if (*buff == '=')
			break;
		(*key_nr)++;
		buff++;
	}
	if (*buff != '=')
		return 0;

	*val_nr = 0;
	while (size--) {
		if (*buff == '\r')
			break;
		(*val_nr)++;
		buff++;
	}
	if (*buff != '\r')
		return 0;
	return 1;
}

int get_key_val(char *buff, int size, char *key, char *val)
{
	while (*buff == ' ' || *buff == '\t') {
		size--;
		buff++;
	}
	if (*buff == '\0')
		return 0;

	while (size--) {
		if (*buff == '=') {
			*key = '\0';
			break;
		}
		*key++ = *buff++;
	}
	if (*buff != '=')
		return 0;

	while (size--) {
		if (*buff == '\r') {
			*val = '\0';
			break;
		}
		*val++ = *buff++;
	}
	if (*buff != '\r')
		return 0;
	return 1;
}

int get_section(char *buff, int size, char *section)
{
	while (size--) {
		if (*buff == '[' || *buff == '\0')
			break;
	}
	if (*buff == '\0')
		return 0;
	buff++;
	while (size--) {
		if (*buff == ']')
			break;
		*section++ = *buff++;
	}
	*section = '\0';

	return 1;
}

static int parse(struct ini_config *ini, char *buff, unsigned long size)
{
	int curr_section;
	char *curr_pos;
	char *tmp;
	char line[128];
	int section_nr;
	int key_nr;
	int key_cnt;
	int val_cnt;
	int cnt;
	curr_section = 0;

	curr_pos = buff;
	while (size) {
		cnt = get_section_key_nr(curr_pos, size, &section_nr, &key_nr);
		if (cnt == -1)
			return 0;
		tmp = curr_pos;
		ini->section_tbl[curr_section].key_nr = key_nr;
		ini->section_tbl[curr_section].key_tbl = (struct key *)malloc(sizeof(struct key));
		if (!ini->section_tbl[curr_section].key_tbl)
			return 0;
		ini->section_tbl[curr_section].name = (char *)malloc(section_nr);
		tmp = get_line(tmp, line, 128);
		get_section(line, 128, ini->section_tbl[curr_section].name);
		for (int i = 0; i < key_nr; i++) {
			tmp = get_line(tmp, line, 128);
			get_key_val_nr(line, 128, &key_cnt, &val_cnt);
			ini->section_tbl[curr_section].key_tbl[i].key_name = (char *)malloc(key_cnt);
			ini->section_tbl[curr_section].key_tbl[i].key_val = (char *)malloc(val_cnt);
			get_key_val(line, 128, 
				ini->section_tbl[curr_section].key_tbl[i].key_name,
				ini->section_tbl[curr_section].key_tbl[i].key_val);
			printf("section:%s,key_name:%s, key_val%s\n", ini->section_tbl[curr_section].name,
				ini->section_tbl[curr_section].key_tbl[i].key_name,
				ini->section_tbl[curr_section].key_tbl[i].key_val);
		}
		curr_section++;
		curr_pos += cnt;
		size -= cnt;
	}
	return 1;
}
struct ini_config *ini_load(char *name)
{
        FILE *fp;
	char *buff;
        struct _stat st;
	struct ini_config *p_ini;
        
	p_ini = NULL;

	fp = fopen(name, "rb");

        if (!fp)
                return NULL;
       if (_stat(name, &st) < 0)
                return NULL;
       buff = (char *)malloc(st.st_size);
       if (buff == NULL) {
	       p_ini = NULL;
	       goto end1;
       }
       fread(buff, st.st_size, 1, fp);
       /* now parse it */
       p_ini = (struct ini_config *)malloc(sizeof(struct ini_config));
       if (!p_ini) {
	       p_ini = NULL;
	       goto end2;
       }
       p_ini->section_nr = get_section_nr(buff, st.st_size);
       p_ini->section_tbl = (struct section *)malloc(sizeof(struct section));
       if (!p_ini->section_tbl) {
	       free(p_ini);
	       p_ini = NULL;
	       goto end2;
       }

       if (!parse(p_ini, buff, st.st_size)) {
	       free(p_ini);
	       free(p_ini->section_tbl);
	       p_ini = NULL;
       }

       /* release the memory */
end2:
	free(buff);
end1:
	fclose(fp);
	
	return p_ini;
}
