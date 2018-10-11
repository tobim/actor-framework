find_package(OpenCL REQUIRED)
include(OpenCLTargets)

# get header files; only needed by CMake generators,
# e.g., for creating proper Xcode projects
file(GLOB_RECURSE LIBCAF_OPENCL_HDRS "caf/*.hpp")

# list cpp files excluding platform-dependent files
set(LIBCAF_OPENCL_SRCS
  src/device.cpp
  src/global.cpp
  src/manager.cpp
  src/opencl_err.cpp
  src/platform.cpp
  src/program.cpp
)

add_library(libcaf_opencl ${LIBCAF_OPENCL_SRCS} ${LIBCAF_OPENCL_HDRS})
target_include_directories(libcaf_opencl PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
target_link_libraries(libcaf_opencl PUBLIC caf::core OpenCL::OpenCL)
set_target_properties(libcaf_opencl
  PROPERTIES
  EXPORT_NAME opencl
  SOVERSION ${CAF_VERSION_COMPAT}
  VERSION ${CAF_VERSION}
  OUTPUT_NAME caf_opencl
)

install(TARGETS libcaf_opencl
  EXPORT CAFOpenCLTargets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT opencl
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT opencl
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT opencl
)
install(EXPORT CAFOpenCLTargets
  DESTINATION ${INSTALL_CAF_CMAKEDIR}
  NAMESPACE caf::
  EXPORT_LINK_INTERFACE_LIBRARIES
  COMPONENT opencl
)
install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/caf"
  DESTINATION include
  COMPONENT opencl
  FILES_MATCHING PATTERN "*.hpp"
)

# create alias so find_package(CAF) and add_subdirectory(caf) can be
# interchanged without the need of additional modifications to the
# consuming cmake scripts
add_library(caf::opencl ALIAS libcaf_opencl)

# export the OpenCL target to the build tree
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/OpenCLTargets.cmake
  ${PROJECT_BINARY_DIR}
  COPYONLY
)
install(FILES
  "${PROJECT_BINARY_DIR}/OpenCLTargets.cmake"
  DESTINATION ${INSTALL_CAF_CMAKEDIR}
  COMPONENT dev
)

add_subdirectory(test)
