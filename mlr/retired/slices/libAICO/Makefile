BASE   = $(shell realpath .)
NAME   = $(shell basename $(BASE))

ALL =	extern/SWIFT++_1.2/src \
	src/MT
#	extern/SWIFT++_1.2/decomposer/src \

makeAll: $(ALL:%=makePath/%) makeSubpaths/test 

cleanAll: $(ALL:%=cleanPath/%) cleanSubpaths/test 

include build/generic.mk
