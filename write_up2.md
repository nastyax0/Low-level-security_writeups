Given a pcap: 


# Introduction:

_In this writeup we are going to reverse an ELF,_  
Its very much given in statement that we have to perform buffer overflow,

Downloading gives us PCAP file, 
 
Its an elf and has been sent through MIME protocol, 

Copy the thing as base64 and decrypt it by this command and save it as elf: 
```bash
base64 -d file.elf.b64 > file.elf
```

We get an elf so let's give it permission to execute and it leads us to login thing:
 


BTW,

 


 

We have a lots of sections here like `.text` , `.plt (procedure linkage table)` , ` .data` etc etc
But I am very interested in the .text section obviously as it contains information about our elf and we may find main there.
 
So lets examine that address for that you may use:
```assembly
x/50i [address]
```

 
Okay it just spitted out its main but we can easily calculate the address ourselves too.
>[rip+0x168]  = 0x555555555091 + 0x168 = 0x555555555200 

Nice, let's examine that address too 
```assembly
x/30i 0x555555555200
```
We got a login function written out there. Let's set a breakpoint there.
 



## Understanding(Setting up the base):


Actual thing to reverse now:

 

I like to execute the program and go in depth about the behavior of the program. Here's what I found
 
We can see a fgets()
```assembly
   0x00005555555551b3 <+74>:	mov    esi,0xa0    <= 160 bytes
   0x00005555555551b8 <+79>:	mov    rdi,rax
   0x00005555555551bb <+82>:	call   0x555555555050 <fgets@plt>
   0x00005555555551c0 <+87>:	mov    rax,QWORD PTR [rbp-0x8]
```
But this fgets() accepts 160 bytes? Nice!

Fine, we can see two mallocs there, they are assigned different sizes:
 ```assembly
0x0000555555555182 <+25>:	mov    edi,0x10       =>16 bytes 
   0x0000555555555187 <+30>:	call   0x555555555060 <malloc@plt>
   0x000055555555518c <+35>:	mov    QWORD PTR [rbp-0x10],rax
   0x0000555555555190 <+39>:	mov    edi,0x8          => 8bytes
   0x0000555555555195 <+44>:	call   0x555555555060 <malloc@plt>
   0x000055555555519a <+49>:	mov    QWORD PTR [rbp-0x8],rax          =>storing the returned pointer to memory
   0x000055555555519e <+53>:	mov    rax,QWORD PTR [rbp-0x8]         =>accesing the variable
 
   0x00005555555551a2 <+57>:	mov    DWORD PTR [rax],0x1                => storing one byte to the memory[rax]
   ```


I assume this one byte is our uid as:{keep a check on rax and eax}


 ```assembly
   0x00005555555551d9 <+112>:	mov    rax,QWORD PTR [rbp-0x8]
   0x00005555555551dd <+116>:	mov    eax,DWORD PTR [rax]
   0x00005555555551df <+118>:	test   eax,eax
```

See? Program is then testing eax if its non zero clear flag, and make it jump to 0x51f8
But, if our eax is zero then it would set ZF=1 and would NOT jump.

>(test performs a Bitwise AND on the two register essentially to check zero or non zero , its similar to and (instruction but does not store result))

Lets input some input to check behavior of our eax[rax]:
1)	If buffer size is shorter than 32 bytes:

 


See? Our rax is 0x1 its our uid!

2)	If buffer size is longer than 32 bytes:

             
 

Take a look at uid: it has got overwritten by 0x49494949(in decimal)

But why did I choose 32? See our Is are getting overwritten (0x49), it's clear indication at 32rd it's going to get overwritten.

But why exactly 32? 
```assembly
    0x0000555555555182 <+25>:	mov    edi,0x10
   0x0000555555555187 <+30>:	call   0x555555555060 <malloc@plt>
   0x000055555555518c <+35>:	mov    QWORD PTR [rbp-0x10],rax   <= buffer pointer stored here
   0x0000555555555190 <+39>:	mov    edi,0x8
   0x0000555555555195 <+44>:	call   0x555555555060 <malloc@plt>
   0x000055555555519a <+49>:	mov    QWORD PTR [rbp-0x8],rax   <= uid pointer stored here
```
Take out the calculator do 519a-518c = 14 bytes distance between buffer pointer and uid pointer

And 16 bytes of our buffer and 1 byte of our uid buffer so 16 + 14 +1 =31 bytes to reach 0x000055555555519a and at 32nd bit we writing in * 0x000055555555519a.






## Conclusion
	First malloc stores our password and second stores our uid.
	Our uid can get overwritten easily.
	fgets() stores 160 bytes.
	Our eax/rax must not be a non-Zero meaning uid must be stored as zero to set ZF as 1.

# Actual Hacking
We know that the distance between first and second malloc is of 32 bytes, at 32th byte rax gets overwritten so,  

>fgets() can help us, remember fgets() gets our string as input and checks for the end of line? When it's done checking it adds a null terminator by itself to signify the end (\0).

Lets input the 31 bytes and let fget() null terminator (\0) overwrite 32rd byte

 

Neat…….

# Why did all this happen?
Simply because our fgets() input 160 bytes and malloc is allocating 16 bytes, hence we are able to overflow the buffer and hence causing buffer overflow.


Pretty good, so this was a simple example of buffer overflow. I will return back with more challenges for myself and hopefully this writeup finds it helpful to you all :) 

