#include "fib.h"
#include <stdexcept>
using namespace std;
vector<unsigned long long> fib(unsigned int n) 
{
    if (n == 0)
    {
        throw invalid_argument("n must be > 0");
    }
    vector<unsigned long long> fib;
    fib.reserve(n);
    fib.push_back(1);
    if (n == 1)
    {
        return fib;
    }
    fib.push_back(1);
    for (unsigned int i = 2; i < n; ++i) 
    {
        unsigned long long next = fib[i - 1] + fib[i - 2];
        fib.push_back(next);
    }
    return fib;
}
