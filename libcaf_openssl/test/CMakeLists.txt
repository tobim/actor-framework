include(CAFTestHelpers)

set(LIBCAF_OPENSSL_TEST_SRCS
  authentication.cpp
  remote_actor.cpp
)

if (TARGET caf::test-main)
  add_executable(libcaf_openssl-test ${LIBCAF_OPENSSL_TEST_SRCS})
  target_link_libraries(libcaf_openssl-test caf::openssl caf::test-main)
else ()
  add_executable(libcaf_openssl-test ${LIBCAF_OPENSSL_TEST_SRCS} ${CAF_TEST_MAIN})
  target_link_libraries(libcaf_openssl-test caf::openssl caf::test)
endif ()
scan_caf_test_suites(libcaf_openssl-test REGEX "${TEST_REGEX}")
register_caf_tests(libcaf_openssl-test ARGUMENTS "-r 300 -n -v 5")
