set(CURRENT_FILTER "Solver")

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Mitosis.Core MITOSIS_CORE_H MITOSIS_CORE_CPP)
add_library(Mitosis.Core STATIC ${MITOSIS_CORE_H} ${MITOSIS_CORE_CPP})
configure_cpp(Mitosis.Core "core")
list(APPEND MITOSIS_SOLVER_LIBS Mitosis.Core)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Mitosis.Geometry MITOSIS_GEOMETRY_H MITOSIS_GEOMETRY_CPP)
add_library(Mitosis.Geometry STATIC ${MITOSIS_GEOMETRY_H} ${MITOSIS_GEOMETRY_CPP})
configure_cpp(Mitosis.Geometry "geometry")
list(APPEND MITOSIS_SOLVER_LIBS Mitosis.Geometry)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Mitosis.Objects MITOSIS_OBJECTS_H MITOSIS_OBJECTS_CPP)
add_library(Mitosis.Objects STATIC ${MITOSIS_OBJECTS_H} ${MITOSIS_OBJECTS_CPP})
configure_cpp(Mitosis.Objects "objects")
list(APPEND MITOSIS_SOLVER_LIBS Mitosis.Objects)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Mitosis.Formatters MITOSIS_FORMATTERS_H MITOSIS_FORMATTERS_CPP)
add_library(Mitosis.Formatters STATIC ${MITOSIS_FORMATTERS_H} ${MITOSIS_FORMATTERS_CPP})
configure_cpp(Mitosis.Formatters "formatters")
list(APPEND MITOSIS_SOLVER_LIBS Mitosis.Formatters)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Mitosis.Streams MITOSIS_STREAMS_H MITOSIS_STREAMS_CPP)
add_library(Mitosis.Streams STATIC ${MITOSIS_STREAMS_H} ${MITOSIS_STREAMS_CPP})
configure_cpp(Mitosis.Streams "streams")
list(APPEND MITOSIS_SOLVER_LIBS Mitosis.Streams)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Mitosis.Solvers MITOSIS_SOLVERS_H MITOSIS_SOLVERS_CPP)
add_library(Mitosis.Solvers STATIC ${MITOSIS_SOLVERS_H} ${MITOSIS_SOLVERS_CPP})
configure_cpp(Mitosis.Solvers "solvers")
list(APPEND MITOSIS_SOLVER_LIBS Mitosis.Solvers)
