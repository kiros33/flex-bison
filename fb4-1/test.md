# Parsing SQL
* Example 4-1. MySQL lexer
* Example 4-2. MySQL subset parser

### Manually build
* Compile test
```
bison parser.y
flex scanner.l
g++ -c -o parser.opp parser.cpp
g++ -c -o scanner.opp scanner.cpp
```

* Build test
```
g++ -o fb parser.cpp scanner.cpp
```

* stdin
```
make test0
```

* file input
```
make test
make test1
```