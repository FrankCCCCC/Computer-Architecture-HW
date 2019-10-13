.data
inputA:		.asciiz "input a: "
inputB:		.asciiz "input b: "
ans:		.asciiz "ans: "
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

addi $a0, $s0, 0
addi $a1, $s1, 0
jal Fn
la $a0, ans
li $v0, 4
syscall
addi $a0, $v1, 0
li $v0, 1
syscall

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
beq $t0, 1, Cond0

slt $t0, $zero, $a1
beq $t0, 1, Cond1

slt $0, $a1, $a0
beq $t0, 1, Cond2

j Cond3

Cond0:
	addi $v1, $zero, 0
	j Return

Cond1:
	addi $v1, $zero, 0
	j Return

Cond2:
	addi $v1, $zero, 2
	j Return

Cond3:
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
addi $t1, $zero, 2


Exit:
