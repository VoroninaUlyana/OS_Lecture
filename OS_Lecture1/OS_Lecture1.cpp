#include <iostream>
#include "fib.h"
using namespace std;
int main() 
{
    try {
        unsigned int n;
        cout << "Enter n: ";
        cin >> n;
        auto result = Fib(n);
        cout << "First " << n << " Fibonacci numbers:\n";
        for (unsigned int i = 0; i < result.size(); ++i)
        {
            cout << i + 1 << ": " << result[i] << '\n';
        }

    }
    catch (const exception& e) 
    {
        cerr << "Error: " << e.what() << '\n';
    }
    return 0;
}
