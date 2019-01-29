cmake_minimum_required(VERSION 3.13) # v3.12 has bug with C# (failes to set reference)
project(MiCoSi LANGUAGES CXX CSharp)
include(CSharpUtilities)

# Configure VS and directories
if(MSVC)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
    set(CMAKE_SUPPRESS_REGENERATION ON)
endif()

# Define macros for less code duplication
macro(discover_cpp dir headers cpp)
  file(GLOB_RECURSE ${headers} ${dir}/*.h)
  file(GLOB_RECURSE ${cpp}     ${dir}/*.cpp)

  # Configure filters for VS project
  if(MSVC)
    foreach(_source IN ITEMS ${${headers}})
      get_filename_component(_source_path "${_source}" PATH)
      file(RELATIVE_PATH _source_path_rel "${dir}" "${_source_path}")
      string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
      source_group("Header Files\\${_group_path}" FILES "${_source}")
    endforeach()

    foreach(_source IN ITEMS ${${cpp}})
      get_filename_component(_source_path "${_source}" PATH)
      file(RELATIVE_PATH _source_path_rel "${dir}" "${_source_path}")
      string(REPLACE "/" "\\" _group_path "${_source_path_rel}")
      source_group("Source Files\\${_group_path}" FILES "${_source}")
    endforeach()
  endif()
endmacro(discover_cpp dir headers cpp)

macro(discover_cs dir sources)
  file(GLOB_RECURSE ${sources} ${dir}/*.cs)
  csharp_set_windows_forms_properties(${${sources}})
endmacro(discover_cs dir sources)

set(CURRENT_FILTER "")
macro(configure_filter proj)
if(MSVC AND NOT "${CURRENT_FILTER}" STREQUAL "")
  set_target_properties(${proj} PROPERTIES FOLDER ${CURRENT_FILTER})
endif()
endmacro(configure_filter proj)

macro(configure_cpp proj out_name)
  configure_filter(${proj})
  target_include_directories(${proj} PUBLIC ${CMAKE_SOURCE_DIR}/Sources/Solver)
  target_include_directories(${proj} PUBLIC ${CMAKE_SOURCE_DIR}/Sources/Apps)
  target_include_directories(${proj} PUBLIC ${CMAKE_SOURCE_DIR}/3rdParty/TinyXML)
  target_include_directories(${proj} PUBLIC ${CMAKE_SOURCE_DIR}/3rdParty/ttgLabs)
  set_target_properties(${proj} PROPERTIES PREFIX "")
  set_target_properties(${proj} PROPERTIES OUTPUT_NAME ${out_name})

  get_target_property(tt ${proj} TYPE)
  if(tt STREQUAL "EXECUTABLE" OR tt STREQUAL "MODULE_LIBRARY")
    add_custom_command(TARGET ${proj} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${proj}> ${CMAKE_SOURCE_DIR}/SDK/Bin)
    set_target_properties(${proj} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/SDK")
  endif()

  set(args ${ARGN})
  list(LENGTH args arg_num)
  if(${arg_num} GREATER 0)
    math(EXPR arg_num ${arg_num}-1)
    foreach(i RANGE 0 ${arg_num})
      list(GET args ${i} arg)
      target_link_libraries(${proj} ${arg})
    endforeach(i)
  endif()
endmacro(configure_cpp proj out_name)

macro(configure_cs proj out_name)
  configure_filter(${proj})
  set_target_properties(Visualizer PROPERTIES DOTNET_TARGET_FRAMEWORK_VERSION "v4.0")
  target_compile_options(${proj} PUBLIC "/unsafe")

  get_target_property(tt ${proj} TYPE)
  if(tt STREQUAL "EXECUTABLE" OR tt STREQUAL "MODULE_LIBRARY")
    add_custom_command(TARGET ${proj} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${proj}> ${CMAKE_SOURCE_DIR}/SDK/Bin)
  endif()

  set(cs_deps
    "Microsoft.CSharp"
    "System"
    "System.Data"
    "System.Core"
    "System.Xml"
    "System.Drawing"
    "System.Windows.Forms"
    "WindowsBase"
    "${CMAKE_SOURCE_DIR}/SDK/Bin/WindowsMediaLib.dll"
    "${CMAKE_SOURCE_DIR}/SDK/Bin/SlimDX.dll"
    "${CMAKE_SOURCE_DIR}/SDK/Bin/SpriteTextRenderer.dll"
    "${CMAKE_SOURCE_DIR}/SDK/Bin/SpriteTextRenderer.SlimDX.dll"
    "${CMAKE_SOURCE_DIR}/SDK/Bin/DirectShow.dll"
  )

  set(args ${ARGN})
  list(LENGTH args arg_num)
  if(${arg_num} GREATER 0)
    math(EXPR arg_num ${arg_num}-1)
    foreach(i RANGE 0 ${arg_num})
      list(GET args ${i} arg)
      list(APPEND cs_deps "${CMAKE_SOURCE_DIR}/SDK/Bin/${arg}.dll")
    endforeach(i)
  set_property(TARGET ${proj} PROPERTY VS_DOTNET_REFERENCES ${cs_deps})

  endif()

endmacro(configure_cs proj out_name)

# Copy the global supplementary files to the image of SDK
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/SDK)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/SDK/Bin)
file(COPY ${CMAKE_SOURCE_DIR}/Resources/Docs DESTINATION ${CMAKE_SOURCE_DIR}/SDK)
file(COPY ${CMAKE_SOURCE_DIR}/Resources/UI DESTINATION ${CMAKE_SOURCE_DIR}/SDK)
file(COPY ${CMAKE_SOURCE_DIR}/Resources/Samples DESTINATION ${CMAKE_SOURCE_DIR}/SDK)

# Finally, include rules for sources and tests
include(${CMAKE_SOURCE_DIR}/3rdParty/CMakeLists.inc)
include(${CMAKE_SOURCE_DIR}/Sources/CMakeLists.inc)
