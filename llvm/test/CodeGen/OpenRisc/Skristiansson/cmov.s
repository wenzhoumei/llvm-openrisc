	.text
	.file	"cmov.ll"
	.global	foo                             # -- Begin function foo
	.p2align	2
	.type	foo,@function
foo:                                    # @foo
# %bb.0:                                # %entry
	l.sfgts r3, r4
	l.bf .LBB0_2
# %bb.1:                                # %entry
	l.or r3, r4, r4
.LBB0_2:                                # %entry
	l.or r11, r3, r3
	l.jr lr
.Lfunc_end0:
	.size	foo, .Lfunc_end0-foo
                                        # -- End function
	.section	".note.GNU-stack","",@progbits
