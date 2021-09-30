#COL216 - Assignment 1
# n - number of points
# Area - function to find the area
# area - final area under the curve


Area:
	move 	$t0, $a0	        #starting index of stack next 2 points is stored
	lw		$t1, 0($t0)			#x1
	lw		$t2, 4($t0)			#y1
	lw		$t3, 8($t0)			#x2
	lw		$t4, 12($t0)		#y2
	sub		$t5, $t1, $t3		
	abs		$t5, $t5			#abs(x1-x2)
	sub		$t6, $t2, $t4		
	abs		$t6, $t6			#abs(y1-y2)
	add		$t7, $t2, $t4
	abs		$t7, $t7			#abs(y1+y2)
	# t6> t7 means that y1 and y2 has different signs and they will intersect x axis which is handled in target:
	bgt		$t6, $t7, target	# if $t6 > $t7 then target
	
	mul 	$t8, $t7, $t5		#2 * area
	mtc1	$t8, $f0			#transfered in float register
	cvt.d.w	$f0, $f0			#converted into double
	div.d	$f0, $f0, $f2		# divded by 2(stored in f2)
	add.d	$f12, $f12, $f0		# added to f12 (which is our output register)
	jr		$ra					# jump to $ra
	





	target:
	# in this branch we calculated the area in two part ($f0 and $f14) with the help of proportionality
	#  formula used :           ((abs(x1-x2)/abs(y1-y2))*y1*y1 + (abs(x1-x2)/abs(y1-y2))*y2*y2
		mtc1	$t2, $f4
		cvt.d.w	$f4, $f4
		mtc1	$t4, $f6
		cvt.d.w	$f6, $f6
		mtc1	$t5, $f8
		cvt.d.w	$f8, $f8
		mtc1	$t6, $f10
		cvt.d.w	$f10, $f10
		div.d   $f0, $f8, $f10
		mul.d 	$f0, $f0, $f4
		mul.d 	$f0, $f0, $f4
		div.d	$f0, $f0, $f2
		div.d   $f14, $f8, $f10
		mul.d 	$f14, $f14, $f6
		mul.d 	$f14, $f14, $f6
		div.d	$f14, $f14, $f2	
		add.d	$f12, $f12, $f0
		add.d	$f12, $f12, $f14
		jr		$ra					
		

	.data
n:	.word 0
result:		.double 0.00          #start value for the result ($f12)
const2:	.double 2.00			#constant value (2) for division
.text

.globl main
.ent main

main:
	#.......... taking n as a input.........
	li $v0, 5 
	syscall
	sw $v0, n
	
	 #.........Runtime heap memory allocation........

	mul $t3, $v0, 8
	move $a0, $t3
	li $v0, 9
	syscall 
	move $s0, $v0


	#.............take input..................
	move $t0, $s0  
	li $t1, 1 #index 
	lw $t2, n 
	mul $t3, $t2, 2  #2*n

	input_points:
		li $v0, 5 #read integer
		syscall 
		sw $v0, 0($t0) #store the integer
		add $t0, $t0, 4    
		
		add $t1,$t1, 1      #increment the index
		ble		$t1, $t3, input_points
		
	#------------------------------------------------------------

	ldc1	$f12, result
	ldc1	$f2, const2
	lw		$s2, n
	blt		$s2, 2, one_point	# if $t0 < $t1 then target
	
	mul     $s2, $s2, 8
	sub		$s2, $s2, 8
	add		$s2, $s2, $s0 
	
	
	 	
loop_area:
	move 	$a0, $s0		
	jal		Area				
	# add		$s1, $s1, $v0		
	addi	$s0, $s0, 8	
	bgt		$s2, $s0, loop_area	
one_point:
	li		$v0, 3 		
	syscall		
	
	

	li $v0, 10
	syscall #Exit


