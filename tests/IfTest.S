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
	addi $sp, $sp, -12
	sw $ra, ($sp)
	sw $zero, 4($sp)
	sw $zero, 8($sp)

	lw $t0, MS_COUNTER_REG_ADDR
	add $a0, $t0, $zero
	jal rand_seed

	lw $t0, 4($sp) 
	jal rand
	li $t1, 100 
	move $a0, $v0
	move $a1, $t1
	la $a2, lo
	la $a3, hi
	jal divide
	lw $t0, hi
	addi $t0, $t0, 1
	sw $t0, 4($sp)

	lw $t0, 8($sp) 
	jal rand
	li $t1, 100 
	move $a0, $v0
	move $a1, $t1
	la $a2, lo
	la $a3, hi
	jal divide
	lw $t0, hi
	addi $t0, $t0, 1
	sw $t0, 8($sp)

	li $a0, 'X' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, '=' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	lw $t0, 4($sp) 
	move $a0, $t0
	jal put_udecimal
	li $a0, ',' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 'Y' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, '=' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	lw $t0, 8($sp) 
	move $a0, $t0
	jal put_udecimal
	li $a0, '\n' 
	jal put_char

	lw $t0, 4($sp) 
	lw $t1, 8($sp) 
	seq $t0, $t0, $t1
	beqz $t0, falseif_0
	li $a0, 'X' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 'i' 
	jal put_char
	li $a0, 's' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 'e' 
	jal put_char
	li $a0, 'q' 
	jal put_char
	li $a0, 'u' 
	jal put_char
	li $a0, 'a' 
	jal put_char
	li $a0, 'l' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 't' 
	jal put_char
	li $a0, 'o' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 'Y' 
	jal put_char
	li $a0, '\n' 
	jal put_char
	j endif_0
falseif_0:
	li $a0, 'X' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 'i' 
	jal put_char
	li $a0, 's' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 'n' 
	jal put_char
	li $a0, 'o' 
	jal put_char
	li $a0, 't' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 'e' 
	jal put_char
	li $a0, 'q' 
	jal put_char
	li $a0, 'u' 
	jal put_char
	li $a0, 'a' 
	jal put_char
	li $a0, 'l' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 't' 
	jal put_char
	li $a0, 'o' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, 'Y' 
	jal put_char
	li $a0, '\n' 
	jal put_char
endif_0:

	addi $sp, $sp, 12
	jr $ra

