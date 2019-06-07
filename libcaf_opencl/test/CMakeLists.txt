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
setup_caf_test(libcaf_opencl-test)
