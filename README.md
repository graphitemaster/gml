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

# Types

| Type     | Description                                             |
|----------|---------------------------------------------------------|
| Function | functions are first-class allowing lambdas and closures |
| Atom     | symbolic names for constants and table fields           |
| String   | string of text allowing unicode characters              |
| Array    | an array                                                |
| Table    | a dynamic dictionary which can be keyed by anything     |


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
