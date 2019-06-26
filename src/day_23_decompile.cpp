#include <iostream>
#include <limits>

int main(int argc [[maybe_unused]], char** argv [[maybe_unused]]) {
    int min = std::numeric_limits<int>::min();
    std::cout << min << std::endl;
    std::cout << min-1 << std::endl;
    std::cout << std::numeric_limits<int>::max() << std::endl;
    std::cout << (int)(((long)std::numeric_limits<int>::min())-1) << std::endl;
    std::cout << (int)(((long)std::numeric_limits<int>::min())-2) << std::endl;
    long n1 = (long)3-(long)std::numeric_limits<int>::min();
    long n2 = (long)std::numeric_limits<int>::max()-(long)93;
    long n = n1+n2;
    std::cout << (int)(2+n1) << std::endl;
    std::cout << (int)((long)std::numeric_limits<int>::max()-n2) << std::endl;
    std::cout << (int)(2-n1-n2) << std::endl;
    std::cout << (int)(2-n) << std::endl;
    std::cout << "The innermost loop needs " << n << " iterations" << std::endl;
}
