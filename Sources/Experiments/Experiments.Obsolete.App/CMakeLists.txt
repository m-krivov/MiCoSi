discover_cs(${CMAKE_CURRENT_LIST_DIR} EXPERIMENTS_OBSOLETE_APP_CS)
add_executable(Experiments.Obsolete.App ${EXPERIMENTS_OBSOLETE_APP_CS})
configure_cs(Experiments.Obsolete.App "Experiments.Obsolete.App" MiCoSi.Launcher MiCoSi.CliArgs MiCoSi.API)
add_dependencies(Experiments.Obsolete.App MiCoSi.Launcher)
