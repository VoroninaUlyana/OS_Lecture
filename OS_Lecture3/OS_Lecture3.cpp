#include "list.h"
#include <iostream>
#include <cassert>
using namespace std;
void runTests()
{
    {
        Node* head = nullptr;
        newelem(head, 49);
        newelem(head, 33);
        newelem(head, 81);
        assert(head != nullptr);
        assert(head->data == 49);
        assert(head->ssylka->data == 33);
        assert(head->ssylka->ssylka->data == 81);
        clearlist(head);
        assert(head == nullptr);
    }
    {
        Node* head = nullptr;
        newelem(head, 49);
        newelem(head, 29);
        newelem(head, 1);
        head = reverselist(head);
        assert(head->data == 1);
        assert(head->ssylka->data == 29);
        assert(head->ssylka->ssylka->data == 49);
        clearlist(head);
        assert(head == nullptr);
    }
    {
        Node* head = nullptr;
        for (int i = 0; i < 5; ++i)
            newelem(head, i);
        clearlist(head);
        assert(head == nullptr);
    }
    cout << "\nAll tests passed successfully!\n\n";
}
int main() 
{
    runTests();
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