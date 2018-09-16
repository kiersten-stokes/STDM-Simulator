#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#define ADDR_MAX	8
#define TIME_MAX	100
#define LINE_LEN	1024

typedef struct data_object {
	char *name;
	int priority;
	char address[ADDR_MAX];
	int timeslot[TIME_MAX];
	int slots_used;
} data_object;


typedef struct backlog_obj {
	char *name;
	char address[ADDR_MAX];
	int frame_no;
} backlog_obj;


typedef struct source_data {
	void *obj;	
	struct source_data *next;
	struct source_data *prev;
} source_data;


typedef struct source_list {
	int num_members;
	struct source_data anchor;
} source_list;

/* initialize list */
void list_init(struct source_list* list) {
	list->num_members = 0;
	list->anchor.next = &list->anchor;
	list->anchor.prev = &list->anchor;
}

/* get first list element */
struct source_data *list_first(struct source_list* list) {
	if (list->num_members == 0) {
		return NULL;
	}
	return list->anchor.next;
}

/* get last list element */
struct source_data *list_last(struct source_list* list) {
	if (list->num_members == 0) {
		return NULL;
	}
	return list->anchor.prev;
}

/* get next list element */
struct source_data *list_next(struct source_list* list, struct source_data* elem) {
	if (elem->next == &list->anchor) {
		return NULL;
	}
	return elem->next;
}

/* append element to list */
void list_append(struct source_list* list, void* obj) {
	struct source_data *newelem = malloc(sizeof(struct source_data));
	if (newelem == NULL) {
		fprintf(stderr, "memory could not be allocated.");
		exit(1);
	}
	if (list->num_members == 0) {
		newelem->next = &(list->anchor);
		newelem->prev = &(list->anchor);
		list->anchor.next = newelem;
	} else {
		struct source_data *lastelem = list_last(list);
		newelem->next = lastelem->next;
		newelem->prev = lastelem;
		lastelem->next = newelem;
	}
	list->anchor.prev = newelem;
	newelem->obj = obj;
	list->num_members++;
}

/* unlink element from list */
void list_unlink(struct source_list* list, struct source_data* elem) {
	if (elem == NULL) {
		fprintf(stderr, "element does not exist.");	
		return;
	}
	elem->prev->next = elem->next;
	elem->next->prev = elem->prev;
	free(elem);
	list->num_members--;
}
