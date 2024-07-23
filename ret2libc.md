This one got no crackme but a simple c program,

ret2libc type of exploitation are of ROP (Return Orineted Programming) type.

> "What is ROP, what are gadgets(or vehicles), exactly which library function it has to return to, why that library or function?"

These were the questions that popped up in my mind while researching about the technique.

So I felt to start of with simple file first to understand the technique behind before jumping to a program which is unknow to us.

Given a program:


First lets get the address of the function secure(This is unstripped binary thats why its easy I need to research how to do same for stripped[maybe topic for another MD])

```sh
  info function secure
```

> 0x00007ff63e0c1601  secure

//edit trm




