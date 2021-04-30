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
		.equ base, 8
		.equ exp, 12
		
		.equ pow, -16
		.equ index, -20
		//for swap
	.type swap, @function

swap:
	//init
	pushl %ebp
	movl %esp, %ebp

	pushl %ebx
	pushl %esi
	pushl %edi

	// next: edx, top: ecx
	// edx = next address, ecx = top address
	movl 8(%ebp), %edx
	movl 12(%ebp), %ecx

	//get value of top to ebx
	movl (%edx), %ebx
	movl (%ecx), %eax

	movl %ebx, (%ecx)
	movl %eax, (%edx)

	popl %edi
	popl %esi
	popl %ebx
	movl %ebp, %esp
	popl %ebp
	ret

	//for print all elements
	.type print, @function
print:
	//setup
	pushl %ebp
	movl %esp,%ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	//int iIndex = 1
	subl $4, %esp
	movl $5, -16(%ebp)
loop2:
	//%eax = index
	movl -16(%ebp), %eax
	// add address of ebp to eax
	addl %ebp, %eax

	//compare %eax, (%ebp) = old ebp
	cmp %eax, (%ebp)
	//go to loopend2
	je loopend2
	//if not pushl the value of address
	movl (%eax), %eax
	pushl %eax
	pushl $printfFormat
	//call printf
	call printf
	addl $8, %esp
	//increment -16(%ebp) & goto loop
	incl -16(%ebp)
	jmp loop2

loopend2:
	movl -16(%ebp), %edx
	movl -12(%ebp), %edi
	movl -8(%ebp), %esi
	movl -4(%ebp), %ebx
	movl %ebp, %esp
	popl %ebp
	ret

	//for calculating power
	.type ipower,@function
ipower:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %esi
	pushl %edi
	//int iPower = 1
	pushl $1
	//int iIndex
	subl $4, %esp
	movl $1, index(%ebp)
	cmp $0, exp(%ebp)
	jl fraction

loop1:
	//(iIndex > iExp) goto loopend1
	movl index(%ebp), %eax
	cmpl exp(%ebp), %eax
	jg loopend1

	//iPower *= iBase
	movl base(%ebp), %eax
	imull base(%ebp)
	movl %eax, pow(%ebp)

	//iIndex++
	incl index(%ebp)

	//goto loop1
	jmp loop1

fraction:
	movl $0, pow(%ebp)
	jmp loopend1

loopend1:
	//return iPower
	movl pow(%ebp), %eax
	movl -12(%ebp), %edi
	movl -8(%ebp), %esi
	movl -4(%ebp), %ebx
	movl %ebp, %esp
	popl %ebp
	ret

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
		//if(buffer[0]!='/') goto power
		movl $0, %eax
		movb buffer(%eax) , %al
		cmpb $0x5E, %al
		jne op_f

		//if(%esp == %ebp) goto empty
		cmp %esp, %ebp
		je empty

		//a = (int)stack.pop()
		//exponent
		popl %edx

		//if(%esp == %ebp) error
		cmp %esp, %ebp
		je error
		
		//b=(int)stack.pop()
		//base
		pop %ecx

		pushl %edx
		pushl %ecx
		call ipower
		addl $8, %esp
		popl %ecx
		popl %edx
		pushl %eax
		jmp loop

    op_f:
		//if(buffer[0]!='f') goto clear
		movl $0, %eax
		movb buffer(%eax), %al
		cmpb $0x66, %al
		jne op_c
		//setup call print
		pushl %eax
		pushl %ecx
		pushl %edx
		call print
		popl %edx
		popl %ecx
		popl %eax
		jmp loop
    op_c:
		//if(buffer[0]!='c') goto duplicate
		movl $0, %eax
		movb buffer(%eax), %al
		cmpb $0x63, %al
		jne op_d
		movl %ebp, %esp
		jmp loop

    op_d:
		//if(buffer[0]!='d') goto reverse
		movl $0, %eax
		movb buffer(%eax), %al
		cmpb $0x64, %al
		jne op_r
		//if(%esp == %ebp) goto empty
		cmp %esp, %ebp
		je empty
		movl (%esp), %eax
		pushl %eax
		jmp loop

    op_r:
		//if(buffer[0]!='r') goto loop
		movl $0 , %eax
		movb buffer(%eax), %al
		cmp $0x72, %al
		jne loop

		//if(%esp == %ebp) goto empty
		cmp %esp , %ebp 
		je empty		

		popl %edx
		cmp %esp, %ebp
		je error
		pushl %edx
		//top
		movl $0, %ecx
		//next
		movl $4, %edx
		movl $0, %eax
		// move address of the stack to the ecx, edx
		addl %esp, %ecx
		addl %esp, %edx
		// caller has to save
		pushl %eax
		pushl %ecx
		pushl %edx

		//call swap
		pushl %ecx
		pushl %edx
		call swap
		addl $8, %esp
		popl %edx
		popl %ecx
		popl %eax

		//store the top at %eax
		movl (%esp), %eax
		//store the next at %edx
		movl 4(%esp), %edx
		jmp loop

    empty:

middle:
    jmp loop:

quit:
    movl    $0, %eax
	movl    %ebp, %esp
	popl    %ebp
	ret