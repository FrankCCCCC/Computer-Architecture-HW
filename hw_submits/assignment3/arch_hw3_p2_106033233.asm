.data
inputA:		.asciiz "input a: "
inputB:		.asciiz "input b: "
ans:		.asciiz "ans: "
newline:	.asciiz "\n"
cond0:		.asciiz "cond0 x <= 0"
cond1:		.asciiz "cond1 y <= 0"
cond2:		.asciiz "cond2 x > y"
cond3:		.asciiz "cond3 else fn recursive"
testCond0:	.asciiz "TestCond0 "
testCond1:	.asciiz "TestCond1 "
sep:		.asciiz ":"
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

# Test Fn functiom
# addi $a0, $s0, 0
# addi $a1, $s1, 0
# jal Fn
# la $a0, ans
# li $v0, 4
# syscall
# addi $a0, $v1, 0
# li $v0, 1
# syscall

# Newline
# la $a0, newline
# li $v0, 4
# syscall

# Test re functiom
# addi $a0, $s0, 0
# jal Re
# la $a0, ans
# li $v0, 4
# syscall
# addi $a0, $v1, 0
# li $v0, 1
# syscall

addi $sp, $sp, -16
sw $s3, 12($sp)
sw $s2, 8($sp)
sw $s1, 4($sp)
sw $s0, 0($sp)

addi $a0, $s0, 0
jal Re # Call c = re(a)
lw $s3, 12($sp)
lw $s2, 8($sp)
lw $s1, 4($sp)
lw $s0, 0($sp)
addi $s2, $v1, 0
sw $s2, 8($sp)

la $a0, ans
li $v0, 4
syscall # Print("ans: ")
addi $a0, $s2, 0
li $v0, 1
syscall  # Print(d)

addi $a0, $s1, 0
addi $a1, $s2, 0
jal Fn # Call d = Fn(b, c)
lw $s3, 12($sp)
lw $s2, 8($sp)
lw $s1, 4($sp)
lw $s0, 0($sp)
addi $s3, $v1, 0
sw $s3, 12($sp)

la $a0, ans
li $v0, 4
syscall # Print("ans: ")
addi $a0, $s3, 0
li $v0, 1
syscall # Print(d)

j Exit

Fn:
# int x is $a0 , int y is $a1
addi $sp, $sp, -24 # Create Stack
sw $s2, 20($sp) # Third Call fn recursive
sw $s1, 16($sp) # Second Call fn recursive
sw $s0, 12($sp) # First Call fn recursive
sw $a1, 8($sp) #Save int y, $a1
sw $a0, 4($sp) #Save int x, $a0
sw $ra, 0($sp) #Save return address, $ra

slt $t0, $zero, $a0
beq $t0, 0, Cond0

slt $t0, $zero, $a1
beq $t0, 0, Cond1

slt $t0, $a1, $a0
beq $t0, 1, Cond2

j Cond3

Cond0:
	addi $v1, $zero, 0
#	la $a0, cond0
#	li $v0, 4
#	syscall
	j Return

Cond1:
	addi $v1, $zero, 0
#	la $a0, cond1
#	li $v0, 4
#	syscall
	j Return

Cond2:
	addi $v1, $zero, 2
#	la $a0, cond2
#	li $v0, 4
#	syscall
	j Return

Cond3:
#	la $a0, cond3
#	li $v0, 4
#	syscall
# 3 * fn(x - 1, y)
addi $a0, $a0, -1
jal Fn
lw $s2, 20($sp) # Third Call fn recursive
lw $s1, 16($sp) # Second Call fn recursive
lw $s0, 12($sp) # First Call fn recursive
lw $a1, 8($sp) #Save int y, $a1
lw $a0, 4($sp) #Save int x, $a0
lw $ra, 0($sp) #Save return address, $ra
addi $s0, $v1, 0
mul $s0, $s0, 3
sw $s0, 12($sp)

# 2 * fn(x, y - 1)
addi $a1, $a1, -1
jal Fn
lw $s2, 20($sp) # Third Call fn recursive
lw $s1, 16($sp) # Second Call fn recursive
lw $s0, 12($sp) # First Call fn recursive
lw $a1, 8($sp) #Save int y, $a1
lw $a0, 4($sp) #Save int x, $a0
lw $ra, 0($sp) #Save return address, $ra
addi $s1, $v1, 0
mul $s1, $s1, 2
sw $s1, 16($sp)

# fn(x - 1, y - 1)
addi $a0, $a0, -1
addi $a1, $a1, -1
jal Fn
lw $s2, 20($sp) # Third Call fn recursive
lw $s1, 16($sp) # Second Call fn recursive
lw $s0, 12($sp) # First Call fn recursive
lw $a1, 8($sp) #Save int y, $a1
lw $a0, 4($sp) #Save int x, $a0
lw $ra, 0($sp) #Save return address, $ra
addi $s2, $v1, 0
sw $s2, 20($sp)

add $v1, $s0, $s1
add $v1, $v1, $s2

Return:
addi $sp, $sp, 24 # Delete Stack
jr $ra

Re:
	addi $sp, $sp, -16
	sw $s1, 12($sp) # Save $s1, as Re(x - 2)
	sw $s0, 8($sp) # Save $s0, as Re(x - 1)
	sw $a0, 4($sp) # Save $a0 as int x
	sw $ra, 0($sp) # Save $ra Return Address
	addi $t1, $zero, 2
	slt $t0, $a0, $t1
	beq $t0, 1, re_cond1
	
	re_cond0:
		# addi $t3, $a0, 0
		# Test
		# la $a0, testCond0
		# li $v0, 4
		# syscall
		# lw $a0, 4($sp)
		
		# Calculate Re(n-1)
		# addi $a0, $t3, 0
		addi $a0, $a0, -1
		
		# Print Int
		# li $v0, 1
		# syscall
		
		jal Re
		lw $s1, 12($sp)
		lw $s0, 8($sp)
		lw $a0, 4($sp)
		lw $ra, 0($sp)
		addi $s0, $v1, 0
		sw $s0, 8($sp)
		
		# Print Res
		# addi $t3, $a0, 0
		# la $a0, sep
		# li $v0, 4
		# syscall
		# addi $a0, $v1, 0
		# li $v0, 1
		# syscall
		# addi $a0, $t3, 0
		#####################
		# addi $t4, $a0, 0
		# Test
		# la $a0, testCond1
		# li $v0, 4
		# syscall
		
		# Calculate Re(n-2)
		# addi $a0, $t4, 0
		addi $a0, $a0, -2
		
		# Print Int
		# li $v0, 1
		# syscall
		
		jal Re
		# Print Res
		# addi $t3, $a0, 0
		# la $a0, sep
		# li $v0, 4
		# syscall
		# addi $a0, $v1, 0
		# li $v0, 1
		# syscall
		# addi $a0, $t3, 0 # ---
		lw $s1, 12($sp)
		lw $s0, 8($sp)
		lw $a0, 4($sp)
		lw $ra, 0($sp)
		addi $s1, $v1, 0
		sw $s1, 12($sp)
		
		lw $s0, 8($sp)
		lw $s1, 12($sp)
		mul $t1, $a0, $a0 # $t1 = x * x
		mul $t2, $a0, $s0 # $t2 = x * re(x - 1)
		add $t3, $a0, -1  # $t3 = (x - 1)
		mul $t3, $t3, $s1 # $t3 = (x - 1) * re(x - 2)
		
		add $v1, $t1, $t2
		add $v1, $v1, $t3
		
		j re_return
		
	re_cond1:
		bne $a0, 1, re_cond11
			re_cond10:
				addi $v1, $zero, 1
				j re_return
			re_cond11:
				addi $v1, $zero, 0
				j re_return
	re_return:
	lw $s1, 12($sp) # Save $s1, as Re(x - 2)
	lw $s0, 8($sp) # Save $s0, as Re(x - 1)
	lw $a0, 4($sp) # Save $a0 as int x
	sw $ra, 0($sp) # Save $ra Return Address
	addi $sp, $sp, 16
	jr $ra
Exit:
	lw $s3, 12($sp)
	lw $s2, 8($sp)
	lw $s1, 4($sp)
	lw $s0, 0($sp)
	addi $sp, $sp, 16
