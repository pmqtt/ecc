# ecc
Ethan's C Compiler, written in C!

## Status
It can compile arbitrary return expressions that consist of integers. See an example below!

```c
int main() {
    return (2*3)-4 << (1+1) | 2;
}
```

```assembly
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	subq $20, %rsp
	movl $2, -4(%rbp)
	movl -4(%rbp), %r11d
	imull $3, %r11d
	movl %r11d, -4(%rbp)
	movl -4(%rbp), %r10d
	movl %r10d, -8(%rbp)
	movl $4, %r10d
	subl %r10d, -8(%rbp)
	movl $1, -12(%rbp)
	movl $1, %r10d
	addl %r10d, -12(%rbp)
	movl -8(%rbp), %r10d
	movl %r10d, -16(%rbp)
	movb -12(%rbp), %cl
	shll %cl, -16(%rbp)
	movl -16(%rbp), %r10d
	movl %r10d, -20(%rbp)
	movl $2, %r10d
	orl %r10d, -20(%rbp)
	movl -20(%rbp), %eax
	movq %rbp, %rsp
	popq %rbp
	ret

.section .note.GNU-stack,"",@progbits
```