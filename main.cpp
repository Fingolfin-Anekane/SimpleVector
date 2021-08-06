#include "simple_vector.h"
#include <algorithm>
#include <iostream>
// Tests
#include "tests.h"

int main() {
    Test1();
    Test2();
    int* my_array = new int[20];
    int* my_array1 = new int[15];
    //std::fill(&my_array[0], &my_array[5], 228);
    for (int i = 1; i < 6; ++i){
        my_array[i-1] = i;
    }
    for (int i = 0; i < 20; ++i){
        //std::cout << my_array[i] << " ";
    }
    std::cout << std::endl;
    std::copy_backward(&my_array[0], &my_array[5], &my_array[6]);
    //std::cout << my_array[0] << std::endl;
    for (int i = 0; i < 20; ++i){
        //std::cout << my_array[i] << " ";
    }
    //std::cout << my_array[19] << std::endl;
    return 0;
}