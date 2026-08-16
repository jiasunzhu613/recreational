# Lisp Interpreter

A minimal C-based Lisp interpreter project with a REPL, parser, AST, and basic object support.

## Getting Started

Requirements:
- `gcc` or any other C compiler (and be sure to change `CC` flag in `Makefile`)
- `make`

Build the interpreter:
```sh
make
```

Run the interpreter:
```sh
./main.out
```

## Built-in Functions

The interpreter currently provides the following built-in functions. Numeric
operations accept fixnums (integers), and predicates return `#t` or `#f`.

| Function | Description | Example |
| --- | --- | --- |
| `+` | Adds two or more numbers. | `(+ 1 2 3)` => `6` |
| `-` | Negates one number, or subtracts subsequent numbers from the first. | `(- 10 3 2)` => `5` |
| `*` | Multiplies two or more numbers. | `(* 2 3 4)` => `24` |
| `>`, `<`, `>=`, `<=` | Compares exactly two numbers. | `(>= 4 4)` => `#t` |
| `eql` | Tests two numbers for equal value. | `(eql 3 3)` => `#t` |
| `atom` | Returns `#t` when its argument is not a list. | `(atom 7)` => `#t` |
| `eq` | Tests whether two evaluated objects are the same object. | `(eq 'foo 'foo)` => `#t` |
| `car` | Returns the first element of a non-empty list or pair. | `(car '(1 2 3))` => `1` |
| `cdr` | Returns everything after the first element of a non-empty list or pair. | `(cdr '(1 2 3))` => `(2 3)` |
| `cons` | Constructs a pair from two values. | `(cons 1 '(2 3))` => `(1 2 3)` |

Arguments are evaluated before a built-in function is applied. 

> !NOTE
> Due to lack of proper error handling for the time being, supplying an unsupported type or the wrong number of arguments currently produces no result (or just segfaults LOL)

## Sample LISP Code To Try
Basic naive, non-memoized factorial
```lisp
(defun factorial (x) (if (eql x 0) 1 (* x (factorial (- x 1)))))
(factorial 4)
```
