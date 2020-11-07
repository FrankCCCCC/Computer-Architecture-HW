.data
str1:		.asciiz "Please select an integer number A from (0~10): "
str2:		.asciiz "A * 2 = "
str3:		.asciiz "******"
str4:		.asciiz "THE END"
Pnewline: 	.asciiz "\n"
.text

_Start:
	# Prompt the user to enter int
	
	           
	
	
	# Get the int and store in $t0
	
  	
  	
  	
  	#if input = 0, goto _Exit
  	
  	
  	# If $t0 < 0 || $t0 > 10, goto _OutofRange
  	
  	
  	
  	
  	
  	
  	
  	#If input =7, compute input * 2, and print out the answer.
	
	
	            
	
	
  	
  	
  	 
		
		
	
  	#After print out, go back to wait the user to enter number.
  	
  	
_function_others:
	#setup counter i
	
	 
	#Write a Loop for print ******
	Loop:
	
	#Chech the input and the counter
	
	
	#End the loop and wait the user to enter number.
	

	# print out ******
	
	
	 
	
	#print out the counter number
	             
	 
	
	
	# print out ******
	             
	
	 
	
	# print out \n (´«¦æ)
	
		
		
	
	#counter ++
	
	
	#go back to the Loop
	j Loop                

#Stop the program
_Exit:
	#print out THE END
	
