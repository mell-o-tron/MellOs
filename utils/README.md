:warning: THIS IS OUTDATED AND WILL BE RENDERED IRRELEVANT ONCE WE PERKIFY :warning:

## Typedefs
### Maybe
The types `maybe_int` and `maybe_void` (which is an alias of the first) are used as return types for those functions that may either succeed or fail.
 [← Main README](../README.md)
```c
typedef struct {
    bool is_some;
    int val;

} maybe_int;
```

## Error handling

### Maybe
A bunch of macros and funcions are defined to make the process of dealing with options a bit easier.

```c
#define fail(code) return (maybe_int){false, code}      // returns a bottom value with an "error code"
#define succeed() return (maybe_int){true, 0}           // returns a top value
#define just(code) (maybe_int){true, code}              // wraps an int in an inhabited maybe_int
#define failed(x) !x.is_some                            // checks if failed
#define succeeded(x) x.is_some                          // checks if succeeded

int unwrap (maybe_int x);                               // unwraps value
```

### ...on fail
- `int halt_on_fail (maybe_void x)` halts whenever `x` is the result of a failure.
- `int wat_on_fail (maybe_void x)` *wat..?*s on fail
- `int wat_err_on_fail (maybe_void x)` *wat..?*s and prints the error code on fail
- `int msg_on_fail (maybe_void x, const char * msg)` prints a custom message and the error code on fail.
