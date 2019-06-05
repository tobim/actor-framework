include(CAFTestHelpers)

set(LIBCAF_IO_TEST_SRCS
  automatic_connection.cpp
  basp.cpp
  default_multiplexer.cpp
  dynamic_broker.cpp
  http_broker.cpp
  ip_endpoint.cpp
  receive_buffer.cpp
  remote_actor.cpp
  remote_group.cpp
  remote_spawn.cpp
  triggering.cpp
  typed_broker.cpp
  typed_remote_actor.cpp
  unpublish.cpp
)

if (TARGET caf::test-main)
  add_executable(libcaf_io-test ${LIBCAF_IO_TEST_SRCS})
  target_link_libraries(libcaf_io-test caf::io caf::test-main)
else ()
  add_executable(libcaf_io-test ${LIBCAF_IO_TEST_SRCS} ${CAF_TEST_MAIN})
  target_link_libraries(libcaf_io-test caf::io caf::test)
endif ()
scan_caf_test_suites(libcaf_io-test REGEX "${TEST_REGEX}")
register_caf_tests(libcaf_io-test ARGUMENTS "-r 300 -n -v 5")
