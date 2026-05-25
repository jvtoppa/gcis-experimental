# GCIS Project Documentation {#mainpage}

Welcome to the **General Coded Induction of Strings (GCIS)** library documentation! 

## Overview
This library provides high-performance tools for string classification and grammar-based compression.

## Quick Start Example
```cpp
#include "gcis.h"
#include <vector>

int main() {
    GCIS compressor;
    std::vector<uint64_t> input = {1, 2, 1, 2, 3};
    
    // Classify strings into L or S types
    bitVector types = compressor.classify(input);
    return 0;
}
