## Typedefs
### Maybe Void
Every operationally `void` function that may return an error in the form of a negative integer shall be defined with type `maybe_void`, and will use the macros `fail(n)` and `succeed` to terminate or return execution.

## Error handling

### Maybe Void (repetita)
Every operationally `void` function that may return an error in the form of a negative integer shall be defined with type `maybe_void`, and will use the macros `fail(n)` and `succeed` to terminate or return execution. Note that, to return -1, you want to write `fail(1)`, NOT `fail(-1)` (although it also works if you write the negative number, there is a check).

### ...on fail
- `int halt_on_fail (maybe_void x)` halts whenever `x` is the result of a failure.
- `int wat_on_fail (maybe_void x)` *wat..?*s on fail
- `int wat_err_on_fail (maybe_void x)` *wat..?*s and prints the error code on fail
- `int msg_on_fail (maybe_void x, const char * msg)` prints a custom message and the error code on fail.
