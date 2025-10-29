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
void newelem(Node*& head, int value) 
{
    Node* newNode = new Node(value);
    if (!head) 
    {
        head = newNode;
        return;
    }
    Node* temp = head;
    while (temp->ssylka)
    {
        temp = temp->ssylka;
    }
    temp->ssylka = newNode;
}
void clearlist(Node*& head) 
{
    while (head) 
    {
        Node* temp = head;
        head = head->ssylka;
        delete temp;
    }
}