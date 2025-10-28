#include <iostream>
#include "palindrome.h"
using namespace std;
int main()
{
    try
    {
        int n;
        cout << "Enter a number: ";
        cin >> n;
        if (cin.fail()) 
        { 
            throw invalid_argument("Invalid input"); 
        }
        if (palindrome(n))
        {
            cout << n << " Palindrome!\n";
        }
        else
        {
            cout << n << " Not a palindrome!\n";
        }
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
