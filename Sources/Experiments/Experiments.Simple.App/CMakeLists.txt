discover_cs(${CMAKE_CURRENT_LIST_DIR} EXPERIMENTS_SIMPLE_APP_CS)
add_executable(Experiments.Simple.App ${EXPERIMENTS_SIMPLE_APP_CS})
configure_cs(Experiments.Simple.App "Simple.App"
             MiCoSi.Launcher MiCoSi.Simulator MiCoSi.CliArgs MiCoSi.API)
add_dependencies(Experiments.Simple.App MiCoSi.Launcher)
