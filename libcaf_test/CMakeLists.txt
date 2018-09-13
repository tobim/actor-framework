include(CAFTestHelpers)

file(GLOB_RECURSE LIBCAF_TEST_HDRS "caf/*.hpp")

add_library(libcaf_test INTERFACE)

target_include_directories(libcaf_test INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)
if (TARGET Threads::Threads)
  target_link_libraries(libcaf_test INTERFACE caf::core caf::io Threads::Threads)
else ()
  if(NOT APPLE AND NOT WIN32)
    target_link_libraries(libcaf_test INTERFACE -pthread)
    target_compile_options(libcaf_test INTERFACE -pthread)
  endif ()
endif ()

set_target_properties(libcaf_test PROPERTIES EXPORT_NAME test)

install(TARGETS libcaf_test
  EXPORT CAFTestTargets
  PUBLIC_HEADER DESTINATION include
  COMPONENT dev
)

install(
  DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/caf"
  DESTINATION include
  COMPONENT dev
)

# 'export' the test helper module to the build tree
configure_file(
  "${PROJECT_SOURCE_DIR}/cmake/CAFTestHelpers.cmake"
  "${PROJECT_BINARY_DIR}"
  COPYONLY
)

install(FILES
  "${PROJECT_BINARY_DIR}/CAFTestHelpers.cmake"
  DESTINATION "${INSTALL_CAF_CMAKEDIR}"
  COMPONENT dev
)

add_library(caf::test ALIAS libcaf_test)

if (NOT ${CMAKE_VERSION} VERSION_LESS "3.1")
  add_library(libcaf_test-main INTERFACE)
  target_link_libraries(libcaf_test-main INTERFACE libcaf_test)
  set(isExe $<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>)
  target_sources(libcaf_test-main INTERFACE
      "$<${isExe}:$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/caf-test.cpp>>"
      "$<${isExe}:$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${DATADIR}/caf/test/caf-test.cpp>>"
  )

  if (NOT ${CMAKE_VERSION} VERSION_LESS "3.3")
    set_target_properties(libcaf_test-main PROPERTIES EXPORT_NAME test-main)

    install(TARGETS libcaf_test-main
      EXPORT CAFTestTargets
      PUBLIC_HEADER DESTINATION include
      COMPONENT dev
    )
    install(FILES src/caf-test.cpp DESTINATION ${DATADIR}/caf/test)
  endif ()

  add_library(caf::test-main ALIAS libcaf_test-main)
else ()
  set(CAF_TEST_MAIN "${CMAKE_CURRENT_SOURCE_DIR}/src/caf-test.cpp" PARENT_SCOPE)
endif ()

install(EXPORT CAFTestTargets
  DESTINATION "${INSTALL_CAF_CMAKEDIR}"
  NAMESPACE caf::
  EXPORT_LINK_INTERFACE_LIBRARIES
)

export(EXPORT CAFTestTargets
  FILE CAFTestTargets.cmake
  NAMESPACE caf::
)

# 'export' the test helper module to the build tree
configure_file(
  ${PROJECT_SOURCE_DIR}/cmake/CAFTestHelpers.cmake
  ${PROJECT_BINARY_DIR}
  COPYONLY
)

install(FILES
  "${PROJECT_BINARY_DIR}/CAFTestHelpers.cmake"
  DESTINATION "${INSTALL_CAF_CMAKEDIR}"
  COMPONENT dev
)
