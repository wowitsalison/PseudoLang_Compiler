# Toy Language Compiler
This project is a pre-course exploration of compiler design, created to benchmark my current understanding of compilers against the skills I'll aquire by the end of my upcoming compiler design course. By building this project now, I aim to identify areas for growth and demonstrate progress over the semester.

## Overview
The **Toy Language Compiler** is a simple compiler that translates a custom toy programming language into C++ code. It performs lexical and syntax analysis on the input and generates equivalent C++ source code, which can be compiled and executed using standard tools like GCC or Clang.

### Features
- Tokenizes a custom toy language
- Parses basic programming constructs (e.g., variable declarations and arithmetic operations).
- Ouputs valid C++ source code

## Purpose
The main objective of this project is to serve as a **baseline** for evaluating my growth in compiler desing and give me a surface-level understanding of compilers before taking the course.
- **Before the course**: Focus on foundational conepts such as lexical analysis, parsing, and basic code generation.
- **After the course**: Expand functionality, incorporate advanced concepts like semantic analysis, and optimization.

This comparison wil highlight how much I've learned throughout the semester.

## Example
### Input (Toy Language):
```toy
let x = 5;
let y = 10;
let z = x + y;
```

### Output (Generated C++):
```cpp
#include <iostream>

int main() {
  int x = 5;
  int y = 10;
  int z = x + y;
  return 0;
}
```
Compile the generated C++ code with:  
```
g++ output.cpp -o output
./output
```

## Project Goals
1. Develop a foundational understanding of compiler design ocncepts.
2. Provide a point of comparison for assessing progress after completing the compiler design course
3. Showcase versatility in programming through a project in C++.

## File Structure  
```plaintext
📂 toy-compiler  
├── 📂 src  
│   ├── lexer.cpp         # Tokenizer implementation  
│   ├── parser.cpp        # Syntax parser  
│   ├── codegen.cpp       # C++ code generation  
│   └── main.cpp          # Entry point of the compiler  
├── 📂 examples  
│   ├── example1.toy      # Sample toy language file  
│   └── example2.toy  
├── README.md             # Project overview  
└── Makefile              # Build automation  
```

## Getting started
### Prerequisites
- **C++ compiler**: GCC, Clang, or MSVC.
- **Build Tools**: Make (optional)

### Build and Run
1. Clone the repository:
```
git clone https://github.com/wowitsalison/my-first-compiler.git
cd my-first-compiler
```
2. Build the compiler:
```
make
```
3. Compile a sample toy language file:
```
./my-first-compiler examples/example1.toy -o output.cpp
g++ output.cpp -o output
./output
```

## License
This project is open-source and available under the MIT License.
