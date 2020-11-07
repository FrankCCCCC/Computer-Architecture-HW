.data

descript:	.asciiz "Please select an integer number A from (0~10): "
compute:	.asciiz "A * 2 = "
stars:		.asciiz "******"
end:		.asciiz "THE END\n"
newline:	.asciiz "\n"
skip: 		.asciiz "skip\n"

.text

# Input = $t0

_Start:
	addi $t0, $zero, 0
	
	# Print Descript
	la $a0, descript
	li $v0, 4
	syscall
	
	# Scan input integer
	li $v0, 5
	syscall
	addi $t0, $v0, 0
	
	# if input == 0
	beq $t0, 0, _Exit
	
	#if input <0
	slt $t1, $t0, $zero
	beq $t1, 1, _Skip
	
	#if input > 10
	slti $t1, $t0, 11
	beq $t1, 0, _Skip
	
	# if input == 7
	beq $t0, 7, _Compute
	
	# if input <= 10 && input > 0 && input != 7
	j _Stars
	
	# Callee Block if input > 10 || input < 0
	_Skip:
	#la $a0, skip
	#li $v0, 4
	#syscall
	j _Start
	
	# Callee Block if input == 7
	_Compute:
	la $a0, compute
	li $v0, 4
	syscall
	add $t0, $t0, $t0
	addi $a0, $t0, 0
	li $v0, 1
	syscall
	la $a0, newline
	li $v0, 4
	syscall
	j _Start
	
	# Callee Block if input <= 10 && input >= 0 && input != 7
	_Stars:
	# $t4 = i in _Loop
	# $t5 = bool in _Loop
	addi $t4, $zero, 0
		_Loop:
		la $a0, stars
		li $v0, 4
		syscall
		move $a0, $t4
		li $v0, 1
		syscall
		la $a0, stars
		li $v0, 4
		syscall
		la $a0, newline
		li $v0, 4
		syscall
		addi $t4, $t4, 1
		slt $t5, $t4, $t0
		beq $t5, 1, _Loop
	j _Start
	
_Exit:
	# Print "The End"
	la $a0, end
	li $v0, 4
	syscall