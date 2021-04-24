#include <iostream>
#include <cctype>

#include "differentiator.h"

using namespace se;

int main() {
    differentiator differentiator;
    char* input_str = nullptr;
    
    // Get the expression
    input_str = (char*) calloc(max_str_size, sizeof(char));
    std::cout << "Enter the expression: ";
    std::cin.getline(input_str, max_str_size);

    // Differentiate the expression
    differentiator.set_expression(input_str);
    differentiator.diff();
    differentiator.generateLatex("output.tex");

    // Free resources
    free(input_str);
    
    return 0;
}