discover_cs(${CMAKE_CURRENT_LIST_DIR} MEROTELIC_KTS_CS)
add_executable(MerotelicKTs ${MEROTELIC_KTS_CS})
configure_cs(MerotelicKTs "MerotelicKTs" Mitosis.Launcher ConsoleApp.CliArgs Mitosis.API)
add_dependencies(MerotelicKTs Mitosis.Launcher)
