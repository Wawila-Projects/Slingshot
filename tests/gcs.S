#include "screen.h"
#include "system.h"
.global main

.data
lo: .word 0
hi: .word 0
a: .word 0
b: .word 0
.text

main:
	li $a0, BRIGHT_WHITE
	li $a1, BLACK
	jal set_color
	jal clear_screen
	addi $sp, $sp, -16
	sw $ra, ($sp)
	sw $zero, 4($sp)
	sw $zero, 8($sp)
	sw $zero, 12($sp)

	lw $t0, a 
	li $t1, 28 
	sw $t1, a

	lw $t0, b 
	li $t1, 21 
	sw $t1, b

	lw $t0, 4($sp) 
	lw $t1, a 
	sw $t1, 4($sp)

	lw $t0, 8($sp) 
	lw $t1, b 
	sw $t1, 8($sp)

	lw $t0, 12($sp) 
	lw $t1, 4($sp) 
	move $a0, $t1
	lw $t1, 8($sp) 
	move $a1, $t1
	jal gcd
	sw $v0, 12($sp)

	li $a0, 'G' 
	jal put_char
	li $a0, 'C' 
	jal put_char
	li $a0, 'D' 
	jal put_char
	li $a0, '(' 
	jal put_char
	lw $t0, a 
	move $a0, $t0
	jal put_udecimal
	li $a0, ',' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	lw $t0, b 
	move $a0, $t0
	jal put_udecimal
	li $a0, ')' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	li $a0, '=' 
	jal put_char
	li $a0, ' ' 
	jal put_char
	lw $t0, 12($sp) 
	move $a0, $t0
	jal put_udecimal
	li $a0, '\n' 
	jal put_char

	addi $sp, $sp, 16
	jr $ra
gcd:
	addi $sp, $sp, -12
	sw $ra, ($sp)
	lw $t0, 8($sp) 
	li $t1, 0 
	seq $t0, $t0, $t1
	beqz $t0, falseif_0
	lw $t1, 4($sp) 
	lw $v0, 4($sp)
	move $v0, $t1

	j endif_0
falseif_0:
	lw $t1, 4($sp) 
	lw $t2, 8($sp) 
	move $a0, $t1
	move $a1, $t2
	la $a2, lo
	la $a3, hi
	jal divide
	lw $t1, hi
	move $a1, $t1
	lw $t1, 8($sp) 
	move $a0, $t1
	jal gcd
	move $v0, $v0

endif_0:

	lw $ra, ($sp)
	addi $sp, $sp, 12
	jr $ra

