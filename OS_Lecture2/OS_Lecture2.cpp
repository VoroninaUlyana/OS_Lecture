#include <iostream>
#include "palindrome.h"
#include <cassert>
using namespace std;
void runTests() 
{
    assert(palindrome(121));
    assert(palindrome(0));
    assert(!palindrome(123));
    assert(palindrome(12321));
    assert(!palindrome(12345));
    cout << "All tests passed!\n";
}
int main()
{
    runTests();
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
