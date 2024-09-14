	.text
	.file	"extload.ll"
	.global	extload                         # -- Begin function extload
	.p2align	2
	.type	extload,@function
extload:                                # @extload
	.cfi_startproc
# %bb.0:                                # %entry
	l.addi r4, r1, -4
	l.or r1, r4, r4
	.cfi_def_cfa_offset 4
	l.sb 1(r1), r3
	l.sh 2(r1), r3
	l.movhi r11, hi(0)
	l.addi r3, r1, 4
	l.or r1, r3, r3
	l.jr lr
.Lfunc_end0:
	.size	extload, .Lfunc_end0-extload
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
