#COL216 Assignment 2
#push - arguments - a number to be push onto the stack - returns - nothing
#pop - arguments - remove top element - returns - value at the top of the stack


# input : enter your string and press enter.

.data
	postFixExp:		.space	1000		#maximum input size
	Invalid:		.asciiz "Invalid input! \n"
	Stacku:			.asciiz "Invalid postfix! \n"
.text

push:
	move $t9, $a0
	sub $sp, $sp, 4
	sw $t9, 0($sp)
	jr $ra

pop:
	lw $v0, 0($sp)
	addi $sp, $sp, 4
	jr $ra

main:
	# ------ text input --------
	li $v0, 8
	la $a0, postFixExp
	li $a1, 100
	syscall
	#---------------------------

# -----------------------------------------------------------------------------------------------------------------------------------------------#
# Following is algorithm for evaluation postfix expressions:																					 #
# 1) Create a stack to store operands (or values).																								 #
# 2) Scan the given expression and do following for every scanned element.																		 #
# 		a) If the element is a number, push it into the stack																					 #
# 		b) If the element is a operator, pop operands for the operator from stack. Evaluate the operator and push the result back to the stack	 #
# 3) When the expression is ended, the number in the stack is the final answer																	 #
# -----------------------------------------------------------------------------------------------------------------------------------------------#


	move $s1, $sp              #storing the initial pointer of buildin stack 
	move $t0, $a0				#t0 contain the starting point of stored string.
	li $t5, 10
	li $t6, 0 
	loop:
        lbu $t1, ($t0) # load the content of the address stored in $t0  
		beq $t1, $t5, exit		# for termination of loop 
        addi $t0, $t0, 1 # go to next byte   

		blt $t1, 48, not_number							#checks if taken character is a number or not
  		bgt $t1, 57, not_number
		addi $t1, $t1, -48								#changing ascii value to string
		move $a0, $t1
		jal push 
		j loop
		not_number:
			beq		$t1, 42, muln	#if char is * then jump to muln
			beq		$t1, 43, addn  # if char is + then jump to addn
			beq		$t1, 45, subn	# if char is - then jump to subn
			error: #if the input is neither a number nor in (*, +, -)
				li $v0, 4
				la $a0, Invalid
				syscall
				li $v0, 10
				syscall
	
# ---------------------------------------------------------------------------------------------------#
		# popping two number and doing the operation and then push back the resultant number in the stack
		
		muln:
			beq		$sp, $s1, error2	
			jal pop
			move $t7, $v0
			beq		$sp, $s1, error2
			jal pop   
			move $t8, $v0
			mul $t6, $t7, $t8        
			move $a0, $t6  
			jal push
			j loop
		
		addn:
			beq		$sp, $s1, error2	
			jal pop
			move $t7, $v0
			beq		$sp, $s1, error2
			jal pop   
			move $t8, $v0
			add $t6, $t7, $t8        
			move $a0, $t6  
			jal push
			j loop

		subn:
			beq		$sp, $s1, error2
			jal pop
			move $t7, $v0
			beq		$sp, $s1, error2
			jal pop   
			move $t8, $v0
			sub $t6, $t8, $t7        
			move $a0, $t6  
			jal push
        	j loop 

# ---------------------------------------------------------------------------------------------------#

    exit:
		sub $s1, $s1, 4
		bgt		$s1, $sp, error2	# checking if any number is left in stack or not
		
		li $v0, 1
		move $a0, $t6
		syscall
		li $v0, 10
		syscall
	
	error2:
		li $v0, 4
		la $a0, Stacku
		syscall
		li $v0, 10
		syscall