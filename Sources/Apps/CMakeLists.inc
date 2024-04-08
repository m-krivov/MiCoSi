set(CURRENT_FILTER "Apps")

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.Args MICOSI_ARGS_H MICOSI_ARGS_CPP)
add_library(MiCoSi.Args STATIC ${MICOSI_ARGS_H} ${MICOSI_ARGS_CPP})
configure_cpp(MiCoSi.Args "MiCoSi.Args")

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.CliArgs MICOSI_CLIARGS_H MICOSI_CLIARGS_CPP)
add_library(MiCoSi.CliArgs MODULE ${MICOSI_CLIARGS_H} ${MICOSI_CLIARGS_CPP})
configure_clicpp(MiCoSi.CliArgs "MiCoSi.CliArgs" MiCoSi.Args ${MICOSI_SOLVER_LIBS})
export_to_sdk(MiCoSi.CliArgs)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.App MICOSI_APP_H MICOSI_APP_CPP)
add_executable(MiCoSi.App ${MICOSI_APP_H} ${MICOSI_APP_CPP})
configure_cpp(MiCoSi.App "MiCoSi" MiCoSi.Args 3rdParty ${MICOSI_SOLVER_LIBS})
export_to_sdk(MiCoSi.App)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.Visualizer.App)
