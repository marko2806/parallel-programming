#pragma once
#include "Connect4.h"

struct Task_node {
	struct PotezNode potez;
	int id;
	struct Task_node* next;
	struct Task_node* prev;
}typedef Task;

struct Task_list {
	Task* first;
	Task* last;
}typedef Task_list;