include(CAFTestHelpers)

set(LIBCAF_OPENCL_TEST_SRCS
  opencl.cpp
)

if (TARGET caf::test-main)
  add_executable(libcaf_opencl-test ${LIBCAF_OPENCL_TEST_SRCS})
  target_link_libraries(libcaf_opencl-test caf::opencl caf::test-main)
else ()
  add_executable(libcaf_opencl-test ${LIBCAF_OPENCL_TEST_SRCS} ${CAF_TEST_MAIN})
  target_link_libraries(libcaf_opencl-test caf::opencl caf::test)
endif ()
scan_caf_test_suites(libcaf_opencl-test REGEX "${TEST_REGEX}")
register_caf_tests(libcaf_opencl-test ARGUMENTS "-r 300 -n -v 5")
