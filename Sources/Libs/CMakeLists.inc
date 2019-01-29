set(CURRENT_FILTER "Libs")

discover_cpp(${CMAKE_CURRENT_LIST_DIR}/Mitosis.API MITOSIS_API_H MITOSIS_API_CPP)
add_library(Mitosis.API MODULE ${MITOSIS_API_H} ${MITOSIS_API_CPP})
configure_cpp(Mitosis.API "Mitosis.API" 3rdParty ${MITOSIS_SOLVER_LIBS})
set_target_properties(Mitosis.API PROPERTIES COMMON_LANGUAGE_RUNTIME "")
set_target_properties(Mitosis.API PROPERTIES DOTNET_TARGET_FRAMEWORK_VERSION "v4.0")
