# Compilers Project - UDF Compiler

This project was divided into three main parts.
- First, we implemented the **abstract syntax tree (AST)** by creating all the necessary node classes for the UDF language, extending the CDK framework with custom nodes and setting up the visitor skeletons, ensuring the code compiled even if not yet functional.
- In the second part, we developed the **scanner** and **parser** using Flex and Bison, along with the **XML generato**r, ensuring that the outputted XML correctly represented the AST structure.
- Finally, the third phase involved generating **Postfix** and corresponding x86 (32-bit) assembly code, with complete **type checking** to enforce correct type usage and conversions during code generation.

# UDF Language
UDF is an imperative language. This description presents the language's characteristics in an intuitive way: [data types](#data-types); [name manipulation](#manipulating-names); [lexical conventions](#lexical-conventions); [structure/syntax](#grammar); [function specification](#functions); [instruction semantics](#instructions); [expression semantics](#expressions); and, finally, some [examples](#examples).

## Data Types
The language is weakly typed (some implicit conversions are performed). There are 4 basic data types, all compatible with the C language, and with memory alignment always in 32 bits:
- Numeric types: integers, in 2's complement, occupy 4 bytes; reals, in floating point, occupy 8 bytes (IEEE 754).
- Character strings are vectors of characters terminated by ASCII NUL (character with the value zero). Variables and literals of this type can only be used in assignments, printing, or as function arguments/returns. Characters are 8-bit values ​​that cannot be manipulated directly.
- Pointers represent object addresses and occupy 4 bytes. They can be the object of arithmetic operations (shifts) and allow access to the pointed value.

There are also types associated with tensors, that is, types that describe multidimensional structures (see below). The values ​​in memory associated with these types are effectively pointers, but to structures that represent tensors and not directly to the data. These pointers do not accept pointer arithmetic or indexing operations (although pointers to these pointers do accept them).

The types supported by each operator and the operation to be performed are indicated in the definition of the expressions.

## Name Manipulation
Names (identifiers) correspond to variables and functions. In the following points, the term entity is used to designate them indiscriminately, being explicit when the description is valid only for a subset.

### Namespace and identifier visibility
The global namespace is unique, so a name used to designate an entity in a given context cannot be used to designate others (even if of a different nature).

Identifiers are visible from the declaration to the end of their scope: file (globals) or function (locals). Reusing identifiers in lower contexts hides declarations in higher contexts: local redeclarations can hide globals up to the end of a function. It is not possible to import or define global symbols in function contexts (see global symbols).

It is not possible to define functions inside blocks.

### Validity of variables
Global entities (declared outside any function) exist throughout the execution of the program. Variables local to a function exist only during its execution. Formal arguments are valid while the function is active.

## Lexical Conventions
For each group of lexical elements (tokens), the longest sequence of characters constituting a valid element is considered. For example, the designation >= is always a single lexical element (as opposed to the illegal situation of having two symbols: > followed by =).

### White space characters
These are considered separators and do not represent any lexical element: ASCII line feed LF (0x0A, \n), ASCII reel indent CR (0x0D, \r), ASCII space SP (0x20, ) and ASCII horizontal tab HT (0x09, \t).

### Comments
There are two types of comments, which also function as separator elements:
- explanatory: they begin with **//** and end at the end of the line;
- operational -- they begin with **/\*** and end with **\*/**, and can be nested.

If the start sequences are part of a character string, they do not start a comment (see the definition of character strings).

### Keywords
The following keywords are reserved and do not constitute identifiers (they must be written exactly as indicated):
- **types**: `int` `real` `ptr` `string` `tensor` `void`
- **declarations**: `forward` `public` `auto`
- **statements**: `if` `elif` `else` `for` `break` `continue` `return` `write` `writeln`
- **expressions**: `input` `nullptr` `objects` `sizeof`
- tensor operators: (see below)

The identifier `udf`, although not reserved, when referring to a function, corresponds to the main function, and must be declared public.

### Types
The following lexical elements designate types in declarations (see grammar): `int` (integer), `real` (real), `string` (character string), `tensor` (tensors), `auto` (type inferred from the initial value), `ptr` (pointers). See grammar.

The special type `auto` is used to indicate that the type of the variable or function return should be inferred from the type of its initial value. When applied to a function, it implies that the return type must be inferred from the expression that is used in the return statement (the expression must agree on the type to return).

The `auto` type can be used to define generic pointers (like **void\*** in C/C++), compatible with all pointer types. They are also the only pointer type that is convertible to an integer (the value of the integer is the value of the memory address). The nesting level is irrelevant in this case, i.e. **ptr<auto>** designates the same type as **ptr<ptr<...ptr<auto>...>>**. The use of the word `void` in this context is identical.

### Expression operators
The lexical elements presented in the definition of expressions are considered operators.

### Delimiters and terminators
The following lexical elements are delimiters/terminators: , (comma), ; (semicolon), and ( and ) (expression delimiters).

### Identifiers (names)
These begin with a letter, followed by 0 (zero) or more letters, digits or _ (underscore). The length of the name is unlimited and two names are distinct if there is a change from uppercase to lowercase, or vice versa, of at least one character.

### Literals
These are notations for constant values ​​of some types of the language (not to be confused with constants, i.e., identifiers that designate elements whose value cannot be changed during the execution of the program).

### Integers
An integer literal is a non-negative number. An integer constant can, however, be negative: negative numbers are constructed by applying the unary arithmetic negation operator (-) to a positive literal.

Decimal integer literals consist of sequences of 1 (one) or more digits from 0 to 9.

Hexadecimal integer literals always begin with the sequence 0x, followed by one or more digits from 0 to 9 or a to f (case insensitive). The letters from a to f represent the values ​​from 10 to 15 respectively. Example: 0x07.

If the integer literal cannot be represented on the machine due to an overflow, a lexical error must be generated.

### Floating point reals
Positive real literals are expressed as in C (only base 10 is supported).

There are no negative literals (negative numbers result from the application of the unary negation operation).

A literal without a . (decimal point) or exponential part is of type integer.

If the real literal cannot be represented on the machine due to an overflow, a lexical error must be generated.

Examples: 3.14, 1E3 = 1000 (integer represented in floating point). 12.34e-24 = 12.34 x 10-24 (scientific notation).

### Strings
Strings are enclosed in quotation marks (") and can contain any character except ASCII NULL (0x00 \0). In strings, comment delimiters have no special meaning. If a literal containing \0 is written, then the string ends at that position. Example: "ab\0xy" has the same meaning as "ab".

Characters can be designated by special sequences (starting with \), especially useful when there is no direct graphical representation. Special sequences correspond to the ASCII characters LF, CR and HT (\n, \r and \t, respectively), quotation mark (\"), slash (\\), or any others specified by 1 or 2 hexadecimal digits (e.g. \0a or just \a if the following character does not represent a hexadecimal digit).

Distinct lexical elements that represent two or more consecutive strings are represented in the language as a single string resulting from concatenation.

Examples:
- "ab" "cd" is the same as "abcd".
- "ab" /* comment with "dummy string" */ "cd" is the same as "abcd".

### Pointers
The only admissible literal for pointers is indicated by the reserved word nullptr, indicating the null pointer.

## Grammar
The grammar of the language is summarized below. It is assumed that elements in fixed type are literals; that curly brackets group elements: ( and ); that alternative elements are separated by a vertical bar: |; that optional elements are enclosed in square brackets: [ and ]; that elements that repeat zero or more times are enclosed in ⟨ and ⟩. Some elements used in the grammar are also elements of the described language if represented in fixed type (e.g., parentheses).

|                   |   |                                                                                           |
|-------------------|---|-------------------------------------------------------------------------------------------|
| file              | → | declaration ⟨ declaration ⟩                                                               |
| declaration       | → | variable ; | function                                                                     |
| variable          | → | [ public | forward ] type identifier [ = expression ]                                     |
| variable          | → | [ public ] auto identifier = expression                                                   |
| function          | → | [ public | forward ] ( type | auto ) identificador ( [ variables ] ) [ block ]            |
| variables         | → | variable ⟨ , variable ⟩                                                                   |
| type              | → | int | real | string | void | tensor< dimension ⟨ , dimension ⟩ > | ptr< ( type | auto ) > |
| block             | → | { ⟨ declaration ⟩ ⟨ instruction ⟩ }                                                       |
| instruction       | → | expression ; | write expressões ; | writeln expressões ;                                  |
| instruction       | → | break | continue | return [ expression ] ;                                                |
| instruction       | → | conditional-instr | iteration-instr | block                                               |
| expressions       | → | expression ⟨ , expression ⟩                                                               |
| conditional-instr | → | if ( expression ) instruction                                                             |
| conditional-instr | → | if ( expression ) instruction ⟨ elif ( expression ) instruction ⟩ [ else instruction ]    |
| iteration-instr   | → | for ( [ variables ] ; [ expressões ] ; [ expressões ] ) instruction                       |
| iteration-instr   | → | for ( [ expressões ] ; [ expressões ] ; [ expressões ] ) instruction                      |


### Types, identifiers, literals and expression definitions
Some definitions have been omitted from the grammar: data types, identifier (see identifiers), literal (see literals); expression (see expressions), dimension (corresponds to an integer literal).

### Left-values
Left-values ​​are memory locations that can be modified (except where prohibited by the data type). The elements of an expression that can be used as left-values ​​are individually identified in the semantics of the expressions.

### Files
A file is called main if it contains the main function (the one that starts the program).

### Variable declarations
A variable declaration always indicates a data type and an identifier. Variables of the special type void cannot be declared.

Examples:
- Integer: `int i`
- Real: `real r`
- String: `string s`
- Pointer to integer: `ptr<int> p1` (equivalent to int* in C)
- Pointer to real: `ptr<real> p2` (equivalent to double* in C)
- Pointer to string: `ptr<string> p3` (equivalent to char** in C)
- Pointer to pointer to integer: `ptr<ptr<int>> p4 `(equivalent to int** in C)
- Generic pointer: `ptr<auto> p5` (equivalent to void* in C)
- 3-dimensional tensor (7x8x9) initialized to zeros: `tensor<7,8,9> t`

### Global symbols
By default, symbols are private to a module, and cannot be imported by other modules.

The `public` keyword allows you to declare an identifier as public, making it accessible from other modules.

The `forward` keyword allows you to declare entities defined in other modules in a module (therefore, the symbols declared by it are also implicitly public). Entities cannot be initialized in these declarations. Functions that are private to a module are defined in advance by omitting the function body, i.e., without using this keyword.

Examples:
- Declare a variable private to the module: `real pi = 22`
- Declare a variable public: `public real pi = 22`
- Use an external definition: `forward real pi`

### Initialization
When present, it is an expression that follows the = ("equal") sign: integer, real, pointer. Real entities can be initialized by integer expressions (implicit conversion). The initialization expression must be a literal if the variable is global. Declaring a tensor without an explicit initial value implies its initialization with zeros.

Strings are (possibly) initialized with a non-zero list of values ​​without separators.

Examples:
- Integer (literal): `int i = 3`
- Integer (expression): `int i = j+1`
- Real (literal): `real r = 3.2`
- Real (expression): `real r = i - 2.5 + f(3)`
- String (literal): `string s = "hello"`
- String (literal): `string s = "hello" "mom"`
- Pointer (literal): `ptr<ptr<ptr<real>>> p = nullptr`
- Pointer (expression): `ptr<real> p = q + 1`
- Pointer (generic): `ptr<auto> p = q`
- Tensor (literal): `tensor<2,2,3> t = [ [ [1, 2, 3], [4, 5, 6] ], [ [7, 8, 9], [10, 11, 12] ] ]`
- Tensor (expression): `tensor<2,2,3> t2 = 2 * t1`
- Generic (simple): `auto p = 2.1`
- Generic (tensor 2x2): `auto t = [[1,2],[3,4]]`


## Functions
A function allows you to group a set of instructions in a body, executed based on a set of parameters (the formal arguments), when it is invoked from an expression.

### Declaration
Functions are always called by constant identifiers preceded by the data type returned by the function. If the function does not return a value, it is declared with the return type `void`. In this case, the `return` statement does not admit an expression.

Functions that receive arguments must indicate them in the header. Functions without arguments define an empty header. It is not possible to apply the export/import qualifiers `public` or `forward` (see global symbols) to the declarations of the arguments of a function. It is not possible to specify initial values ​​(default values) for function arguments. The `auto` type cannot be used to declare argument types (except for defining generic pointers).

A function declaration without a body is used to type an external identifier or to make forward declarations (used to pre-declare functions that are used before they are defined, for example, between two mutually recursive functions). If the declaration has a body, a new function is defined (in this case, the `forward` keyword cannot be used).

### Body
The body of a function consists of a block that may have (optional) declarations followed by (optional) instructions. A function without a body is a declaration and is considered undefined.

The keywords `public` or `forward` (see global symbols) cannot be used inside the body of a function.

A `return` statement causes the function to stop immediately, as does the return of the value indicated as the instruction's argument. The type of this value must match the declared type.

Any subblock (used, for example, in a conditional or iteration instruction) may define variables.

### Main function and program execution
A program begins with the invocation of the udf function (without arguments). The arguments with which the program was called can be obtained through the following functions:
- `int argc()` (returns the number of arguments);
- `string argv(int n)` (returns the nth argument as a string) (n>0); and
- `string envp(int n)` (returns the nth environment variable as a string) (n>0).

The return value of the main function is returned to the environment that invoked the program. This return value follows the following rules (operating system):
- 0 (zero): error-free execution;
- 1 (one): invalid arguments (in number or value);
- 2 (two): execution error.

Values ​​greater than 128 indicate that the program terminated with a signal. In general, for correct operation, programs should return 0 (zero) if execution was successful and a value other than 0 (zero) in case of error.

The runtime library (RTS) contains information about other available support functions, including system calls (see also the RTS manual).

## Instructions
Except where otherwise noted, instructions are executed in sequence.

### Blocks
Each block has a local variable declaration zone (optional), followed by a zone with instructions (possibly empty). It is not possible to declare or define functions inside blocks.

The visibility of variables is limited to the block in which they were declared. Declared entities can be used directly in sub-blocks or passed as arguments to functions called inside the block. If the identifiers used to define local variables are already being used to define other entities within the scope of the block, the new identifier refers to a new entity defined in the block until it ends (the previously defined entity continues to exist, but cannot be directly referred to by its name). This rule also applies to function arguments (see the body of the functions).

### Conditional instruction
This instruction behaves identically to the `if-else` instruction in C. The `elif` parts behave like `else if` sequences.

### Iteration instruction
This instruction behaves identically to the `for` instruction in C. In the variable declaration area, only one auto instruction may be used, and in this case it must be the only one.

### Termination instruction
The `break` instruction terminates the innermost loop in which the instruction is located, just like the `break` instruction in C. This instruction can only exist within a loop, and is the last instruction in its block.

### Continuation instruction
The `continue` instruction restarts the innermost loop in which the instruction is located, just like the `continue` instruction in C. This instruction can only exist within a loop, and is the last instruction in its block.

### Return instruction
The `return` instruction, if any, is the last instruction in its block. See the behavior in the description of the body of a function.

### Expressions as instructions
Expressions used as instruction are evaluated, even if they do not produce side effects. The notation is as indicated in the grammar (expression followed by ;).

### Print instructions
The keywords `write` and `writeln` can be used to display values ​​in the program output. The first one displays the expression without changing lines; the second one displays the expression changing lines. When there is more than one expression, the various expressions are displayed without separation. Numeric values ​​(integer or real) are printed in decimal. Character strings are printed in native encoding. Pointers cannot be printed.

## Expressions
An expression is an algebraic representation of a quantity: all expressions have a type and return a value.

There are primitive expressions and expressions that result from the evaluation of operators.

The following table shows the relative precedence of operators: it is the same for operators on the same line, with the following lines having lower priority than the previous ones. Most operators follow the semantics of the C language (except where explicitly indicated). As in C, the logical values ​​are 0 (zero) (false value), and non-zero (true value).

In addition to the expressions in the table, there are also some special ones for tensors (see below).


| Expression Type | Operators | Associativity | Operands | Semantics |
|-----------------|-----------|---------------|----------|-----------|
| primary | ( ) [ ] | non-associative | - | square brackets, indexing |
|unary | + - ? | non-associative | - | identity and symmetric, position indication |
| multiplicative | * / % | from left to right | integers, reals, tensors | C (% is only for integers and cannot be applied to tensors). The application of operators to tensors results in coefficient-to-coefficient operations (if applied to two tensors with the same shape) and the operation between the scalar and each element of the tensor (if applied to a tensor and a scalar). |
| additive | + - | from left to right | integers, reals, pointers, tensors | C: if they involve pointers, they calculate: (i) displacements, i.e., one of the operands must be of type pointer and the other of type integer; (ii) pointer differences, i.e., only when the operator - is applied to two pointers of the same type (the result is the number of objects of the type pointed to between them). If the memory is not contiguous, the result is undefined. Applying the operators to tensors results in coefficient-to-coefficient operations (if applied to two tensors with the same shape) and the operation between the scalar and each element of the tensor (if applied to a tensor and a scalar). |
| comparative | < > <= >= | left to right | integers, reals | C |
|equality | == != | left to right | integers, reals, pointers, tensors | C. Applying the operators to tensors results in coefficient-to-coefficient operations (if applied to two tensors with the same shape). |
| logical "not" | ~ | non-associative | integers | C |
| logical "and" | && | left to right | integers | C: the 2nd argument is only evaluated if the 1st is not false. |
| logical "or" | \|\| | left to right | integers | C: the 2nd argument is only evaluated if the 1st is not true. assignment = right-to-left all types The value of the expression on the right-hand side of the operator is stored in the position indicated by the left-value (left operand of the operator). Integer values ​​can be assigned to real left-values ​​(automatic conversion). In other cases, both types must agree. |


### Primitive expressions
Literal expressions and function invocations were defined above.

### Identifiers
An identifier is an expression if it has been declared. An identifier can denote a variable.

An identifier is the simplest case of a left-value, that is, an entity that can be used on the left side of an assignment.

### Reading
The operation of reading an integer or real value can be performed by the expression indicated by the reserved word `input`, which returns the value read, according to the expected type (integer or real). If used as an argument of the printing operators or in other situations that allow several types (write or writeln), an integer must be read.

Examples: `a = input` (reading for a), `f(input)` (reading for function argument), `write input` (reading and printing).

### Curved parentheses
An expression between curved parentheses has the value of the expression without the parentheses and allows the priority of the operators to be changed. An expression in parentheses cannot be used as a left-value (see also indexing expression).

### Expressions resulting from operator evaluation
Pointer indexing
Pointer indexing returns the value of a memory location indicated by a pointer. It consists of a pointer expression followed by the index in square brackets. The result of a pointer indexing is a left-value.

Example (access to position 0 of the memory zone indicated by p): `p[0]`

### Identity and symmetrical
The identity (+) and symmetrical (-) operators apply to integers, reals and tensors. They have the same meaning as in C.

Applying the operator to tensors results in the operation being distributed over all coefficients.

### Memory reservation
The memory reservation expression, indicated by objects (integer argument), returns the pointer that points to the memory zone, on the stack of the current function, containing enough space for the number of objects indicated by its integer argument.

Example (reserves vector with 5 reais, pointed to by p): `ptr<real> p = objects(5)`

### Position indication expression
The suffix operator ? applies to left-values, returning the corresponding address (with pointer type).

Example (indicates the address of a): `a?`

### Dimension expression
The sizeof operator applies to expressions, returning the corresponding dimension in bytes. Applied to a tensor, it returns the tensor's capacity (in bytes).

Examples: `sizeof(a)` (dimension of a).

### Tensor-dependent expressions
#### Tensor explicitly written in the program
Tensors can be written directly in programs. The format uses the bracket notation that surrounds expressions (these expressions must be convertible to floating-point values).

Examples:
- 2x2 tensor: `[[2,2], [2,3]]`
- 2x2 tensor with expressions: `[[2+1, i], [f(1), 2]]`

### Capacity
For a tensor t, the length, indicated by `t.capacity` (integer value), corresponds to the number of cells in the tensor (not to be confused with sizeof, which indicates the number of bytes occupied by these cells).

### Number of dimensions
For a tensor t, the number of dimensions is indicated by `t.rank` (integer value).

### Dimensions
For a tensor t, the dimensions, indicated by `t.dims` (pointer to an integer vector), correspond to the vector with the dimensions of the tensor. It is also possible to obtain a single dimension through the `dim` function: e.g. `t.dim(0`) (first dimension). Dimensions up to the limit defined by `t.rank` are valid.

### Indexing
Tensor indexing returns the value of a tensor position indicated by a set of coordinates (starting at 0). It consists of a tensor followed by the `@` operator and the coordinates to be indexed. The result of a tensor indexing is a left-value.

Example (accessing a position of the three-dimensional tensor t): `t@(x,y,z)` (x, y and z are integer expressions)

### Reshaping
For a tensor t1, with capacity D1 x ... x Dn (D1 ... Dn are the lengths in each dimension), the reshaping operation `t.reshape`(S1, ..., Sk) (S1 ... Sk are the lengths in each new dimension) produces a new tensor t2 with the same capacity as t1 and, consequently, where D1 x ... x Dn == S1 x ... x Sk. The arguments of reshape are always positive integer literals.

Example:
```c
tensor<2,2,2> t1 = [ [ [1, 2], [3, 4] ], [ [5, 6], [7, 8] ] ];
tensor<2, 4> t2 = t1.reshape(2,4);  // [ [1, 2, 3, 4], [5, 6, 7, 8] ]
```

### Contraction
Given two tensors, t1 and t2, the contraction operation, represented by t1 ** t2, is possible if the last dimension of the first tensor is equal to the first dimension of the second tensor. For matrices (i.e., two-dimensional tensors), this operation corresponds to the usual matrix multiplication. This operation has a precedence immediately above that of the usual multiplication.

Example:
```c
tensor<3,2,2> t1 = [[[1, 2], [3, 4]], [[5, 6], [7, 8]], [[9, 0], [8, 7]]];
tensor<2,2,3> t2 = [[[1, 2, 3], [4, 5, 6]], [[7, 8, 9], [0, 8, 7]]];
writeln t1**t2;
writeln t2**t1;
```
Result:
```bash
Tensor<3,2,2,3>[[[[1.5E1, 1.8E1, 2.1E1], [4, 2.1E1, 2E1]], [[3.1E1, 3.8E1, 4.5E1], [1.2E1, 4.7E1, 4.6E1]]], [[[4.7E1, 5.8E1, 6.9E1], [2E1, 7.3E1, 7.2E1]], [[6.3E1, 7.8E1, 9.3E1], [2.8E1, 9.9E1, 9.8E1]]], [[[9, 1.8E1, 2.7E1], [3.6E1, 4.5E1, 5.4E1]], [[5.7E1, 7.2E1, 8.7E1], [3.2E1, 9.6E1, 9.7E1]]]]
Tensor<2,2,2,2>[[[[3.8E1, 1.4E1], [4.1E1, 4.1E1]], [[8.3E1, 3.8E1], [9.5E1, 9.8E1]]], [[[1.28E2, 6.2E1], [1.49E2, 1.55E2]], [[1.03E2, 4.8E1], [1.12E2, 1.13E2]]]]
```


## Examples
The examples are not exhaustive and do not illustrate all aspects of the language.

### Program with several modules
Definition of the factorial function in a file (factorial.udf):
```c
public int factorial(int n) {
if (n > 1) return n * factorial(n-1); else return 1;
}
```

Example of using the factorial function in another file (main.udf):
```c
// external builtin functions
forward int argc()
string forward argv(int n)
forward int atoi(string s)

// external user functions
forward int factorial(int n)

// the main function
public int udf() {
int f = 1;
writeln "Test for the factorial function";
if (argc() == 2) f = atoi(argv(1)); writeln f, "!= ", factorial(f); 
return 0;
}
```

# How to compile
```bash
udf --target asm factorial.udf
udf --target asm main.udf
yasm -gdwarf2 -felf32 factorial.asm
yasm -gdwarf2 -felf32 main.asm
ld -melf_i386 -o factorial principal.o main.o -lrts
```

also:
```bash
make clean $$ make
./udf prog.udf
yasm -felf32 prog.asm
ld -m elf_i386 -o prog prog.o -lrts -L$HOME/compiladores/root/usr/lib
./prog
```