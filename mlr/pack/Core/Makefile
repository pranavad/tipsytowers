BASE = .

NAME   = $(shell basename $(PWD))

################################################################################

src_paths =  $(shell find src -mindepth 1 -maxdepth 1 -type d -not -name 'extern' -not -name 'CMakeFiles' -not -name 'retired')

exa_paths = $(shell find examples -maxdepth 3 -name 'makefile' | grep -o '.*/')

test_paths = $(shell find test -maxdepth 3 -name 'makefile' | grep -o '.*/')

################################################################################

srcTest: $(src_paths:%=makePath/%) $(test_paths:%=makeTest/%)

clean: $(src_paths:%=cleanPath/%) $(test_paths:%=cleanPath/%) cleanLocks

################################################################################

# test: setConfigFlag $(exa_paths:%=cleanPath/%) cleanLocks $(exa_paths:%=makePath/%)

# setConfigFlag: force
# 	echo "MLR_TESTS = 1" > build/z.mk

runTests:
	@rm -f z.test-report
	@find test  -mindepth 1 -maxdepth 1 -type d -exec build/run-path.sh {} \;

#testLibs:
#	@find examples -mindepth 2 -maxdepth 2 -type d \
# 	      -not -name '*py' -not -name 'CMakeFiles'\
#	      -exec $(BASE)/build/make-path.sh {} lib_test.so \;

################################################################################


force:

include build/generic.mk
