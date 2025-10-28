#include <iostream>
#include <cassert>
#include "fib.h"
using namespace std;
void runTests()
{
    {
        auto v = Fib(1);
        assert(v.size() == 1 && v[0] == 1);
    }
    {
        auto v = Fib(2);
        assert(v.size() == 2 && v[0] == 1 && v[1] == 1);
    }
    {
        auto v = Fib(6);
        unsigned long long expected[] = { 1, 1, 2, 3, 5, 8 };
        for (int i = 0; i < 6; ++i)
            assert(v[i] == expected[i]);
    }
    cout << "All tests passed successfully!\n";
}
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
    runTests();
    return 0;
}
