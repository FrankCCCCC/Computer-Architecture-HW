.data
inputA:		.asciiz "input a: "
inputB:		.asciiz "input b: "
inputC:		.asciiz "input c: "
res:		.asciiz "result = "
newline:	.asciiz "\n"
.text

# Scan input a variable
# Text hint
la $a0, inputA
li $v0, 4
syscall
# Ask Input
li $v0, 5
syscall
addi $s0, $v0, 0

# Scan input b variable
# Text hint
la $a0, inputB
li $v0, 4
syscall
# Ask Input
li $v0, 5
syscall
addi $s1, $v0, 0

# Scan input c variable
# Text hint
la $a0, inputC
li $v0, 4
syscall
# Ask Input
li $v0, 5
syscall
addi $s2, $v0, 0

# Call abs_sub function
addi $a0, $s0, 0
addi $a1, $s1, 0
jal Abs_sub
addi $a0, $v1, 0
li $v0, 1
syscall

la $a0, newline
li $v0, 4
syscall

# Call madd function
addi $a0, $s0, 0
addi $a1, $s1, 0
jal Madd
addi $a0, $v1, 0
li $v0, 1
syscall

j Exit

# Function abs_sub(int x, int y); args: $a0, $a1
Abs_sub:
	slt $t0, $a1, $a0
	beq $t0, 1, XLarger
	# $t1 is int large, $t2 is int small
	# $t2 = x < $t1 = y
	addi $t1, $a1, 0
	addi $t2, $a0, 0
	j LargeSubSmall
	
	XLarger:
		# $t1 = x > $t2 = y
		addi $t1, $a0, 0
		addi $t2, $a1, 0
	
	LargeSubSmall:
		# $v1 is large - small
		sub $v1, $t1, $t2
	addi $t0, $zero, 0
	addi $t1, $zero, 0
	addi $t2, $zero, 0
	jr $ra
	
# Function madd(int x, int y); args: $a0, $a1
Madd:
	addi $t3, $zero, 0 # $t3 = int ans = 0
	slt $t0, $a1, $a0 # t0 is bool
	beq $t0, 1, XLarger1
	# $t1 is int large, $t2 is int small
	# $t2 = x < $t1 = y
	addi $t1, $a1, 0
	addi $t2, $a0, 0
	j AddLoop
	
	XLarger1:
		# $t1 = x > $t2 = y
		addi $t1, $a0, 0
		addi $t2, $a1, 0
	
	AddLoop:
		slt $t0, $t1, $t2
		beq $t0, 1, ExitLoop
		Loop:
			add $t3, $t3, $t2
			addi $t1, $t1, -1
			j AddLoop
	ExitLoop:
	addi $v1, $t3, 0
	jr $ra
Exit:
