BASE   = $(shell realpath .)
NAME   = $(shell basename $(BASE))

ALL =	src/MT

makeAll: $(ALL:%=makePath/%) makeSubpaths/test 

cleanAll: $(ALL:%=cleanPath/%) cleanSubpaths/test 

include build/generic.mk
