#ifndef __LINKED_H
#define __LINKED_H

struct dot {
	float y;
	float x;
}typedef Dot;

struct node {
	Dot dot;
	struct node *prev;
	struct node *next;
}typedef Node;

struct header {
	Node *head;
	Node *tail;
	int nodeCount;
}typedef Header;

void insertNodeAtTheEnd(Header *head, Dot value);
void insertNodeAtStart(Header *head, Dot value);
void printList(Header *head);
void wipeList(Header *head);
void deleteLastNode(Header *head);
void deleteFirstNode(Header *head);
void deleteDotAtN(Header *head, int k);
Dot getDotAtN(Header *head, int k);

void startRef(int size);
//void reallocRef(int size);
void reference(Header *head);

#endif