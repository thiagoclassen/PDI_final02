#include <stdlib.h>
#include <stdio.h>
#include "linked.h"

Node **ref;

void wipeList(Header *head) {
	if (head->nodeCount > 0) {
		Node *aux;

		aux = head->head;
		while (aux->next != NULL) {
			aux = aux->next;
			free(aux->prev);
		}
		free(aux);
		head->head = NULL;
		head->tail = NULL;
		head->nodeCount = 0;
	}
}
void insertNodeAtStart(Header *head, Dot value) {
	Node *newNode = (Node *) malloc(sizeof(Node));
	newNode->dot = value;
	newNode->prev = NULL;
	if (head->head == NULL) {
		/*
		 * if the head is NULL means that the list was empty, then we should also set
		 * this new element as tail of the list.
		 */
		newNode->next = NULL;
		head->tail = newNode;
		head->head = newNode;
	} else {
		newNode->next = head->head;
		head->head->prev = newNode;
		head->head = newNode;
	}
	head->nodeCount++;
}

void insertNodeAtTheEnd(Header *head, Dot value) {
	Node *newNode = (Node *) malloc(sizeof(Node));
	newNode->dot = value;
	newNode->next = NULL;
	newNode->prev = NULL;

	if (head->head == NULL) {
		/*
		 * if the head is NULL means that the list was empty, then we should also set
		 * this new element as tail of the list.
		 */
		head->head = newNode;
		head->tail = newNode;
		newNode->prev = NULL;
	} else {
		head->tail->next = newNode;
		newNode->prev = head->tail;
		head->tail = newNode;
	}
	head->nodeCount++;
	//printf("\nPOS: %d", head->nodeCount);
}

void printList(Header *head) {

	Node *aux;
	int i = 1;

	printf("\n");
	if (head->nodeCount > 0) {
		for (aux = head->head, i = 1; aux != head->tail; i++, aux = aux->next) {
			printf("\n NodeCount: %d POS: %d [%d][%d] Dist: %f", head->nodeCount, i, aux->dot.y, aux->dot.x, aux->dot.dist);        
	}	
	} else {
		printf("\nLista vazia.\n");
	}

	printf("\n");

}

void deleteFirstNode(Header *head) {

	if (head->nodeCount > 1) {

		Node *aux;
		aux = head->head;

		head->head = aux->next;
		head->head->prev = NULL;
		head->nodeCount--;

		free(aux);
	} else if (head->nodeCount == 1) {
		wipeList(head);
	}
}

void deleteDotAtN(Header *head, int k) {



	if (head->nodeCount == 0) {
		printf("\nLista vazia.\n");
	} else if (head->nodeCount == 1) {
		wipeList(head);
	} else if (k == 0) {
		deleteFirstNode(head);
	} else if (k == head->nodeCount - 1) {
		deleteLastNode(head);
	} else {
		int i;
		Node *aux;
		aux = head->head;

		for (i = 0; i < k; i++) {
			aux = aux->next;
		}

		aux->next->prev = aux->prev;
		aux->prev->next = aux->next;
		head->nodeCount--;

		free(aux);
	}
}

Dot getDotAtN(Header *head, int k) {

	if (head->nodeCount == 0) {
		printf("\nLista vazia.\n");
	} else {
		int i;
		Node *aux;
		aux = head->head;

		for (i = 0; i < k; i++) {
			aux = aux->next;
		}

		return aux->dot;
	}
}

void deleteLastNode(Header *head) {

	if (head->nodeCount > 1) {

		Node *aux;
		aux = head->tail;

		head->tail = aux->prev;
		head->tail->next = NULL;
		head->nodeCount--;

		free(aux);
	} else if (head->nodeCount == 1) {

		wipeList(head);
	}

}


int checkNodeByValue(Header *head, int y, int x) {

	int flag = 0, i;		
	Node *aux;
	aux = head->head;

	for (i = 0; i < head->nodeCount && flag == 0; i++) {
		/*
		 * Checks if the node is the one we're searching for.
		 */
		if (aux->dot.x == x && aux->dot.y == y) {
			flag = 1;			
		}
		aux = aux->next;
	}

	return flag;
}

void startRef(int size) {
	free(ref);
	ref = (Node**) malloc(size * sizeof(Node*));
}

void reference(Header *head) {
	printf("Ref...");
	int i;
	Node *aux;
	aux = head->head;

	startRef(head->nodeCount);

	for (i = 0; i < head->nodeCount; i++) {
		ref[i] = aux;

		aux = aux->next;
	}
}