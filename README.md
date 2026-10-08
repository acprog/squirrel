# Squirrel plugin for Unreal engine
Squirrel script plugin for unreal engine

Code from the following repositories is used:

https://github.com/albertodemichelis/squirrel
Copyright (c) 2003-2022 Alberto Demichelis

https://github.com/matusnovak/simplesquirrel
Copyright (c) 2019 Matus Novak matusnov@gmail.com

Differences from Squirrel 3.2:
1. The 'in' operator for arrays works according to the logic from table — to check if an element exists in the array.
2. local is required before variables in foreach (local ...).
3. Automatic .bindenv(this) for all nested functions.
4. Array concatenation using + and += in addition to the extend() method.
5. The find() method for tables, similar to array (search by key, returns the value). 
6. The array now includes the indexof and removeat methods (duplicating find and remove).
7. Initialization of default values for fields inherited from a C++ class.
8. The tostring() methods for the table and array print their contents.
9. Creation of nested classes.
10. The unary ! operator for variable checking.
11. An optional + sign before positive numbers is allowed.
12. Passing int as float and float as int to function parameters is allowed.
13. the ability to access array elements from the end using negative indices [-1]
14. array slice [:] similar to Python
15. The startswith() and endswith() methods have been added to the string.
