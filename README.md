# VA_OPT Polyfill

A portable C preprocessor polyfill for `__VA_OPT__`, providing consistent 
behavior across C99, GNU, MSVC, and modern compilers with native `__VA_OPT__` 
support.

The C23 standard introduced `__VA_OPT__`, which conditionally expands content
based on whether variadic macro arguments are empty. This library provides
`VA_OPT` and related macros that work across a wide range of compilers to make
it easier to write more portable code with `__VA_OPT__` type functionality.

## Basic Example

```c
#include <stdio.h>
#include "va_opt.h"

#define LOG(fmt, ...) \
    printf(fmt VA_OPT((__VA_ARGS__), ,) __VA_ARGS__)

// Usage:
LOG("Hello");           // Expands to: printf("Hello")
LOG("Value: %d", 42);   // Expands to: printf("Value: %d" , 42)
```

This is equivalent to the C23 syntax:

```c
#define LOG(fmt, ...) \
    printf(fmt __VA_OPT__(,) __VA_ARGS__)
```

## Quick Start

1. Copy `va_opt.h` into your project
2. Include the header:
   ```c
   #include "va_opt.h"
   ```
3. Use `VA_OPT((__VA_ARGS__), expansion)` in your macros

The library automatically detects and uses the best available implementation for
your compiler.

## Public Macros

### `VA_OPT((__VA_ARGS__), ...)`

Conditionally expands to the content after `(__VA_ARGS__)` if `(__VA_ARGS__)`
is not empty.

**Parameters:**
- `(__VA_ARGS__)` — A parenthesized argument list to test for emptiness
- `...` — Content to expand when the args list is not empty

**Returns:** The variadic content if the args list is not empty; nothing 
otherwise.

```c
#define CALL_1(fn, ...) fn(1 VA_OPT((__VA_ARGS__), ,) __VA_ARGS__)

CALL_1(foo)           // Expands to: foo(1)
CALL_1(foo, 2, 3)     // Expands to: foo(1 , 2, 3)
```

### `VA_NOPT((__VA_ARGS__), ...)`

The inverse of `VA_OPT`. Conditionally expands to the content after
`(__VA_ARGS__)` if it is empty.

**Parameters:**
- `(__VA_ARGS__)` — A parenthesized argument list to test for emptiness
- `...` — Content to expand when `(__VA_ARGS__)` is empty

**Returns:** The variadic content if `(__VA_ARGS__)` is empty; nothing
otherwise.

```c
#define DEFAULT_100(...) \
    VA_NOPT((__VA_ARGS__), 100) VA_OPT((__VA_ARGS__), __VA_ARGS__)

DEFAULT_100()      // Expands to: 100
DEFAULT_100(42)    // Expands to: 42
```

### `VA_ISEMPTY(...)`

Tests whether the argument list is empty.

**Parameters:**
- `...` — Arguments to test

**Returns:** `1` if the arguments are empty, `0` otherwise.

```c
VA_ISEMPTY()        // Expands to: 1
VA_ISEMPTY(a)       // Expands to: 0
VA_ISEMPTY(a, b)    // Expands to: 0
```

### `VA_NOTEMPTY(...)`

Tests whether the argument list is non-empty. The logical inverse of
`VA_ISEMPTY`.

**Parameters:**
- `...` — Arguments to test

**Returns:** `1` if the arguments are non-empty, `0` otherwise.

```c
VA_NOTEMPTY()       // Expands to: 0
VA_NOTEMPTY(a)      // Expands to: 1
VA_NOTEMPTY(a, b)   // Expands to: 1
```

### `VA_OPT_SUPPORTED`

A predefined macro that expands to `1` if the compiler natively supports
`__VA_OPT__`, and `0` otherwise. This can be useful for conditional compilation
or diagnostics.

## Implementation Selection

The library automatically selects the most appropriate implementation based on
compiler detection:

| Priority | Implementation | Condition |
|----------|---------------|-----------|
| 1 | Native | `__VA_OPT__` is supported |
| 2 | GNU | GCC style comma elision (`,##__VA_ARGS__`) |
| 3 | MSVC | MSVC traditional preprocessor comma elision |
| 4 | C99 | Portable fallback using argument counting |

### Manual Override

Define exactly one of the following before including `va_opt.h` to force a
specific implementation:

```c
#define VA_OPT_USE_NATIVE   // Force native __VA_OPT__
#define VA_OPT_USE_GNU      // Force GNU comma elision
#define VA_OPT_USE_MSVC     // Force MSVC comma elision
#define VA_OPT_USE_C99      // Force portable C99 polyfill
```

## Compiler Compatibility Matrix

- Compilers tested via godbolt.org, an amazing resource

| Legend:                   |    |
| ------------------------- | -- |
| Did not compile:          | ❌ | 
| Did not pass tests:       | ⚠️ | 
| Passed tests:             | ✅ | 
| Compiler Warnings:        | ℹ️ |
| Default auto selected impl| ⭐ |

| CC                       | Native | GNU  | MSVC | C99   |
| ------------------------ | ------ | ---- | ---- | ----- |
| GCC 15.2                 | ⭐✅   | ✅   | ❌   | ✅    |
| GCC 15.2 (-std=c99)      | ⭐✅   | ⚠️  | ❌   | ✅    |
| GCC 3.4.6*               | ❌     | ⭐✅ | ❌   | ✅    |
| Clang 21.1.0             | ⭐✅   | ✅   | ❌   | ✅    |
| Clang 3.4.1              | ❌     | ⭐✅ | ❌   | ✅    |
| MSVC v19.44 (Conformant) | ⭐✅   | ✅   | ❌   | ✅    |
| MSVC v19.44              | ❌     | ⚠️  | ⭐✅ | ✅ℹ️ |
| MSVC v19.20              | ❌     | ⚠️  | ⭐✅ | ✅ℹ️ |
| NVC 25.11                | ⭐✅   | ✅   | ❌   | ✅    |
| ICC 2021.10.0            | ❌     | ⭐✅ | ❌   | ✅    |
| CompCert 3.12**          | ⭐✅   | ⚠️  | ❌   | ✅    |
| TCC 0.9.27**             | ❌     | ⚠️  | ❌   | ⭐✅  |
| SDCC 4.5.0**             | ⭐✅   | ⚠️  | ❌   | ✅    |

\* This compiler outputted assembly which was then assembled and run locally with gcc 15.2.1.  
\** This compiler was only used for preprocessing and then the result was compiled locally with gcc 15.2.1.

## C99 Polyfill Limitations

When using the pure C99 implementation (no compiler extensions), the following
edge case will not compile:

```c
#define MAC2(x, y) whatever

// This will NOT compile in C99 mode:
VA_OPT((MAC2), ...)
VA_ISEMPTY(MAC2)
```

Specifically, if the first argument is a macro that requires two or more
non-variadic parameters, the C99 polyfill will fail to compile.

This limitation does not apply when using the Native, GNU, or MSVC
implementations.

## Running Tests

The header includes a built-in test suite. Compile and run with:

```sh
# Test with automatic implementation selection
cc -x c -DTEST_VA_OPT va_opt.h -o va_opt_test && ./va_opt_test

# Test with specific implementations
cc -x c -DVA_OPT_USE_C99 -DTEST_VA_OPT va_opt.h -o va_opt_test && ./va_opt_test
cc -x c -DVA_OPT_USE_GNU -DTEST_VA_OPT va_opt.h -o va_opt_test && ./va_opt_test
cc -x c -DVA_OPT_USE_NATIVE -DTEST_VA_OPT va_opt.h -o va_opt_test && ./va_opt_test
```

For MSVC:
Ensure TEST_VA_OPT and/or VA_OPT_USE_MSVC are defined and the header is treated
as a .c file. Review the compiler compatibility table above to see which modes require conformance mode via the `/Zc:preprocessor` option.

## Technical Details

### Implementation Strategies

**Native (`__VA_OPT__`):** Directly uses the C23 `__VA_OPT__` feature when 
available.

**GNU Comma Elision:** Uses the GCC extension where `,##__VA_ARGS__` removes the
 preceding comma when `__VA_ARGS__` is empty.

**MSVC Comma Elision:** Uses the MSVC traditional preprocessor behavior where
`, __VA_ARGS__` automatically elides the comma for empty arguments. An
undocumented part of this feature is to skip comma elision when `,__VA_ARGS__`
appears in a macro argument list, and a workaround for this exclusion is
implemented.

**C99 Polyfill:** Employs an argument-counting technique based on Jens Gustedt's
 ISEMPTY macro from the P99 library, extended to support unlimited arguments.

## License

This library uses a triple-license structure:

- **CC0 1.0 Universal** — Primary code sections (public domain dedication)
- **Apache License 2.0** — P99-derived ISEMPTY implementation
- **Boost Software License 1.0** — Cloak.h-derived utility macros

As most compilers don't require the pure C99 implementation, you may remove all
Apache code from this header and therefore remove the attribution requirements
from any compiled binary which stem from the use of this library.

See the [LICENSES](LICENSES/) directory for full license texts.

## Credits

- **Jens Gustedt** — P99 ISEMPTY technique
- **Paul Fultz II** — Cloak.h utility macros

## Version

The version is available programmatically via `VA_OPT_H_VERSION`.
