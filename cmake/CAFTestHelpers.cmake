include(CMakeParseArguments)

function(scan_caf_test_suites target)
  message(STATUS "Scanning ${target} for test suites")
  cmake_parse_arguments(SCAN "" "REGEX" "" ${ARGN})
  get_target_property(sources ${target} SOURCES)
  foreach(source ${sources})
    file(STRINGS ${source} contents)
    foreach(line ${contents})
      if ("${line}" MATCHES "${SCAN_REGEX}")
        string(REGEX REPLACE ".*${SCAN_REGEX}.*" "\\1" suite ${line})
        list(APPEND suites ${suite})
      endif()
    endforeach()
  endforeach()
  list(LENGTH suites num_suites)
  if(${num_suites} GREATER 0)
    list(REMOVE_DUPLICATES suites)
    set_target_properties(${target} PROPERTIES CAF_TEST_SUITES "${suites}")
  endif()
endfunction(scan_caf_test_suites)

function(collect_suites_rec target output)
  if(TARGET ${target})
    get_target_property(type_ ${target} TYPE)
    if(NOT "INTERFACE_LIBRARY" STREQUAL "${type_}")
      get_target_property(libs ${target} LINK_LIBRARIES)
      if(NOT "libs-NOTFOUND" STREQUAL "${libs}")
        foreach(lib ${libs})
          collect_suites_rec(${lib} child_output)
          list(APPEND my_output ${child_output})
        endforeach()
      endif()
      get_target_property(suites ${target} CAF_TEST_SUITES)
      if(NOT "suites-NOTFOUND" STREQUAL "${suites}")
        list(APPEND my_output ${suites})
      endif()
      set(${output} ${my_output} PARENT_SCOPE)
    endif()
  endif()
endfunction(collect_suites_rec)

function(register_caf_tests target)
  cmake_parse_arguments(REGISTER "" "ARGUMENTS" "" ${ARGN})
  collect_suites_rec(${target} suites)
  list(LENGTH suites num_suites)
  if(${num_suites} GREATER 0)
    list(REMOVE_DUPLICATES suites)
    # creates one CMake test per test suite.
    macro (make_test suite)
      string(REPLACE " " "_" test_name ${suite})
      add_test(NAME ${test_name} COMMAND ${target} ${REGISTER_ARGUMENTS} -s "${suite}" ${ARGN})
    endmacro ()
    list(LENGTH suites num_suites)
    message(STATUS "Found ${num_suites} test suites")
    foreach(suite ${suites})
      make_test("${suite}")
    endforeach ()
  endif()
endfunction(register_caf_tests)