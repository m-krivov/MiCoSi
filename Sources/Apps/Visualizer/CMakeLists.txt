discover_cs(${CMAKE_CURRENT_LIST_DIR} VISUALIZER_CS)
add_executable(Visualizer WIN32 ${VISUALIZER_CS})
configure_cs(Visualizer "visualizer" Mitosis.API
             WindowsMediaLib SlimDX SpriteTextRenderer
             SpriteTextRenderer.SlimDX DirectShow)
export_to_sdk(Visualizer)
add_dependencies(Visualizer Mitosis.API)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/Render/Shaders.fx DESTINATION ${SDK_DIR}/UI)
file(COPY ${CMAKE_CURRENT_LIST_DIR}/Capture/Encoder.prx DESTINATION ${SDK_DIR}/UI)
