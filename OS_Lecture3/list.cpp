#include "list.h"
#include <iostream>
using namespace std;
void printlist(Node* head) 
{
    Node* current = head;
    while (current) 
    {
        cout << current->data << " ";
        current = current->ssylka;
    }
    cout << endl;
}
Node* reverselist(Node* head) 
{
    Node* pred = nullptr;
    Node* curr = head;
    Node* next = nullptr;
    while (curr != nullptr) 
    {
        next = curr->ssylka;
        curr->ssylka = pred;
        pred = curr;
        curr = next;
    }
    return pred;
}