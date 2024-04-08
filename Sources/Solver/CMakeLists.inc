set(CURRENT_FILTER "Solver")

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.Core MICOSI_CORE_H MICOSI_CORE_CPP)
add_library(MiCoSi.Core STATIC ${MICOSI_CORE_H} ${MICOSI_CORE_CPP})
configure_cpp(MiCoSi.Core "core")
list(APPEND MICOSI_SOLVER_LIBS MiCoSi.Core)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.Geometry MICOSI_GEOMETRY_H MICOSI_GEOMETRY_CPP)
add_library(MiCoSi.Geometry STATIC ${MICOSI_GEOMETRY_H} ${MICOSI_GEOMETRY_CPP})
configure_cpp(MiCoSi.Geometry "geometry")
list(APPEND MICOSI_SOLVER_LIBS MiCoSi.Geometry)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.Objects MICOSI_OBJECTS_H MICOSI_OBJECTS_CPP)
add_library(MiCoSi.Objects STATIC ${MICOSI_OBJECTS_H} ${MICOSI_OBJECTS_CPP})
configure_cpp(MiCoSi.Objects "objects")
list(APPEND MICOSI_SOLVER_LIBS MiCoSi.Objects)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.Formatters MICOSI_FORMATTERS_H MICOSI_FORMATTERS_CPP)
add_library(MiCoSi.Formatters STATIC ${MICOSI_FORMATTERS_H} ${MICOSI_FORMATTERS_CPP})
configure_cpp(MiCoSi.Formatters "formatters")
list(APPEND MICOSI_SOLVER_LIBS MiCoSi.Formatters)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.Streams MICOSI_STREAMS_H MICOSI_STREAMS_CPP)
add_library(MiCoSi.Streams STATIC ${MICOSI_STREAMS_H} ${MICOSI_STREAMS_CPP})
configure_cpp(MiCoSi.Streams "streams")
list(APPEND MICOSI_SOLVER_LIBS MiCoSi.Streams)

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.Solvers MICOSI_SOLVERS_H MICOSI_SOLVERS_CPP)
add_library(MiCoSi.Solvers STATIC ${MICOSI_SOLVERS_H} ${MICOSI_SOLVERS_CPP})
configure_cpp(MiCoSi.Solvers "solvers")
list(APPEND MICOSI_SOLVER_LIBS MiCoSi.Solvers)
