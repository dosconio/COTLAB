# UPDATE of COTLAB



### Update 2024 夏至

- 



### Update 2024 春分

Rewritten by C++



---



### Update 2023 重陽

20231005 stage:

- `str` `*` `int` → `strstr...`

20231008 stage:

- fix little mistake: `list` directive prints double `last` because of the `if` statement does not return and enter the next judgement

20231011 stage:

- fix the mistake of UNISYM's `TokenAll` about number as identifier
- a batch file can be dragged to COT.EXE and keep in shell, so you can see the result

20231016 stage:

- directive `cd`, `dir`
- decide two officially building versions: `Win64`(aka `W64`); `Lin64(Ubuntu)`(aka `U64`)
- help fix-bug yo `CoeDig` caused `sin(0.0001) ---> ,0.000000`, and re-sure its round modes
- fix-bug: the digits of `acos()` returned does not match the show_precise
- change prompt `<E:\PROJ\SVGN\COTLAB>` to `>E>\PROJ\SVGN\COTLAB>` for example, to match `>>> `

20231017 stage:

- prompt for Windows to `>E>PROJ/SVGN/COTLAB>` for example
- directive `fdump`
- instant function `ffset` with `dest_file sors_file sec_id`



----



### Update 2023 寒露

Big change: Complex Arithmetic;

20230929 stage:

- copyright information
- change directive `fast` into `mode`
- cancel `Line ...` prefix for single line output, for pipe using, but endless case appears e.g. `cot 1+2 | cot`
- fix mistake of negative operation

20230930 stage:

- help USL remake `Cde::coeln2`, `hs::StrTokenAll`(to accept `""`), change `#ifndef _ARN_FLAG_DISABLE` to `#ifdef _ARN_FLAG_ENABLE` in `ustring.c`.

  - > I conflicted with Arina in `CoeAr`, which, while capable of representing a wide range of numbers, greatly reduced hardware compatibility and computation speed. It is hoped that a limited length version of her CDE architecture will be supported directly by hardware in the future.

- spaces before a line is invalid except the console shell mode.

- accept strcat `+` , e.g. `"abc"+"def" ---> "abcdef"`

20231001 stage:

- help USL `DatimeCalendar` and update COTLAB
- use locale time but UTC time by default

20231005 stage:

- support datatype and operations of **complex**, e.g. `1.2e-3j`

---



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

20230926 stage:

- `cal` function, whose name is compatible with Linux. `Break the balance between with Linux`
- fix mistake of abort `Match error`

20230927 stage:

- covert single identifier to function, e.g. `cal` to `cal()` when `cal` is not an object registered

- add icon for GCC version (Here shared)

- Adapt to Ubuntu Linux 64bit, the input of 3 bytes of direction-keys should be solved by users  
