#pragma once
#include "task.h"


void append(Task_list* list, Task* task);


Task* remove_first(Task_list* list);
void insert(Task_list* list, Task* task);

int has_elements(Task_list *list);