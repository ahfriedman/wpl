	.text
	.file	"WPLC.ll"
	.globl	program                         # -- Begin function program
	.p2align	4, 0x90
	.type	program,@function
program:                                # @program
	.cfi_startproc
# %bb.0:                                # %entry
	subq	$40, %rsp
	.cfi_def_cfa_offset 48
	movl	$10, 8(%rsp)
	movl	$10, 4(%rsp)
	movl	$10, 12(%rsp)
	movl	$.L__unnamed_1, %edi
	movl	$1, %esi
	xorl	%edx, %edx
	movl	$1, %ecx
	xorl	%eax, %eax
	callq	printf@PLT
	movl	8(%rsp), %esi
	movl	4(%rsp), %edx
	movl	12(%rsp), %ecx
	movl	$.L__unnamed_2, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$95, 8(%rsp)
	movl	$.L__unnamed_3, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	8(%rsp), %esi
	movl	4(%rsp), %edx
	movl	12(%rsp), %ecx
	movl	$.L__unnamed_4, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$-185, 4(%rsp)
	movl	$.L__unnamed_5, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	8(%rsp), %esi
	movl	4(%rsp), %edx
	movl	12(%rsp), %ecx
	movl	$.L__unnamed_6, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	8(%rsp), %esi
	movl	4(%rsp), %ecx
	movl	%esi, %eax
	cltd
	idivl	%ecx
	movl	%eax, 12(%rsp)
	movl	$.L__unnamed_7, %edi
	movl	%ecx, %edx
	xorl	%eax, %eax
	callq	printf@PLT
	movl	8(%rsp), %esi
	movl	4(%rsp), %edx
	movl	12(%rsp), %ecx
	movl	$.L__unnamed_8, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	8(%rsp), %eax
	xorl	%esi, %esi
	xorl	%edx, %edx
	cmpl	4(%rsp), %eax
	sete	%sil
	setne	%dl
	xorl	%ecx, %ecx
	cmpl	12(%rsp), %eax
	sete	%cl
	movl	$.L__unnamed_9, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movb	$1, 3(%rsp)
	movb	$1, 1(%rsp)
	movb	$0, 2(%rsp)
	movl	$.L__unnamed_10, %edi
	movl	$1, %esi
	movl	$1, %edx
	xorl	%ecx, %ecx
	xorl	%eax, %eax
	callq	printf@PLT
	movb	3(%rsp), %al
	movb	1(%rsp), %cl
	xorb	%al, %cl
	movzbl	%cl, %edx
	notb	%cl
	movzbl	%cl, %esi
	andl	$1, %esi
	andl	$1, %edx
	xorb	2(%rsp), %al
	notb	%al
	movzbl	%al, %ecx
	andl	$1, %ecx
	movl	$.L__unnamed_11, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movb	1(%rsp), %al
	orb	2(%rsp), %al
	notb	%al
	andb	$1, %al
	movb	%al, 3(%rsp)
	movl	$.L__unnamed_12, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movzbl	2(%rsp), %ecx
	movzbl	1(%rsp), %edx
	movzbl	3(%rsp), %esi
	movl	$.L__unnamed_13, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movb	1(%rsp), %al
	orb	2(%rsp), %al
	andb	$1, %al
	movb	%al, 1(%rsp)
	movl	$.L__unnamed_14, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movzbl	2(%rsp), %ecx
	movzbl	1(%rsp), %edx
	movzbl	3(%rsp), %esi
	movl	$.L__unnamed_15, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movb	3(%rsp), %al
	notb	%al
	movb	1(%rsp), %cl
	notb	%cl
	andb	%al, %cl
	andb	$1, %cl
	movb	%cl, 2(%rsp)
	movl	$.L__unnamed_16, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movzbl	2(%rsp), %ecx
	movzbl	1(%rsp), %edx
	movzbl	3(%rsp), %esi
	movl	$.L__unnamed_17, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movb	3(%rsp), %al
	movb	1(%rsp), %cl
	xorb	%al, %cl
	movzbl	%cl, %edx
	notb	%cl
	movzbl	%cl, %esi
	andl	$1, %esi
	andl	$1, %edx
	xorb	2(%rsp), %al
	notb	%al
	movzbl	%al, %ecx
	andl	$1, %ecx
	movl	$.L__unnamed_18, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	$.L__unnamed_19, 32(%rsp)
	movq	$.L__unnamed_19, 24(%rsp)
	movq	$.L__unnamed_20, 16(%rsp)
	movl	$.L__unnamed_21, %edi
	movl	$.L__unnamed_19, %esi
	movl	$.L__unnamed_19, %edx
	movl	$.L__unnamed_20, %ecx
	xorl	%eax, %eax
	callq	printf@PLT
	movq	32(%rsp), %rax
	xorl	%esi, %esi
	xorl	%edx, %edx
	cmpq	24(%rsp), %rax
	sete	%sil
	setne	%dl
	xorl	%ecx, %ecx
	cmpq	16(%rsp), %rax
	sete	%cl
	movl	$.L__unnamed_22, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	$.L__unnamed_23, 32(%rsp)
	movq	24(%rsp), %rdx
	movq	16(%rsp), %rcx
	movl	$.L__unnamed_24, %edi
	movl	$.L__unnamed_23, %esi
	xorl	%eax, %eax
	callq	printf@PLT
	movq	32(%rsp), %rax
	xorl	%esi, %esi
	xorl	%edx, %edx
	cmpq	24(%rsp), %rax
	sete	%sil
	setne	%dl
	xorl	%ecx, %ecx
	cmpq	16(%rsp), %rax
	sete	%cl
	movl	$.L__unnamed_25, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	8(%rsp), %esi
	movl	4(%rsp), %edx
	movl	12(%rsp), %ecx
	movl	$.L__unnamed_26, %edi
	xorl	%eax, %eax
	callq	printf@PLT
	movl	$-1, %eax
	addq	$40, %rsp
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end0:
	.size	program, .Lfunc_end0-program
	.cfi_endproc
                                        # -- End function
	.globl	main                            # -- Begin function main
	.p2align	4, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# %bb.0:                                # %entry
	pushq	%rax
	.cfi_def_cfa_offset 16
	callq	program@PLT
	popq	%rcx
	.cfi_def_cfa_offset 8
	retq
.Lfunc_end1:
	.size	main, .Lfunc_end1-main
	.cfi_endproc
                                        # -- End function
	.type	.L__unnamed_1,@object           # @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"Int equality test I: %d, %d, %d\n"
	.size	.L__unnamed_1, 33

	.type	.L__unnamed_2,@object           # @1
.L__unnamed_2:
	.asciz	"a = %d, b = %d, c = %d\n"
	.size	.L__unnamed_2, 24

	.type	.L__unnamed_3,@object           # @2
.L__unnamed_3:
	.asciz	"a <- 95\n"
	.size	.L__unnamed_3, 9

	.type	.L__unnamed_4,@object           # @3
.L__unnamed_4:
	.asciz	"a = %d, b = %d, c = %d\n"
	.size	.L__unnamed_4, 24

	.type	.L__unnamed_5,@object           # @4
.L__unnamed_5:
	.asciz	"b <- -37 * (12-2) = -370\n"
	.size	.L__unnamed_5, 26

	.type	.L__unnamed_6,@object           # @5
.L__unnamed_6:
	.asciz	"a = %d, b = %d, c = %d\n"
	.size	.L__unnamed_6, 24

	.type	.L__unnamed_7,@object           # @6
.L__unnamed_7:
	.asciz	"c <- %d/%d\n"
	.size	.L__unnamed_7, 12

	.type	.L__unnamed_8,@object           # @7
.L__unnamed_8:
	.asciz	"a = %d, b = %d, c = %d\n"
	.size	.L__unnamed_8, 24

	.type	.L__unnamed_9,@object           # @8
.L__unnamed_9:
	.asciz	"Int equality test II: %d, %d, %d\n"
	.size	.L__unnamed_9, 34

	.type	.L__unnamed_10,@object          # @9
.L__unnamed_10:
	.asciz	"a = %d, b = %d, c = %d\n"
	.size	.L__unnamed_10, 24

	.type	.L__unnamed_11,@object          # @10
.L__unnamed_11:
	.asciz	"bool equality test I: %d, %d, %d\n"
	.size	.L__unnamed_11, 34

	.type	.L__unnamed_12,@object          # @11
.L__unnamed_12:
	.asciz	"a <- ~(b | c)\n"
	.size	.L__unnamed_12, 15

	.type	.L__unnamed_13,@object          # @12
.L__unnamed_13:
	.asciz	"a = %d, b = %d, c = %d\n"
	.size	.L__unnamed_13, 24

	.type	.L__unnamed_14,@object          # @13
.L__unnamed_14:
	.asciz	"b <- b | c\n"
	.size	.L__unnamed_14, 12

	.type	.L__unnamed_15,@object          # @14
.L__unnamed_15:
	.asciz	"a = %d, b = %d, c = %d\n"
	.size	.L__unnamed_15, 24

	.type	.L__unnamed_16,@object          # @15
.L__unnamed_16:
	.asciz	"c <- ~a & ~b\n"
	.size	.L__unnamed_16, 14

	.type	.L__unnamed_17,@object          # @16
.L__unnamed_17:
	.asciz	"a = %d, b = %d, c = %d\n"
	.size	.L__unnamed_17, 24

	.type	.L__unnamed_18,@object          # @17
.L__unnamed_18:
	.asciz	"bool equality test II: %d, %d, %d\n"
	.size	.L__unnamed_18, 35

	.type	.L__unnamed_19,@object          # @18
.L__unnamed_19:
	.asciz	"String One"
	.size	.L__unnamed_19, 11

	.type	.L__unnamed_20,@object          # @19
.L__unnamed_20:
	.asciz	"String Two"
	.size	.L__unnamed_20, 11

	.type	.L__unnamed_21,@object          # @20
.L__unnamed_21:
	.asciz	"a = %s, b = %s, c = %s\n"
	.size	.L__unnamed_21, 24

	.type	.L__unnamed_22,@object          # @21
.L__unnamed_22:
	.asciz	"str equality test I: %d, %d, %d\n"
	.size	.L__unnamed_22, 33

	.type	.L__unnamed_23,@object          # @22
.L__unnamed_23:
	.asciz	"String 3 now"
	.size	.L__unnamed_23, 13

	.type	.L__unnamed_24,@object          # @23
.L__unnamed_24:
	.asciz	"a = %s, b = %s, c = %s\n"
	.size	.L__unnamed_24, 24

	.type	.L__unnamed_25,@object          # @24
.L__unnamed_25:
	.asciz	"str equality test II: %d, %d, %d\n"
	.size	.L__unnamed_25, 34

	.type	.L__unnamed_26,@object          # @25
.L__unnamed_26:
	.asciz	"Ints still are: a = %d, b = %d, c = %d\n"
	.size	.L__unnamed_26, 40

	.section	".note.GNU-stack","",@progbits
