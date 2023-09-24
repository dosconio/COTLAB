# UPDATE of COTLAB



### Update 2023 中秋節

Last stage fv21. Release 20230924

20230922 stage:

- non-function identifier linkage now is while executing
  - accept `a=b`
- nested assignment, e.g.`a=b=1`
- fix the mistake of directive `%`

20230923 stage:

- ask senpai for fixing the mistakes related to `unisym's CoeInt()` and directive token, e.g. `int(2.2) ---> 0`, `23%17 ---> 23` in COTLAB
- remove redundant warnings
- factorial arithmetic, e.g. `2!! ---> 2` (Here shared with my partner)
- solve mistakes for {`a=()`,  }
- `help`

20230924 stage:

- `(a)(co-/tan-)sin(h)` family
- Update `DtrPow`
- Identifier undefined check enable (not-ripen) (Here shared with my partner)
- fix `last` related
- use `fast` to enable faster arithmetic mode
- add icon for MSVC version
- `1` is integer type but float type, and `+ -` `int` `+ - *` `int` ---> `int`
- Linux host support with simple test, e.g. Ubuntu Community
