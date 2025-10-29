#include "list.h"
#include <iostream>
using namespace std;
int main() 
{
    srand(static_cast<unsigned int>(time(nullptr)));
    int choice, n;
    Node* head = nullptr;
    cout << "How do you want to fill the list?\n";
    cout << "1 - Keyboard input\n";
    cout << "2 - Random numbers\n";
    cout << "Your choice: ";
    cin >> choice;
    cout << "Enter number of elements: ";
    cin >> n;
    if (choice == 1) 
    {
        cout << "Enter " << n << " elements:\n";
        for (int i = 0; i < n; ++i) 
        {
            int number;
            cin >> number;
            newelem(head, number);
        }
    }
    else if (choice == 2) 
    {
        for (int i = 0; i < n; ++i) 
        {
            int number = rand() % 100;
            newelem(head, number);
        }
    }
    else 
    {
        cout << "Invalid choice.\n";
        return 1;
    }
    cout << "Original list: ";
    printlist(head);
    head = reverselist(head);
    cout << "Reversed list: ";
    printlist(head);
    clearlist(head);
    return 0;
}