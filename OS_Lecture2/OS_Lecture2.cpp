#include <iostream>
#include "palindrome.h"
using namespace std;
int main()
{
    int n;
    cout << "Enter a number: ";
    cin >> n;
    if (palindrome(n))
    {
        cout << "Palindrome!\n";
    }
    else
    {
        cout << "Not a palindrome!\n";
    }
}
