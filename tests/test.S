#include "screen.h"
#include "system.h"
.global main

.data
lo: .word 0
hi: .word 0
.text

main:
	li $a0, BRIGHT_WHITE
	li $a1, BLACK
	jal set_color
	jal clear_screen
	addi $sp, $sp, -52
	sw $ra, ($sp)
	sw $zero, 4($sp)


	sw $zero, 48($sp)

	lw $t0, MS_COUNTER_REG_ADDR
	add $a0, $t0, $zero
	jal rand_seed

	li $t0, 75 
	li $t1, 0 
	sll $t1, $t1, 2
	la $t2, 8($sp)
	add $t2, $t2, $t1
	sw $t0, ($t2)

	li $t0, 30 
	li $t1, 1 
	sll $t1, $t1, 2
	la $t2, 8($sp)
	add $t2, $t2, $t1
	sw $t0, ($t2)

	li $t0, 420 
	li $t1, 2 
	sll $t1, $t1, 2
	la $t2, 8($sp)
	add $t2, $t2, $t1
	sw $t0, ($t2)

	jal rand
	li $t1, 3 
	sll $t1, $t1, 2
	la $t0, 8($sp)
	add $t0, $t0, $t1
	sw $v0, ($t0)

	jal rand
	li $t1, 4 
	sll $t1, $t1, 2
	la $t0, 8($sp)
	add $t0, $t0, $t1
	sw $v0, ($t0)

	jal rand
	li $t1, 5 
	sll $t1, $t1, 2
	la $t0, 8($sp)
	add $t0, $t0, $t1
	sw $v0, ($t0)

	jal rand
	li $t1, 6 
	sll $t1, $t1, 2
	la $t0, 8($sp)
	add $t0, $t0, $t1
	sw $v0, ($t0)

	jal rand
	li $t1, 7 
	sll $t1, $t1, 2
	la $t0, 8($sp)
	add $t0, $t0, $t1
	sw $v0, ($t0)

	li $t0, 2 
	li $t1, 8 
	sll $t1, $t1, 2
	la $t2, 8($sp)
	add $t2, $t2, $t1
	sw $t0, ($t2)

	li $t0, 45 
	li $t1, 9 
	sll $t1, $t1, 2
	la $t2, 8($sp)
	add $t2, $t2, $t1
	sw $t0, ($t2)

	lw $t0, 48($sp) 
	li $t1 ,10
	sw $t1, 48($sp)

	li $a0, 'c' 
	jal put_char
	li $a0, 'o' 
	jal put_char
	li $a0, 'u' 
	jal put_char
	li $a0, 'n' 
	jal put_char
	li $a0, 't' 
	jal put_char
	li $a0, ':' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	lw $t0, 48($sp) 
	move $a0, $t0
	jal put_udecimal
	li $a0, '\n' 
	jal put_char

	lw $t0, 4($sp) 
	li $t1, 0 
	sw $t1, 4($sp)
for_0:
	lw $t1, 4($sp) 
	li $t2, 10 
	slt $t1, $t1, $t2
	beqz $t1, endfor_0
	lw $t3, 48($sp) 
	lw $t4, 4($sp) 
	sll $t4, $t4, 2
	la $t5, 8($sp)
	add $t5, $t5, $t4
	lw $t5, ($t5) 
	add $t3, $t3, $t5
	sw $t3, 48($sp)

	li $a0, 'L' 
	jal put_char
	li $a0, 'o' 
	jal put_char
	li $a0, 'c' 
	jal put_char
	li $a0, 'a' 
	jal put_char
	li $a0, 'l' 
	jal put_char
	li $a0, '[' 
	jal put_char
	lw $t3, 4($sp) 
	move $a0, $t3
	jal put_udecimal
	li $a0, ']' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, '=' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	lw $t3, 4($sp) 
	sll $t3, $t3, 2
	la $t4, 8($sp)
	add $t4, $t4, $t3
	lw $t4, ($t4) 
	move $a0, $t4
	jal put_udecimal
	li $a0, '\n' 
	jal put_char

	lw $t2, 4($sp) 
	addi $t2, $t2, 1
	sw $t2, 4($sp)
	j for_0
endfor_0:

	li $a0, 'C' 
	jal put_char
	li $a0, 'o' 
	jal put_char
	li $a0, 'u' 
	jal put_char
	li $a0, 'n' 
	jal put_char
	li $a0, 't' 
	jal put_char
	li $a0, ':' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	lw $t0, 48($sp) 
	move $a0, $t0
	jal put_udecimal
	li $a0, ' ' 
	jal put_char
	li $a0, 'A' 
	jal put_char
	li $a0, 'v' 
	jal put_char
	li $a0, 'e' 
	jal put_char
	li $a0, 'r' 
	jal put_char
	li $a0, 'a' 
	jal put_char
	li $a0, 'g' 
	jal put_char
	li $a0, 'e' 
	jal put_char
	li $a0, ':' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	lw $t0, 48($sp) 
	li $t1, 10 
	move $a0, $t0
	move $a1, $t1
	la $a2, lo
	la $a3, hi
	jal divide
	lw $t0, lo
	move $a0, $t0
	jal put_udecimal

	addi $sp, $sp, 52
	jr $ra

