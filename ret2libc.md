
Given a program: https://github.com/nastyax0/Low-level-security_writeups/blob/master/ret2libc.c

# Introduction:
_This one got no crackme but a simple c program,_

ret2libc type of exploitation are of ROP (Return Orineted Programming) type.

> "What is ROP, what are gadgets(or vehicles), exactly which library function it has to return to, why that library or function?"

These were the questions that popped up in my mind while researching about the technique.

So I felt to start of with simple file first to understand the technique behind before jumping to a program which is unknow to us.

compile it by:  
```bash
gcc -w -D_FORTIFY_SOURCE=0 -fno-stack-protector -z execstack -no-pie -o ret2libc ret2libc.c
```

_[for Debian System]:_
To disable ASLR:

```bash
sysctl kernel.randomize_va_space
```
if kernel.randomize_va_space = 2 or 1 change it to zero,

```bash
sudo sysctl -w kernel.randomize_va_space=0
```

![Screenshot 2024-11-15 093917](https://github.com/user-attachments/assets/c4916221-aabc-4062-b5de-d21a0b4e2c82)

![Screenshot 2024-11-15 095321](https://github.com/user-attachments/assets/29a8a9a7-2a27-4d9a-8e1e-c5b9b62ac4c1)

# Investiagtion:

Reversing the program:

![Screenshot 2024-11-15 083408](https://github.com/user-attachments/assets/eabe4088-6b68-465c-8bed-03b8c3a8862f)

A stripped binary!
Upon further inspection,

```sh
  nm ./ret2libc
```

![Screenshot 2024-11-15 083458](https://github.com/user-attachments/assets/eb25c635-84cc-42c5-9496-f15a134d500e)

Two interesting functions like main and secure(?)

Or, disassemble the entry point,

![Screenshot 2024-11-15 101139](https://github.com/user-attachments/assets/4766a3da-f145-4dd9-8b66-6addfb609795)

![Screenshot 2024-11-15 101218](https://github.com/user-attachments/assets/f78073ab-ce38-4f42-86b8-9dfddb139ce3)

then disassemble main (call function),
 
![Screenshot 2024-11-15 101357](https://github.com/user-attachments/assets/e5ecb2ab-11b0-4504-8ae6-f24d345c933d)

First lets get the address of the function secure

```sh
  info function secure
```
![Screenshot 2024-11-15 101421](https://github.com/user-attachments/assets/fd40cc18-59c9-4c2b-a1cf-1f941babbc3e)
>Non-debugging symbols:
>0x00000000004011a6  secure

Disassemble secure function,

![Screenshot 2024-11-15 125709](https://github.com/user-attachments/assets/692c9322-6cc6-489e-b098-50caba6f1da7)

Here, we find system function call, 

further by using checksec:
> checksec --file=ret2libc

![Screenshot 2024-11-15 125838](https://github.com/user-attachments/assets/461a803d-6bd3-4b19-9419-97ed010a92c4)

By this we got to know we can execute from stack and protections are off, cause we did, just to show that we can easily try to execute bin/shell from stack without much hastle.

# Analysis
By wikipedia, 
> Return-oriented programming (ROP) is a computer security exploit technique that allows an attacker to execute code in the presence of security defenses
> such as executable space protection and code signing.

> In this technique, an attacker gains control of the call stack to hijack program control flow and then executes carefully chosen machine instruction sequences 
> that are already present in the machine's memory, called "gadgets". Each gadget typically ends in a return instruction and is located in a subroutine within the > existing program and/or shared library code. Chained together, these gadgets allow an attacker to perform arbitrary operations on a machine employing
> defenses that thwart simpler attacks.

### _What does all this means?_
In simpler terms our stack looks like this,

![IMG_20241115_130706](https://github.com/user-attachments/assets/e49220e0-58d4-4963-b903-dc0681d1162f)

main and secure has no connection and secure is calling function system, morever we know program is using gets functions we could do buffer overflow to reach till return.

So rather we aim to exploit a buffer overflow vulnerability, specifically designed to demonstrate Return-Oriented Programming (ROP) and ret2libc exploitation. The goal is to execute a system call that runs the command "/bin/sh" to spawn a shell.

For this to work we have to find where system() and bin/sh are located,
For the system its evident in secure function or we could 

```bash
info function system
```

> 0x401040 <system@plt>

further for bin/sh, I will try to find in libc/so.6
by 
```bash
info proc mapping
```
we got ranges, by that

![Screenshot 2024-11-15 104842](https://github.com/user-attachments/assets/81131039-6722-4a91-b595-dcf25aa76893)

![Screenshot 2024-11-15 132123](https://github.com/user-attachments/assets/5ea61e51-ba17-4bcb-86c7-06d41fcefc40)


# Strategy
We identified the system() function and the /bin/sh string in the libc library, which would be used in the ROP chain.
We will attempt a buffer overflow attack where we first fill the buffer with "A"s (to fill up to the return address) and then crafted a payload to redirect execution to the system() function with the /bin/sh string as an argument

ROP Chain Construction:
The core idea of the attack is to use ROP gadgets to set up the registers for the system call:
rdi for the first argument to system(), which is the address of "/bin/sh".
rsi and rdx were set to 0 because system() only takes one argument.
The crafted ROP chain involved:
Overwriting the return address of a function with the address of system() (the function we want to call).
Adding the address of "/bin/sh" (as a string) in rdi, ensuring the system call would execute correctly.

```bash
 python3 -c 'import sys; sys.stdout.buffer.write(b"A"*120 + b"\x40\x10\x40\x00\x00\x00\x00\x00" + b"\x31\xf6\x7f\xf6\x00\x00\x00\x00")' > payload.bin
```

![Screenshot 2024-11-15 132457](https://github.com/user-attachments/assets/9ad5120c-8823-40e8-b6b6-6ab21d86b986)

I guess we have reached system, 










