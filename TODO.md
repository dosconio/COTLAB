Ignore this file

- [ ] 暂时放弃 RFV4 之前努力构建的 RegAr， 改用对十进制友好的 CoeAr，所以先把 CoeAr 对应的部分再完善一下；画出解析 (f"A"+"B"*2) 的流程图；解析时，总 nested-structure 与 总 dynamic unprocessed-symbols-included block dnode with current priorities of operators 相结合。
  - [x] Debug hstring::Chr`+-*/`
  - [ ] Adapt Coe`+-*/`
- [ ] Rank mi Symbol Parse `()`, `+`, `×`, 核实 MALC_COUNT
- [ ] Command? File Process: Interpret line by line
  - [ ] `无参数`或 `-s`: 启动Shell, 逐行解析执行
  - [ ] `-c`: 执行输入的语句，可能有多条
  - [ ] `-f`: 从文件中读入
- [x] Opensrc with GNU License and make README.md "一切皆函数"，使用的语言同名
- [ ] erro 机制
- [ ] ClearParsers
- [ ] float.h 函式以及 Cowiki 同步
- [ ] VSCode 拓展，然后于 **Bilibili** 发行“【预告】”，之后有时间就不断在 **知乎** 上推介 UNISYM