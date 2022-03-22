#ifndef _TREE_H
#define _TREE_H

#include "node.h"

#define MAXTOKEN 8
#define DEF_NODES 10

void initROOT();
void upsizeROOT();
void destroyROOT();
void traversePreOrder(struct node_t* nodePtr, int level);

struct node_t* createNode(char* name);



#endif
