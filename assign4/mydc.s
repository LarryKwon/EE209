### --------------------------------------------------------------------
### mydc.s
###
### Desk Calculator (dc)
### --------------------------------------------------------------------

	.equ   ARRAYSIZE, 20
	.equ   EOF, -1
	
.section ".rodata"

scanfFormat:
	.asciz "%s"

printfFormat:
    .asciz "%d\n"

negative_sign:
    .asciz "-"

plus_ch:
    .asciz "+"

empty_msg:
    .asciz "dc:stack empty\n"

### --------------------------------------------------------------------

        .section ".data"

### --------------------------------------------------------------------

        .section ".bss"
buffer:
        .skip  ARRAYSIZE

### --------------------------------------------------------------------

	.section ".text"

	## -------------------------------------------------------------
	## int main(void)
	## Runs desk calculator program.  Returns 0.
	## -------------------------------------------------------------

	.globl  main
	.type   main,@function


main:

	pushl   %ebp
	movl    %esp, %ebp

    jmp middle
    loop:
    // scanf("%s",buffer)
        pushl $buffer
        pushl $scanfFormat
        call scanf
        addl $8, %esp

        //check if user input EOF
        cmp $EOF, %eax
        je quit

        //if(isdigit(buffer[0])==0)
        movl $0, %eax
        movb buffer(%eax), %al

        //if(buffer[0]=='_') goto negative_num
        cmp $0x5F , %al
        je negative_num
        
        pushl %eax
        call isdigit
        addl $4, %esp
        cmp $0, %eax
        //goto else
        je else
        //goto num
        jne num
    
    num:
        pushl $buffer
        call atoi
        addl $4, %esp
        pushl %eax
        jmp loop
        

    negative_num:
        //if(buffer[1]==EOF) goto loop
        movl $1, %eax
        movb buffer(%eax), %al
        cmpb $EOF, %al
        je loop
        // change '_' -> '-'
        movl $0, %eax
        addl $buffer, %eax
        movb $0x2D , (%eax)

        //int n = atoi(buffer)
        pushl $buffer
        call atoi
        addl $4, %esp
        //push(n);
        pushl %eax
        jmp loop

    else:
		//if(buffer[0]!='p') goto exit
		movl $0, %eax
		movb buffer(%eax), %al
		cmpb $0x70, %al
		jne exit
		//if(%esp == %ebp) goto empty
		cmp %esp, %ebp
		je empty
		//printf("%d\n",(int)stack.top())
		movl (%esp), %eax
		pushl %eax
		pushl $printfFormat
		call printf
		addl $8, %esp
		jmp loop
        
    exit:
		//if(buffer[0]!='q') goto plus
		movl $0, %eax
		movb buffer(%eax), %al
		cmpb $0x71, %al
		je quit
		jne plus
    
	plus:
		//if(buffer[0]!='+') goto minus
		movl $0, %eax
		movb buffer(%eax), %al
		cmpb $0x28, %al
		jne minus
		//if(%esp == %ebp) goto empty
		cmp %esp, %ebp
		je empty
		// a = (int)stack.pop()
		popl %edx
		//if(%esp == %ebp) goto error
		cmp %esp, %ebp
		je error
		//b = (int)stack.pop()
		popl %eax
		addl %edx , %eax
		pushl %eax
		movl $0, %edx
		movl $0, %eax
		jmp loop
    
	error:
		//stack.push(a)
		pushl %edx
		movl $0, %edx
		//goto empty
		jmp empty

    minus:
		//if(buffer[0]!='-') goto multiply
		movl $0, %eax
		movb buffer(%eax) , %al
		cmpb $0x2D, %al
		jne multiply

		//int a,b
		//if(%esp == %ebp) goto empty
		cmp %esp, %ebp
		je empty
		//a = (int)stack.pop()
		popl %edx
		//b=(int)stack.pop()
		
		cmp %esp, %ebp
		je error
		popl %eax
		//res = a-b
		subl %edx, %eax
		pusl %eax
		movl $0, %eax
		movl $0, %edx
		jmp loop

    multiply:
		//if(buffer[0]!='*') goto divide
		movl $0, %eax
		movb buffer(%eax) , %al
		cmpb $0x2A, %al
		jne divide

		//if(%esp == %ebp) goto empty
		cmp %esp, %ebp
		je empty

		//a = (int)stack.pop()
		popl %edx
		//b=(int)stack.pop()
		cmp %esp, %ebp
		je error
		popl %eax

		imul %edx
		pushl %eax
		movl $0, %edx
		movl $0, %eax
		jmp loop

    divide:
		//if(buffer[0]!='/') goto remainder
		movl $0, %eax
		movb buffer(%eax) , %al
		cmpb $0x2F, %al
		jne power

		//if(%esp == %ebp) goto empty
		cmp %esp, %ebp
		je empty

		//a = (int)stack.pop()
		popl %edx
		//b=(int)stack.pop()
		cmp %esp, %ebp
		je error
		
		popl %eax
		//res = b/a, quotinent
		movl %edx, %ecx
		cdq
		idivl %ecx
		pushl %eax
		movl $0, %edx
		movl $0, %eax
		jmp loop

    remainder:
		//if(buffer[0]!='/') goto power
		movl $0, %eax
		movb buffer(%eax) , %al
		cmpb $0x25, %al
		jne power

		//if(%esp == %ebp) goto empty
		cmp %esp, %ebp
		je empty		

		//a = (int)stack.pop()
		popl %edx
		//b=(int)stack.pop()
		cmp %esp, %ebp
		je error
		
		popl %eax
		//res = b/a, quotinent
		movl %edx, %ecx
		cdq
		idivl %ecx
		pushl %edx
		movl $0, %edx
		movl $0, %eax
		jmp loop
		
    power:

    op_f:

    op_c:

    op_d:

    op_r:

    empty:

middle:
    jmp loop:

quit:
    movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret

input:

	dc number stack initialized. 
    %esp = %ebp
	
	## scanf("%s", buffer)
	pushl	$buffer
	pushl	$scanfFormat
	call    scanf
	addl    $8, %esp

	## check if user input EOF
	cmp	$EOF, %eax
	je	quit
	
    

    
	## PSEUDO-CODE
	## /*
	##  * In this pseudo-code we are assuming that no local variables are created
	##  * in the _main_ process stack. In case you want to allocate space for local
	##  * variables, please remember to update logic for 'empty dc stack' condition
	##  * (lines 6, 15 and 20) accordingly.
	##  */
	##

    
	##1 while (1) {
	##2	if (scanf("%s", buffer) == EOF)
	##3		return 0;
	##4 	if (!isdigit(buffer[0])) {
	##5		if (buffer[0] == 'p') {
	##6			if (stack.peek() == NULL) { /* is %esp == %ebp? */
	##7				printf("dc: stack empty\n");
	##8			} else {
	##9				printf("%d\n", (int)stack.top()); /* value is already pushed in the stack */
	##10			}
	##11		} else if (buffer[0] == 'q') {
	##12			goto quit;
	##13		} else if (buffer[0] == '+') {
	##14			int a, b;
	##15			if (stack.peek() == NULL) {
	##16				printf("dc: stack empty\n");
	##17				continue;
	##18			}
	##19			a = (int)stack.pop();
	##20			if (stack.peek() == NULL) {
	##21				printf("dc: stack empty\n");
	##22				stack.push(a); /* pushl some register value */
	##23				continue;
	##24			}
	##25			b = (int)stack.pop(); /* popl to some register */
	##26			res = a + b;
	##27 			stack.push(res);
	##28		} else if (buffer[0] == '-') {
	##29			/* ... */
	##30		} else if (buffer[0] == '^') {
	##31			/* ... powerfunc() ... */
	##32		} else if { /* ... and so on ... */
	##33 	} else { /* the first no. is a digit */
	##34		int no = atoi(buffer);
	##35		stack.push(no);	/* pushl some register value */
	##36	}
	##37 }

quit:	
	## return 0
	movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret