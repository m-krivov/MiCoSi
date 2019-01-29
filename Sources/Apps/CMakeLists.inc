set(CURRENT_FILTER "Apps")

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/ConsoleApp.Args CONSOLEAPP_ARGS_H CONSOLEAPP_ARGS_CPP)
add_library(ConsoleApp.Args STATIC ${CONSOLEAPP_ARGS_H} ${CONSOLEAPP_ARGS_CPP})
configure_cpp(ConsoleApp.Args "args")

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/ConsoleApp CONSOLEAPP_H CONSOLEAPP_CPP)
add_executable(ConsoleApp ${CONSOLEAPP_H} ${CONSOLEAPP_CPP})
configure_cpp(ConsoleApp "mitosis" ConsoleApp.Args 3rdParty ${MITOSIS_SOLVER_LIBS})

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Visualizer)
