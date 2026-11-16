.section .text

.extern buffer
.extern buffer_idx

.globl tqv_user_interrupt10_raw
tqv_user_interrupt10_raw:
	# out 4 hi
	li	a0,	16
	sw	a0,	64(tp)

	# copy pcmw
	la	s1,	buffer
	la	a0,	buffer_idx
	lw	a1,	0(a0)
	add	s1,	s1,	a1
	add	s1,	s1,	a1
	addi	a1,	a1,	1
	#andi	a1,	a1,	2047
	sw	a1,	0(a0)
	lw	a0,	0x028c(tp)
	#li	a0,	11111
	sh	a0,	0(s1)

	# out 4 lo
	li	a0,	0
	sw	a0,	64(tp)

	# short_isr_exit
	.2byte	0x3502
	mret
