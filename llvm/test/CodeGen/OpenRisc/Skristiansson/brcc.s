	.text
	.file	"brcc.ll"
	.global	brcc                            # -- Begin function brcc
	.p2align	2
	.type	brcc,@function
brcc:                                   # @brcc
# %bb.0:                                # %entry
	l.movhi r4, hi(0)
	l.sfeq r3, r4
	l.bf .LBB0_2
	l.j .LBB0_1
.LBB0_1:                                # %while.body
                                        # =>This Inner Loop Header: Depth=1
	l.addi r3, r3, -1
	l.sfne r3, r4
	l.bf .LBB0_1
	l.j .LBB0_2
.LBB0_2:                                # %while.end
	l.jr lr
.Lfunc_end0:
	.size	brcc, .Lfunc_end0-brcc
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
