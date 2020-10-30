set(CURRENT_FILTER "Tests")

if (GTEST_FOUND)
  add_library(gtest STATIC IMPORTED)
  set_target_properties(gtest PROPERTIES IMPORTED_LOCATION_DEBUG
                        $ENV{GTEST_ROOT}/lib/gtestd.lib)
  set_target_properties(gtest PROPERTIES IMPORTED_LOCATION_RELEASE
                        $ENV{GTEST_ROOT}/lib/gtest.lib)

  discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Functional FUNCTIONAL_TESTS_H FUNCTIONAL_TESTS_CPP)
  add_executable(FunctionalTests ${FUNCTIONAL_TESTS_H} ${FUNCTIONAL_TESTS_CPP})
  configure_clicpp(FunctionalTests "FunctionalTests" gtest ConsoleApp.CliArgs Mitosis.API Mitosis.Launcher)
  add_dependencies(FunctionalTests ConsoleApp.CliArgs Mitosis.API Mitosis.Launcher)
  target_include_directories(FunctionalTests SYSTEM PUBLIC "${GTEST_INCLUDE_DIRS}")
  add_test(NAME FunctionalTests WORKING_DIRECTORY "${WORKING_DIR}"
                                COMMAND ${WORKING_DIR}/${out_name}.exe)

  discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Unit UNIT_TESTS_H UNIT_TESTS_CPP)
  add_executable(UnitTests ${UNIT_TESTS_H} ${UNIT_TESTS_CPP})
  configure_cpp(UnitTests "UnitTests" gtest Mitosis.Core Mitosis.Geometry Mitosis.Solvers)
  target_include_directories(UnitTests SYSTEM PUBLIC "${GTEST_INCLUDE_DIRS}")
  add_test(NAME UnitTests WORKING_DIRECTORY "${WORKING_DIR}"
                          COMMAND ${WORKING_DIR}/${out_name}.exe)
endif()
