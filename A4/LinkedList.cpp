#include <bits/stdc++.h>
using namespace std;

struct Node
{
	string instruction;
	string r0,r1,r2;
	Node* next;
};

Node* head = NULL; // head of the list
Node* tail = NULL; // tail of the list

void insertAtTail(string x,string r_0,string r_1,string r_2){
	Node* new_node = new Node;
	new_node->instruction = x;
	new_node->r0 = r_0;
	new_node->r2 = r_2;
	new_node->r1 = r_1;
	new_node->next = NULL;
	if(head == NULL){
		head = new_node;
		tail = new_node;
	}
	tail->next = new_node;
	tail = new_node;
}

bool SameNodes(string i1,string ri0,string ri1,string ri2,string x,string r0,string r1,string r2){
	if(i1 == x && ri0 == r0 && ri1 == r1 && ri2 == r2) return true;
	else return false;
}

bool removeNode(string x,string r0,string r1,string r2){ // true => node is deleted. false => node does not exist
	Node* iter = head;
	if(SameNodes(head->instruction,head->r0,head->r1,head->r2,x,r0,r1,r2)){	
		head = head->next;
		delete iter;
		return true;
	}
	while(iter->next != NULL){
		Node* iternext = iter->next;
		string i1 = iternext->instruction;
		string ri0 = iternext->r0;
		string ri1 = iternext->r1;
		string ri2 = iternext->r2;
		if(SameNodes(i1,ri0,ri1,ri2,x,r0,r1,r2)){
			Node* iternextNext = iternext->next;
			if(iternextNext == NULL){
				tail = iter;
			}
			iter->next = iternextNext;
			/*iternext->next = NULL;*/
			delete iternext;
			return true;
		}
		iter = iter->next;
	}
	return false; 
}

void printList(){
	Node* current = head;
	while(current != NULL){
		cout << current->instruction << ' ';
		current = current->next;
	}
	cout << endl;
}

bool insertJustAfterPointer(Node* ptrAfter,Node* ptrBefore){ 
	if(ptrBefore == NULL || ptrAfter == NULL){
		return false;
	}
	Node* new_node = new Node;
	new_node->instruction = ptrAfter->instruction;
	new_node->r0 = ptrAfter->r0;
	new_node->r1 = ptrAfter->r1;
	new_node->r2 = ptrAfter->r2;
	//delete ptrAfter => make it a tombstone node : (-1,-1,-1,-1) as values
	{
		ptrAfter->instruction = "-1";
		ptrAfter->r0 = "-1";
		ptrAfter->r1 = "-1";
		ptrAfter->r2 = "-1";
	}

	Node* target = ptrBefore->next;
	ptrBefore->next = new_node;
	new_node->next = target;
	return true;
}

int main(){
	insertAtTail("1","2","3","4");
	insertAtTail("2","2","3","4");
	insertAtTail("3","2","3","4");
	insertAtTail("4","2","3","4");
	insertAtTail("5","2","3","4");
	insertAtTail("6","2","3","4");
	insertAtTail("7","2","3","4");
	insertAtTail("8","2","3","4");
	insertAtTail("9","2","3","4");
	printList();
	removeNode("9","2","3","4");
	printList();
	insertAtTail("9","2","3","4");
	/*cout << tail->instruction << tail->r0 << tail->r1 << tail->r2 << endl;*/
	printList();

	Node* ptr1 = head->next->next;
	Node* ptr2 = head;

	cout << ptr1->instruction << ptr1->r0 << ptr1->r1 << ptr1->r2 << endl;
	cout << ptr2->instruction << ptr2->r0 << ptr2->r1 << ptr2->r2 << endl;

	insertJustAfterPointer(ptr1,ptr2);
	printList();
}