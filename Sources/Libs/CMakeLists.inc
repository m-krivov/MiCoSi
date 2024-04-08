set(CURRENT_FILTER "Libs")

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.API MICOSI_API_H MICOSI_API_CPP)
add_library(MiCoSi.API MODULE ${MICOSI_API_H} ${MICOSI_API_CPP})
configure_clicpp(MiCoSi.API "MiCoSi.API" 3rdParty ${MICOSI_SOLVER_LIBS})
export_to_sdk(MiCoSi.API)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/MiCoSi.Launcher)
