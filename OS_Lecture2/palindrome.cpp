#include "palindrome.h"
#include <string>
#include <algorithm>
using namespace std;
bool palindrome(int number)
{
    string s = to_string(number);
    string reversed = s;
    reverse(reversed.begin(), reversed.end());
    return s == reversed;
}
