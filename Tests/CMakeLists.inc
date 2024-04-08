set(CURRENT_FILTER "Tests")

if (${GTest_FOUND})

  discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Functional FUNCTIONAL_TESTS_H FUNCTIONAL_TESTS_CPP)
  add_executable(FunctionalTests ${FUNCTIONAL_TESTS_H} ${FUNCTIONAL_TESTS_CPP})
  configure_clicpp(FunctionalTests "FunctionalTests" googletest MiCoSi.CliArgs MiCoSi.API MiCoSi.Launcher)
  add_dependencies(FunctionalTests MiCoSi.CliArgs MiCoSi.API MiCoSi.Launcher)
  add_test(NAME FunctionalTests WORKING_DIRECTORY "${WORKING_DIR}"
                                COMMAND ${WORKING_DIR}/${out_name}.exe)

  discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Unit UNIT_TESTS_H UNIT_TESTS_CPP)
  add_executable(UnitTests ${UNIT_TESTS_H} ${UNIT_TESTS_CPP})
  configure_cpp(UnitTests "UnitTests" googletest ${MICOSI_SOLVER_LIBS})
  add_test(NAME UnitTests WORKING_DIRECTORY "${WORKING_DIR}"
                          COMMAND ${WORKING_DIR}/${out_name}.exe)
endif()
