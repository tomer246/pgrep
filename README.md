# pgrep

`pgrep` is a multi-threaded grep utility, that uses custom regex.
The regex is implemented using automatas and algorithms for tranforming nfa to dfa.

Currently pgrep is only used for it's custom regex and it's multi-threading efficiency.
In the future, It should support searching in directories (optionally with recursion).

### COMPILING
  ```gcc -pthread pgrep.c -o pgrep```
  
### USAGE
  ```./pgrep [-A <#>|-f <file>|-i|-R|-v|-V] <search term>```
  
  type ```./pgrep --help``` for help
