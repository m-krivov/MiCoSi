cmake_minimum_required(VERSION 3.13)
project(MiCoSi LANGUAGES CXX CSharp)
if(NOT MSVC)
  message(FATAL_ERROR "MiCoSi can be compiled only by Visual Studio")
endif()
include(CSharpUtilities)


# Configure VS and directories
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
set(CMAKE_SUPPRESS_REGENERATION ON)
set(WORKING_DIR ${CMAKE_SOURCE_DIR}/WORKING_DIR)
set(SDK_DIR     ${WORKING_DIR}/SDK)


# Discover Google Tests
if(EXISTS $ENV{GTEST_ROOT}/include)
  enable_testing()
  find_package(GTest REQUIRED)
else()
  set(GTEST_FOUND OFF)
  message(WARNING "Google Tests is not found, unit tests are disabled")
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
  target_include_directories(${proj} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
  set_target_properties(${proj} PROPERTIES PREFIX "")
  set_target_properties(${proj} PROPERTIES OUTPUT_NAME ${out_name})

  set(args ${ARGN})
  list(LENGTH args arg_num)
  if(${arg_num} GREATER 0)
    math(EXPR arg_num ${arg_num}-1)
    foreach(i RANGE 0 ${arg_num})
      list(GET args ${i} arg)
      get_target_property(arg_type ${arg} TYPE)
      if(arg_type STREQUAL "STATIC_LIBRARY")
        target_link_libraries(${proj} ${arg})
      endif()
    endforeach(i)
  endif()

  get_target_property(proj_type ${proj} TYPE)
  if(proj_type STREQUAL "EXECUTABLE" OR
     proj_type STREQUAL "SHARED_LIBRARY" OR
     proj_type STREQUAL "MODULE_LIBRARY")
    add_custom_command(TARGET ${proj} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${proj}> ${WORKING_DIR})
  endif()

  if(MSVC AND proj_type STREQUAL "EXECUTABLE")
    set_property(TARGET ${proj} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${WORKING_DIR}")
  endif()
endmacro(configure_cpp proj out_name)

macro(configure_clicpp proj out_name)
  configure_cpp(${proj} ${out_name} ${ARGN})
  set_target_properties(${proj} PROPERTIES COMMON_LANGUAGE_RUNTIME "")
  set_target_properties(${proj} PROPERTIES DOTNET_TARGET_FRAMEWORK_VERSION "v4.0")

  list(APPEND dotnet_deps "System.Core.dll")
  set(args ${ARGN})
  list(LENGTH args arg_num)
  if(${arg_num} GREATER 0)
    math(EXPR arg_num ${arg_num}-1)
    foreach(i RANGE 0 ${arg_num})
      list(GET args ${i} arg)
      get_target_property(arg_type ${arg} TYPE)
      if(arg_type STREQUAL "MODULE_LIBRARY")
        list(APPEND dotnet_deps "${WORKING_DIR}/${arg}.dll")
      endif()
    endforeach(i)
  endif()
  set_property(TARGET ${proj} PROPERTY VS_DOTNET_REFERENCES ${dotnet_deps})
endmacro(configure_clicpp proj out_name)

macro(configure_cs proj out_name)
  configure_filter(${proj})
  set_target_properties(${proj} PROPERTIES DOTNET_TARGET_FRAMEWORK_VERSION "v4.0")
  target_compile_options(${proj} PUBLIC "/unsafe")

  set(cs_deps
    "Microsoft.CSharp"
    "System"
    "System.Data"
    "System.Core"
    "System.Xml"
    "System.Drawing"
    "System.Windows.Forms"
    "WindowsBase"
  )

  set(args ${ARGN})
  list(LENGTH args arg_num)
  if(${arg_num} GREATER 0)
    math(EXPR arg_num ${arg_num}-1)
    foreach(i RANGE 0 ${arg_num})
      list(GET args ${i} arg)
      list(APPEND cs_deps "${WORKING_DIR}/${arg}.dll")
    endforeach(i)
  set_property(TARGET ${proj} PROPERTY VS_DOTNET_REFERENCES ${cs_deps})

  get_target_property(tt ${proj} TYPE)
  if(tt STREQUAL "EXECUTABLE" OR tt STREQUAL "SHARED_LIBRARY" OR tt STREQUAL "MODULE_LIBRARY")
    add_custom_command(TARGET ${proj} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${proj}> ${WORKING_DIR})
  endif()

  if(MSVC AND tt STREQUAL "EXECUTABLE")
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/${proj}.csproj.user" 
         "<?xml version=\"1.0\" encoding=\"utf-8\"?> \
          <Project ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"> \
            <PropertyGroup Condition=\"'$(Configuration)|$(Platform)' == 'Release|x64'\"> \
              <StartWorkingDirectory>${WORKING_DIR}</StartWorkingDirectory> \
            </PropertyGroup> \
            <PropertyGroup Condition=\"'$(Configuration)|$(Platform)' == 'Debug|x64'\"> \
              <StartWorkingDirectory>${WORKING_DIR}</StartWorkingDirectory> \
            </PropertyGroup> \
          </Project>")
  endif()

  endif()

endmacro(configure_cs proj out_name)

macro(export_to_sdk proj)
  get_target_property(tt ${proj} TYPE)
  if(tt STREQUAL "EXECUTABLE" OR tt STREQUAL "SHARED_LIBRARY" OR tt STREQUAL "MODULE_LIBRARY")
    add_custom_command(TARGET ${proj} POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${proj}> ${SDK_DIR}/Bin)
  endif()
endmacro(export_to_sdk proj)

# Create working directory for all binaries, debuggung and unit tests
file(MAKE_DIRECTORY ${WORKING_DIR})

# Copy the global supplementary files to the image of SDK
file(MAKE_DIRECTORY ${SDK_DIR})
file(MAKE_DIRECTORY ${SDK_DIR}/Bin)
file(COPY ${CMAKE_SOURCE_DIR}/Resources/Docs DESTINATION ${SDK_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Resources/UI DESTINATION ${SDK_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/Resources/Samples DESTINATION ${SDK_DIR})

# Finally, include rules for sources and tests
include(${CMAKE_SOURCE_DIR}/3rdParty/CMakeLists.inc)
include(${CMAKE_SOURCE_DIR}/Sources/CMakeLists.inc)
include(${CMAKE_SOURCE_DIR}/Tests/CMakeLists.inc)
include(${CMAKE_SOURCE_DIR}/Experiments/CMakeLists.inc)
