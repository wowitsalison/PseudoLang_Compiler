# Abstract Syntax Tree (AST) Structure for PseudoLang

This document outlines the complete structure of the Abstract Syntax Tree (AST) for the **PseudoLang** language. The AST represents the syntactical components of the language and is a crucial part of the compiler's design.

## Overview

The AST is the intermediate representation of the source code, built by the compiler after lexical analysis and parsing. Each node in the tree represents a construct from the language grammar, such as a declaration, assignment, or loop.

## Complete AST Tree Structure
```scss
ProgramNode
|
+-- DeclareNode (x, LiteralNode (5))           // declare x <- 5;
|
+-- DeclareNode (y)                            // declare y;
|
+-- AssignmentNode (x, BinaryOpNode (+))       // x <- x + y;
|   |
|   +-- IdentifierNode (x)                     // Left Operand: x
|   |
|   +-- IdentifierNode (y)                     // Right Operand: y
|
+-- OutputNode (IdentifierNode (x))            // put(x);
|
+-- ConditionalNode                            // if statement
|   |
|   +-- ExprNode (Condition)                   // Expression for condition (x = 5)
|   |
|   +-- ThenStatements                         // Then block
|   |   +-- OutputNode (LiteralNode ("x is 5"))  // put("x is 5");
|   |
|   +-- ElseStatements                         // Else block
|       +-- OutputNode (LiteralNode ("x is not 5"))  // put("x is not 5");
|
+-- LoopNode                                   // while loop
|   |
|   +-- ConditionNode (x < 10)                 // while (x < 10)
|   |
|   +-- LoopStatements                         // Loop body
|       +-- OutputNode (IdentifierNode (x))     // put(x);
|       +-- AssignmentNode (x, BinaryOpNode (+))  // x <- x + 1
|           |
|           +-- IdentifierNode (x)              // Left Operand: x
|           |
|           +-- LiteralNode (1)                 // Right Operand: 1
|
+-- DeclareNode (result, BinaryOpNode (*))      // declare result <- x * y;
|   |
|   +-- IdentifierNode (x)                      // Left Operand: x
|   |
|   +-- IdentifierNode (y)                      // Right Operand: y
```

## Breakdown of AST Nodes

1. **ProgramNode**: The root of the AST that contains all the statements in the program.
2. **DeclareNode**: Represents variable declarations. Each `DeclareNode` is either initialized (e.g., `x <- 5`) or uninitialized (e.g., `y`).
3. **AssignmentNode**: Represents the assignment operation, such as `x <- x + y`.
4. **OutputNode**: Represents the output operation (`put` statement).
5. **ConditionalNode**: Represents an `if-elseif-else` block.
6. **LoopNode**: Represents a `while` loop construct.
7. **BinaryOpNode**: Represents binary operations like `+`, `-`, `*`, `/`.

## Covered Tokens

- **Keywords**: `declare`, `if`, `elseif`, `else`, `then`, `while`, `loop`, `end`, `put`
- **Operators**: `<-`, `+`, `-`, `*`, `/`, `=`
- **Expressions**: Literals, identifiers, binary operations, conditions
- **Control Flow**: `if`, `while`, `loop`, `end`
- **Statements**: Declarations, assignments, output, conditionals, loops
