# GML

GML is a dynamically typed, higher-order, interpreted and embeddable
programming language.

# Why
One of the projects I'm currently working on; Neothye, needed a scripting
language primarly for constructing shaders and supplying global configuration
of the engine as well as configuration files for materials.

Another project of mine; Redroid, also required a configuration file format,
as well as a templating engine. INI files were considered at first but the
complexity of Redroid configuration required a hierarchical format like JSON.
Tables in GML work like JSON objects and a templating engine can be easily
constructed in GML as well.

# Philosophy

The general idea was to provide a language with the least amount of
native primitives while also being flexible and ignoring the concept
of user-definable types. To retain flexibility with such a minimal set
of primitives it was also important to consider a serise of builtin
primitive operations (like concatenation, folding, etc).

# Operators
* Binary operators: `+ - * /`
* Logical operators: `&& || !`
* Bitwise operators: `& |`
* Unary operators: `- + ++ --`
* Comparision operators: `< > <= >= == != same`

Objects in GML can be equal or the same. Objects are considered the same
when they refer to the same slot in the enviroment. Sameness is compared
for with the `same` operator. Provided is an analogue of the difference
in C.
```
const char *a = "hello";
const char *b = a; /* points to the same thing */
if (!strcmp(a, b)) { } /* equality */
if (a == b) { } /* sameness */
```

# Keywords

The following identifiers are reserved as they are keywords for the
language.

* while
* if
* else
* elif
* fun
* fn
* same

# Types

| Type     | Description                                             |
|----------|---------------------------------------------------------|
| Function | functions are first-class allowing lambdas and closures |
| Atom     | symbolic names for constants and table fields           |
| String   | string of text allowing unicode characters              |
| Array    | an array                                                |
| Table    | a dynamic dictionary which can be keyed by anything     |

# Control

Control flow is acomplished with `if` `else` `elif` and `while`. Scopes
take on the form `{ }`.

# Functions

There are two ways to declare a function. Standard functions take on
the form:
```
fun name(formals) {
    body;
}
```
Whereas anonymous functions take on the form:
```
fn(formals) {
    body;
}
```

Function return values are implicit, the last statement in a function
is the return value.

Anonymous functions can also take on short hand syntax if they only
contain a single expression:
```
fn(formals) => expr
```

The following two are equivalent:
```
map(fn(x) => x * x, range(1, 11));
map(fn(x) { x * x }, range(1, 11));
```

Function calls are placed with:
```
name(formals);
```

# Atoms
Symbolic constants or keywords take on the form: `:name`. Atoms are
generally useless in function or global scope and are primarly used
for tables. Atoms can be used as symbolic constants. Currently there
are three implicitly defined atomic constants: `:nil, :true, :false`.

# Strings
Strings take on two forms, either `"string"` or with single character
quotes, `'string'`.

Strings can be subscripted as well to retrive a specific character,
subscripting takes on the form.
```
string[index]
```

Strings are immutable, while you may subscript to retrieve a character
at some index you cannot modify the character within the string.

Strings can be concatenated with the binary `+` operator.

Strings can be compared with the logical operators, `== !=`.


# Arrays
Array literals take on the form:
```
name = [ values ];
```

You may specify any amount of values within the brackets delimited by
comma, for example.
```
name = [ 1, 2, "hello", 3.14, :false ];
````

Array subscripting takes on the form:
```
name[index]
```

Arrays are mutable which means you may subscript the array and modify
the contents at a specific index.

Arrays can be concatenated with the binary `+` operator.

Arrays can be compared with the logical operators `== !=`.

# Tables
Tables are dictionaries that operate like dynamic structures, they can
be keyed by any type except a table itself; however, they are often keyed
with atoms.

Tables take on the form:
```
table = { :member = value, :foo = "123", "bar" = 1 };
```

For atoms as keys, tables can be subscripted with:
```
table.member
```

For keys which are not atoms, subscripting can be acomplished with the
value of the key itself, for instance:
```
table["bar"];
```

This also allows for subscripting with atoms as values; thus, the following
two are functionally equivlant.
```
table[:member]
table.member
```

Tables are dynamic, you can add keys and values after the table was
initially created, for example:
```
table = { :a = 1, "b" = 2 };
table.c = 1;
table["c"] = 2;
````

Tables can be compared with logical operators `== !=`.

# Examples

The `range` function allows you to construct an array of integers within
some range.
```
>>> range(0, 5);
[0, 1, 2, 3, 4]
>>> range(-5, 3);
[-5, -4, -3, -2, -1, 0, 1, 2]
```

The `map` function lets you map a function to a sequence such that
the function is called for each of the sequence's items and returns an
array of the return values. For example we can compute some cubes:
```
>>> map(fn(x) => x * x * x, range(1, 11));
[1, 8, 27, 64, 125, 216, 343, 512, 729, 1000]
```

The `filter` function lets you pick out items in a sequence with
a function which needs to evaluate `:true`. A list is constructed for
each of the sequence's items where that function returns `:true`.
```
>>> filter(fn(x) => x % 2 == 0, range(1, 6));
[2, 4]
```

The `reduce` function returns a single value constructed by calling
a binary function on the first two items of the subsequence, then on the
result and the next item, and so on. For example, to compute the sum
of a range of numbers:
```
>>> reduce(fn(x, y) => x + y, range(1, 11));
55
```

Another good example for the `reduce` function is constructing strings
from an array of strings:
```
>>> reduce(fn(x, y) => x + y, [ "hello ", "world" ]);
"hello world"
```

The `length` function lets you query the length of an array or a string:
```
>>> length(range(1, 11));
10
>>> length("hello world");
11
```

The `print` and `println` functions let you print and print lines. They
take variable number of arguments and are formatted in the order arguments
are passed to it.
```
print(1, 3.14, [1, 2, 3]);
1 3.14 [1, 2, 3]
```

You can search for substrings in a string with `strstr`. It will just
return `:true` or `:false` if the needle is found in the haystack.
```
>>> strstr("hello world", "world");
:true
```

You can also pick out a substring in a string with `substr`.
```
>>> substr("hello world", 6, 5);
"world"
```

There are a plethora of math functions as well.
