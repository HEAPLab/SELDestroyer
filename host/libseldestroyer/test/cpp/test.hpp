#include <iostream>

#define ASSERT_EQ(a,b) if ((a)!=(b)) { std::cout << "!!! Assertion failed line " << __FILE__ << ":" << __LINE__ << std::endl; return 1;}