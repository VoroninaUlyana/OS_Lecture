#include "list.h"
#include <iostream>
using namespace std;
int main() 
{
    Node* head = new Node(4);
    head->ssylka = new Node(9);
    head->ssylka->ssylka = new Node(7);
    cout << "Original list: ";
    printlist(head);
    while (head) 
    {
        Node* temp = head;
        head = head->ssylka;
        delete temp;
    }
    return 0;
}