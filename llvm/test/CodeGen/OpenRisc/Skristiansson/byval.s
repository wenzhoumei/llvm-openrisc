	.text
	.file	"byval.ll"
	.global	f1                              # -- Begin function f1
	.p2align	2
	.type	f1,@function
f1:                                     # @f1
	.cfi_startproc
# %bb.0:                                # %entry
	.cfi_def_cfa_offset 0
	l.movhi r4, hi(f0)
	ori r4, r4, lo(f0)
	l.jalr r4
	l.jr lr
.Lfunc_end0:
	.size	f1, .Lfunc_end0-f1
	.cfi_endproc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
