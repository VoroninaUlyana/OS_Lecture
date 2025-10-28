#include "fib.h"
using namespace std;
vector<unsigned long long> Fib(unsigned int n) 
{
    if (n == 0)
    {
        throw invalid_argument("n must be > 0");
    }
    vector<unsigned long long> result;
    result.reserve(n);
    result.push_back(1);
    if (n == 1)
    {
        return result;
    }
    result.push_back(1);
    for (unsigned int i = 2; i < n; ++i) 
    {
        unsigned long long next = result[i - 1] + result[i - 2];
        result.push_back(next);
    }
    return result;
}
