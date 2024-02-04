  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.global delay, time2string, enable_interrupt
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
   # timetemplate.asm
  # Written 2015 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.data
	.align 2
mytime:	.word 0x5957
timstr:	.ascii "text more text lots of text\0"
	.text
main:
	li	$t9,1 # Add a one in this register s
	# print timstr
	la	$a0,timstr
	li	$v0,4
	syscall
	nop
	# wait a little
	li	$a0,1
	jal	delay
	nop
	# call tick
	la	$a0,mytime
	jal	tick
	nop
	# call your function time2string
	la	$a0,timstr
	la	$t0,mytime
	
	lw	$a1,0($t0)
	jal	time2string
	nop
	# print a newline
	li	$a0,10
	li	$v0,11
	syscall
	nop
	# go back and do it all again
	j	main
	nop
# tick: update time pointed to by $a0

enable_interrupt:
	ei
	jr $ra
	nop
	


tick:	lw	$t0,0($a0)	# get time
	addiu	$t0,$t0,1	# increase
	andi	$t1,$t0,0xf	# check lowest digit
	sltiu	$t2,$t1,0xa	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x6	# adjust lowest digit
	andi	$t1,$t0,0xf0	# check next digit
	sltiu	$t2,$t1,0x60	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa0	# adjust digit
	andi	$t1,$t0,0xf00	# check minute digit
	sltiu	$t2,$t1,0xa00	# if digit < a, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x600	# adjust digit
	andi	$t1,$t0,0xf000	# check last digit
	sltiu	$t2,$t1,0x6000	# if digit < 6, okay
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa000	# adjust last digit
tiend:	sw	$t0,0($a0)	# save updated result
	jr	$ra		# return
	nop

  # you can write your code for subroutine "hexasc" below this line
  #
hexasc:	
	and	$t2, $a0, 0xF
	slti	$t1,$t2,10 	# less than 10? store answer in $t1
	beq	$t1,1, below 	# if yes, go to "below"
	nop
	
	addi	$v0,$t2,55 	# if no, i.e. $a0 >= 10, convert number to hex (ascii)
	jr	$ra		# return
	nop
	
	below:
	addi	$v0,$t2,48	# convert number to ascii
	jr	$ra		# return
	nop
	
delay: 
	#addi $t0, $0, 150000
	slti $t1, $a0, 1 # ms
	beq $t1, 1, end
	nop
	addi $t9, $0, 1
	
	
DELAYLOOP:
	sub $a0, $a0, $t9
	addi $t2, $0, 0
forloop:
	addi $t2, $t2, 1
	bne $t2, 7000, forloop
	nop
	bne $a0, 0, DELAYLOOP
	nop
end:
	jr $ra
	nop	
	
time2string:
	PUSH $ra #store return adress
	PUSH $s0
	PUSH $s1
	PUSH $s2
	add $s0,$a0,0 # store address
	add $s1,$a1,0 # store time

	#add $s0 ,$0, $a1 #store the "time" input in a register we will edit
	
	
	# Before we trim individual characters, we trim everything lsb+4 and above. (we are talking base 8 here, which is why we shift 16, since base 2*4 = 8 and 4*4 = 16)
	and $s2, $s1, 0xFFFF # Trim time and store in S2
#	srl $s2, $s1, 16 #shift right and therefore, removes the 4 lsb from s0 and saves in s1   
#	sll $s2, $s2, 16 #shifts left again, meaning we from the original replaced the 4 LSB with 0's
#	sub $s2, $s1, $s2 #subtract s1 from s0, leaving only the 4 LSB
	#addi $t6, $0, 16  #If Using Delayed Branchinng put this to 12.    
	#addi $t9, $0, 4
	# Counter for the loop, its multiplied by 4 because we are dealing with Bytes and not Bits (If subi x, y. z supports the format subi x, y, 4*z this could be 4 instead)
trimloop:
	#sub $t6, $t6, $t9 #If using Delayed Branching, Put this after the bne
	#srlv  $s1, $s0, $t6
	srl $t0 , $s2, 12
	add $a0, $0, $t0 
	jal hexasc
	nop
	sb $v0, 0($s0)
	sll $t0, $t0, 12
	sub $s2, $s2, $t0
	
	srl $t0, $s2, 8
	add $a0, $0, $t0 
	jal hexasc
	nop
	sb $v0, 1($s0)
	sll $t0, $t0, 8
	sub $s2, $s2, $t0
	srl $t0, $s2, 4
	add $a0, $0, $t0 
	jal hexasc
	nop
	sb $v0, 3($s0)
	sll $t0, $t0, 4
	
	sub $t0, $s2, $t0
	add $a0, $0, $t0 
	jal hexasc
	nop
	sb $v0, 4($s0)
	
	addi $v0, $0, 0x3A
	sb $v0, 2($s0)
	#srlv $t3, $s2, $t6
	#add $t3, $s2, $0
	#PUSH ($s1)
	#sllv  $s1, $s1, $t6
	#sub $s0, $s0, $s1
	#bne $t6, $0, trimloop
	# -- Old Code non looped
	#srl  $s1, $s0, 8 
	#PUSH ($s1)
	#sll  $s1, $s1, 8
	#sub $s0, $s0, $s1
	#srl  $s1, $s0, 4
	#PUSH ($s1)
	#sll  $s1, $s1, 4
	#sub $s0, $s0, $s1	
	#PUSH ($s0)
	# -- end of non looped code
#ny copypaste grej
	
	#copypaste				
	#loop variable
	#addi $t4, $0, 5 #iterations + 1, d� vi smyger in att skriva in kolonen i mitten.  Med Delayed Branching skulle det g�ras lite annorlunda.
	#addi $t5, $t5, 4
time2stringoutput: # g�r att loopa r�tt enkelt, �ndrar det senare.
	#sub $t4, $t4, $t9
	#POP ($a0)
	#jal hexasc
	#nop
	#sb $v0, 0($t5)
	#sub $t5, $t5, $t1 #If using Delayed Branching, Put this after the bne
	#beq $t4, 3, extraloop #t4 = 3 betyder att vi skrivit in de f�rsta 2 tecknen, n�sta �r ":"
	#beq $t4, 0, continue # t4 = 0 betyder att vi skrivit in alla tecken
	#j time2stringoutput   # om inget av ovanst�ende, betyder att vi (i nul�get) skrivit in tecken p� plats 4 eller 1, vi hoppar upp och 
	#nop

extraloop:	
	#sub $t4, $t4, $t9 #If using Delayed Branching, Put this after the bne
	#addi $v0, $0, 0x3A
	#sb $v0, 0($t5)
	#sub $t5, $t5, $t9 #If using Delayed Branching, Put this after the bne
	#j time2stringoutput
	#nop
	
continue:	
	
	
	#POP ($a0) # Get the Address
	
	lb $t1, 4($s0)
	lb $t2, 3($s0)
	beq $t1, 0x30, surprise1
	nop

nosurprise:
	addi $v0, $0, 0x00
	sb $v0 ,5($s0)
	
surprisecontinue:
		#sw $t0, 0($a0) #store the word to the adress
	
	
	#print test
	
	# print timstr

	#li	$v0,4
	#syscall
	#nop
	POP $s2
	POP $s1
	POP $s0
	POP $ra #Fetch Return Address
	jr	$ra
	nop
	#t0 �r nu ett fullt ord (8byte) med ascii koden f�r tiden. Ska dock sparas med en kolon och null, total 12 Byte.
	
surprise1:
	beq $t2, 0x30, surprise2
	nop
	j nosurprise
	nop


surprise2:
	addi $t1,$0,88
	sb $t1 ,5($s0)
	addi $v0, $0, 0x00
	sb $v0 ,6($s0)
	j surprisecontinue
	nop
	
	
#	add $t7, $0, 4  # for looping test
	# den h�r funktionen �r nu trasig d� ovanst�ende redan poppar stacken.
#printloop:  #Testing purpsoses only, Remove this and make a loop that instead stores at the address initially stored in $a0
#	POP ($a0)
#	jal hexasc
#	add $a0, $0, $v0
#	li	$v0,11	
#	syscall
#	sub $t7, $t7, 1
#	bne $t7, $0, printloop
#	nop
	
