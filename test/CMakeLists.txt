file(GLOB OT_TESTS "*.ot")

foreach(TEST ${OT_TESTS})
	add_test(NAME ${TEST} COMMAND ot ${TEST})
endforeach()
