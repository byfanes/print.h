# print.h

A lightweight C89 printing library designed around a small print context system.
It supports writing formatted output to files, fixed buffers, or user-defined string targets without relying on a format string parser.

## Features

* C89 compatible design
* File, buffer, and custom string output
* Small internal buffers for number formatting
* Integer formatting:

  * decimal
  * hexadecimal
  * octal
* Floating point formatting (in a basic level)
* Customizable output functions
* Optional use of libc `printf`
* Optional removal of dependency on `string.h`
* Simple extensible argument formatting system

## Basic Usage

Include the header:

```c
#include "print.h"
```

Create a print context like a variable this avoids global context by deisgn which is better for multi-threading:

```c
use_print_ctx;
```

Print to a file:

```c
file_print(stdout, arg_str_lit("Hello World"));
```

Print multiple values:

```c
file_print(stdout,
    arg_str_lit("Value: ")
    arg_dec(number)
    arg_str_lit("\n")
);
```

Print with automatic newline:

```c
file_println(stdout, arg_str_lit("Hello"));
```

## Buffer Output

Write into a fixed buffer:

```c
char buffer[64];

buf_print(buffer, sizeof(buffer),
    arg_str_lit("Number: ")
    arg_dec(123)
);
```

The `*_wrote` versions also writes how many bytes written to given variable(or lvalue):

```c
size_t written;

buf_print_wrote(buffer, sizeof(buffer), written,
    arg_str_lit("Hello")
);
```

## Available Arguments

Built-in formatters:

| Macro                   | Description                            |
| ----------------------- | -------------------------------------- |
| `arg_str(str)`          | Write a null-terminated string         |
| `arg_str_len(str, len)` | Write a string with a specified length |
| `arg_str_lit(str)`      | Write a string literal                 |
| `arg_dec(num)`          | Write a decimal integer                |
| `arg_hex(num)`          | Write a hexadecimal integer            |
| `arg_oct(num)`          | Write an octal integer                 |
| `arg_fp(num)`           | Write a floating point number (basic)  |
| ----------------------- | -------------------------------------- |

## Custom Output

The library uses a print context internally. New output targets can be added by extending the context type and implementing the required writer functions.

For string output, define:

```c
#define PRINT_STRING_OUT(str, buf, len) your_function(str, buf, len)
```

The function should return the number of bytes written.

## Configuration

The following macros can be defined before including the header:

### Use uppercase hexadecimal

```c
#define PRINT_USE_CAPITAL_HEX
```

### Custom floating separator

```c
#define PRINT_FLOAT_SEPARATOR ","
```

### Custom null string

```c
#define PRINT_NULL_STRING "(nil)"
```

### Custom newline

```c
#define PRINT_NEW_LINE "\r\n"
```

### Use libc printf backend for floating point numbers

```c
#define PRINT_USE_PRINTF_FOR_FP
```

### Use libc printf backend

```c
#define PRINT_USE_PRINTF
```

### Avoid string.h

```c
#define PRINT_DONT_USE_STRING_H
```

## Implementation

Like many single-header C libraries, define `PRINT_IMPLEMENTATION` in one source file:

```c
#define PRINT_IMPLEMENTATION
#include "print.h"
```

Other files should only include:

```c
#include "print.h"
```

## Design

The library uses a context-based pipeline system. Formatting functions return a state that allows the next operation to continue or stops execution when an error occurs.

This allows expressions like:

```c
file_print(stdout,
    arg_str_lit("Hex: ")
    arg_hex(value)
);
```

to expand into a sequence of small writing operations while keeping the API lightweight.
