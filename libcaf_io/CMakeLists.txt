# get header files; only needed by CMake generators,
# e.g., for creating proper Xcode projects
file(GLOB_RECURSE LIBCAF_IO_HDRS "caf/*.hpp")

# list cpp files excluding platform-dependent files
set(LIBCAF_IO_SRCS
  src/abstract_broker.cpp
  src/acceptor_manager.cpp
  src/acceptor_manager.cpp
  src/basp_broker.cpp
  src/broker.cpp
  src/connection_helper.cpp
  src/datagram_manager.cpp
  src/datagram_servant.cpp
  src/default_multiplexer.cpp
  src/doorman.cpp
  src/header.cpp
  src/hook.cpp
  src/instance.cpp
  src/interfaces.cpp
  src/ip_endpoint.cpp
  src/manager.cpp
  src/message_type.cpp
  src/middleman.cpp
  src/middleman_actor.cpp
  src/middleman_actor_impl.cpp
  src/multiplexer.cpp
  src/multiplexer.cpp
  src/protocol.cpp
  src/receive_buffer.cpp
  src/routing_table.cpp
  src/scribe.cpp
  src/stream_manager.cpp
  src/test_multiplexer.cpp
  src/acceptor.cpp
  src/datagram_handler.cpp
  src/datagram_servant_impl.cpp
  src/doorman_impl.cpp
  src/event_handler.cpp
  src/pipe_reader.cpp
  src/scribe_impl.cpp
  src/stream.cpp
  src/tcp.cpp
  src/udp.cpp
  src/native_socket.cpp
  src/socket_guard.cpp
)

add_library(libcaf_io ${LIBCAF_IO_SRCS} ${LIBCAF_IO_HDRS})
target_include_directories(libcaf_io PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(libcaf_io PUBLIC caf::core)
if (TARGET Threads::Threads)
  target_link_libraries(libcaf_io PUBLIC Threads::Threads)
else ()
  if(NOT APPLE AND NOT WIN32)
    target_link_libraries(libcaf_io PUBLIC -pthread)
    target_compile_options(libcaf_io PUBLIC -pthread)
  endif ()
endif ()
set_target_properties(libcaf_io
  PROPERTIES
  EXPORT_NAME io
  SOVERSION ${CAF_VERSION_COMPAT}
  VERSION ${CAF_VERSION}
  OUTPUT_NAME caf_io
)

install(TARGETS libcaf_io
  EXPORT CAFIOTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(EXPORT CAFIOTargets
  DESTINATION ${INSTALL_CAF_CMAKEDIR}
  NAMESPACE caf::
  EXPORT_LINK_INTERFACE_LIBRARIES
)
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/caf"
  DESTINATION include
  COMPONENT dev
  FILES_MATCHING PATTERN "*.hpp"
)

export(EXPORT CAFIOTargets
  FILE CAFIOTargets.cmake
  NAMESPACE caf::
)

# create alias so find_package(CAF) and add_subdirectory(caf) can be
# interchanged without the need of additional modifications to the
# consuming cmake scripts
add_library(caf::io ALIAS libcaf_io)

add_subdirectory(test)
