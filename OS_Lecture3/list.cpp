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