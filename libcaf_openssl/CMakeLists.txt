find_package(OpenSSL 1.0.1 REQUIRED)
include(OpenSSLTargets)

# get header files; only needed by CMake generators,
# e.g., for creating proper Xcode projects
file(GLOB_RECURSE LIBCAF_OPENSSL_HDRS "caf/*.hpp")

# list cpp files excluding platform-dependent files
set(LIBCAF_OPENSSL_SRCS
  src/manager.cpp
  src/middleman_actor.cpp
  src/publish.cpp
  src/remote_actor.cpp
  src/session.cpp
)

add_library(libcaf_openssl ${LIBCAF_OPENSSL_SRCS} ${LIBCAF_OPENSSL_HDRS})
target_include_directories(libcaf_openssl PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(libcaf_openssl PUBLIC caf::core caf::io OpenSSL::SSL)
set_target_properties(libcaf_openssl
  PROPERTIES
  EXPORT_NAME openssl
  SOVERSION ${CAF_VERSION_COMPAT}
  VERSION ${CAF_VERSION}
  OUTPUT_NAME caf_openssl
)

install(TARGETS libcaf_openssl
  EXPORT CAFOpenSSLTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT openssl
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT openssl
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT openssl
)
install(EXPORT CAFOpenSSLTargets
  DESTINATION ${INSTALL_CAF_CMAKEDIR}
  NAMESPACE caf::
  EXPORT_LINK_INTERFACE_LIBRARIES
  COMPONENT openssl
)
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/caf"
  DESTINATION include
  COMPONENT openssl
  FILES_MATCHING PATTERN "*.hpp"
)

# create alias so find_package(CAF) and add_subdirectory(caf) can be
# interchanged without the need of additional modifications to the
# consuming cmake scripts
add_library(caf::openssl ALIAS libcaf_openssl)

# export the OpenSSL targets to the build tree
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/OpenSSLTargets.cmake
  ${PROJECT_BINARY_DIR}
  COPYONLY
)
# export the OpenSSL targets to the install location
install(FILES
  "${PROJECT_BINARY_DIR}/OpenSSLTargets.cmake"
  DESTINATION ${INSTALL_CAF_CMAKEDIR}
  COMPONENT dev
)

add_subdirectory(test)
