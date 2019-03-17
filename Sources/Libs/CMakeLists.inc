set(CURRENT_FILTER "Libs")

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Mitosis.API MITOSIS_API_H MITOSIS_API_CPP)
add_library(Mitosis.API MODULE ${MITOSIS_API_H} ${MITOSIS_API_CPP})
configure_clicpp(Mitosis.API "Mitosis.API" 3rdParty ${MITOSIS_SOLVER_LIBS})
export_to_sdk(Mitosis.API)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Mitosis.Launcher)
