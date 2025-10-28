#include "palindrome.h"
#include <string>
#include <algorithm>
#include <stdexcept>
using namespace std;
bool palindrome(int number)
{
    string s = to_string(number);
    if (s.empty()) throw invalid_argument("Input cannot be empty");
    string reversed = s;
    reverse(reversed.begin(), reversed.end());
    return s == reversed;
}
