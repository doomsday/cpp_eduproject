#include "file1.hpp"
#include "file2.hpp"

#include <iostream>

int main()
{
    const char name[] = "R. N. Briggs";
    auto arr1 = name;
    auto& arr2 = name;

    std:: cout << arr2;
}