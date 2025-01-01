# Language Specifications for PseudoLang Language

## Grammar and Syntax

### 1. **Variable Declaration**

- **Syntax**:
```pseudo
delcare VAR [<-VALUE];
```
- **Examples**:
```pseudo
declare x <- 5; // Variable initialized to 5
declare y;      // Unititialized variable
```

### 2. **Assignments**
- **Syntax**:
```pseudo
VAR <- VALUE;
```
- **Examples**:
```pseudo
x <- 10;
z <- x + y
```

### 3. **Arithmetic Operations**
- **Supported Operators**: `+`, `-`, `*`, `/`
- **Examples**:
```pseudo
z <- x + y;
n <- z / 2;
```

### 4. **Comments**
- **Single-line**: Use `//` to add comments
```pseudo
// This is a single-line comment
declare x <- 5; // Inline comment
```

### 5. **Loops**
- **Syntax**:
```pseudo
while (CONDITION) loop
  STATEMENTS;
end loop;
```
- **Example**:
```pseudo
while (i < 10 ) loop
  put(i);
  i <- i + 1;
end loop;
```

### 6. **Conditionals**
- **Syntax**:
```pseudo
if (CONDITION) then
    STATEMENTS;
[elseif (CONDITION) then
    STATEMENTS;]
[else
    STATEMENTS;]
end if;
```
- **Example**:
```pseudo
  if (x = 5) then
    put("x is 5");
elseif (x = 4) then
    put("x is 4");
else
    put("x is something else");
end if;
```

### 7. **Input/Output**
- **Output**: Use `put` to display values or strings
```pseudo
put(x);
put("Hello, World!");
```

### 8. **Keywords and Symbols**
- **Reserved Keywords**: `declare`,  `if`, `elseif`, `else`, `then`, `while`, `loop`, `end`, `put`
- **Operators**: `<-`, `+`, `-`, `*`, `/`, `=`

## Data Types

PseudoLang variables are dynamically typed, and the type is inferred from the assigned value. For example:
```pseudo
declare x <- 5;      // Integer
declare y <- "text"; // String
```

## Error Handling

1. **Syntax Errors**:
    - Missing semicolons or incorrect keywords will result in compilation errors.
2. **Runtime Errors**:
    - Undeclared variables used in expressions
    - Division by zero

## Examples
### **Simple Program**
```pseudo
// Declare and assign variables
declare x <- 5;
declare y <- 10;
declare z;

// Perform arithmetic
z <- x + y;
put(z); // Output: 15

// Conditional example
if (z = 15) then
    put("Sum is correct");
else
    put("Sum is incorrect");
end if;

// Loop example
while (x < 10) loop
    put(x);
    x <- x + 1;
end loop;
```
### **Procedure Example**
```pseudo
// Declare and assign variables
declare m <- 5;
declare n;
declare x;
declare y;

procedure main(m, n, x, y)
begin
    n <- m + 2;
    x <- n * 5;
    y <- x - m;
    put("result of y: "); put(y); 
end;
```

## Future Considerations
- **Functions**: Support for user-defined functions
- **Input**: Support for user input
- **Error Reporting**: Detailed error messages for debugging

---
This document will be updated as the language evolves during development
