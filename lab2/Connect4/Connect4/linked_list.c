#include "task.h"
#include <stdlib.h>
#include <stdio.h>

#define print(format, ...) do{	    \
	printf(format, __VA_ARGS__);	\
	fflush(stdout);					\
}while(0)

void append(Task_list* list, Task* task) {
	// first element
	
	if (list->first == NULL) {
		list->first = task;
		list->last = task;
		task->prev = NULL;
		task->next = NULL;
	}
	// other elements
	else {
		// zadnji element liste je sada prethodnik novog cvora
		task->prev = list->last;
		task->next = NULL;
		task->prev->next = task;
		list->last = task;
	}
}

void insert(Task_list* list, Task* task) {
	if (list->first == NULL) {
		list->first = task;
	}
	else {
		list->first->prev = task;
		task->next = list->first;
		list->first = task;
	}
}


Task* remove_first(Task_list* list) {

	Task* t = list->first;	
	// second element is now first;
	if (list->first != NULL) {
		if (list->first->next == NULL) {
			list->first = NULL;
		}
		else {
			list->first = list->first->next;
		}
	}
	else if (list->first != NULL && list->first->next == NULL) {
		list->first == NULL;
	}
	return t;
}

int has_elements(Task_list* list) {
	return list->first != NULL;
}