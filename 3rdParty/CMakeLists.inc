set(CURRENT_FILTER "")

# A home-made analogue of the 'FetchContent()' routine
# Yes, I have a few reason to avoid using CMake's ExternalProject or git's submodules
macro(fetch_3rd_party
      repo_url commit_hash target_name
      include_subpath release_lib_subpath debug_lib_subpath)
  set(_3rd_party_dir          "${CMAKE_CURRENT_LIST_DIR}/${target_name}")
  set(_3rd_party_include      "${_3rd_party_dir}/${include_subpath}")
  set(_3rd_party_bin          "${_3rd_party_dir}/MiCoSi")
  set(_3rd_party_need_release NOT "${release_lib_subpath}" STREQUAL "" BOOL)
  set(_3rd_party_release_lib  "${_3rd_party_bin}/${release_lib_subpath}")
  set(_3rd_party_need_debug   NOT "${debug_lib_subpath}" STREQUAL "" BOOL)
  set(_3rd_party_debug_lib    "${_3rd_party_bin}/${debug_lib_subpath}")

  if(NOT EXISTS ${_3rd_party_include} OR
     (${_3rd_party_need_release} AND NOT EXISTS ${_3rd_party_release_lib}) OR
     (${_3rd_party_need_debug}   AND NOT EXISTS ${_3rd_party_debug_lib}))
    message("Building '${target_name}'. Please, be patient ...")
    file(REMOVE_RECURSE ${_3rd_party_dir})
    file(MAKE_DIRECTORY ${_3rd_party_dir})

    # Clone repo
    execute_process(COMMAND git clone ${repo_url} ${_3rd_party_dir} --quiet
                    RESULTS_VARIABLE _3rd_party_err_code)
    if(${_3rd_party_err_code})
      message(FATAL_ERROR "Failed to clone 3rd-party library: ${target_name}")
    endif()

    # Bind it to a revision that works fine
    execute_process(COMMAND git checkout ${commit_hash} --quiet
                    WORKING_DIRECTORY ${_3rd_party_dir}
                    RESULTS_VARIABLE _3rd_party_err_code)
    if(${_3rd_party_err_code})
      message(FATAL_ERROR "Failed to checkout 3rd-party library: ${target_name}")
    endif()

    # Compile the project in Debug and Release modes, extract libraries
    if(${_3rd_party_need_release} OR ${_3rd_party_need_debug})
      execute_process(COMMAND ${CMAKE_COMMAND}
                              -B "${_3rd_party_bin}"
                              -G ${CMAKE_GENERATOR}
                              -A ${CMAKE_GENERATOR_PLATFORM}
                              -DCMAKE_POLICY_DEFAULT_CMP0091=NEW
                              -DMSVC_RUNTIME_LIBRARY="MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
                              --no-warn-unused-cli
                      WORKING_DIRECTORY ${_3rd_party_dir}
                      RESULTS_VARIABLE _3rd_party_err_code
                      OUTPUT_QUIET)
      if(${_3rd_party_err_code})
        message(FATAL_ERROR "Failed to configure 3rd-party library: ${target_name}")
      endif()
    endif()

    if(${_3rd_party_need_release})
      execute_process(COMMAND ${CMAKE_COMMAND}
                              --build "${_3rd_party_bin}"
                              --config Release
                      WORKING_DIRECTORY ${_3rd_party_dir}
                      RESULTS_VARIABLE _3rd_party_err_code
                      OUTPUT_QUIET)
      if(${_3rd_party_err_code})
        message(FATAL_ERROR "Failed to compile 3rd-party library: ${target_name} (Release)")
      endif()
    endif()

    if(${_3rd_party_need_debug})
      execute_process(COMMAND ${CMAKE_COMMAND}
                              --build "${_3rd_party_bin}"
                              --config Debug
                      WORKING_DIRECTORY ${_3rd_party_dir}
                      RESULTS_VARIABLE _3rd_party_err_code
                      OUTPUT_QUIET)
      if(${_3rd_party_err_code})
        message(FATAL_ERROR "Failed to compile 3rd-party library: ${target_name} (Debug)")
      endif()
    endif()

    if(NOT EXISTS ${_3rd_party_include} OR
       (${_3rd_party_need_release} AND NOT EXISTS ${_3rd_party_release_lib}) OR
       (${_3rd_party_need_debug} AND NOT EXISTS ${_3rd_party_debug_lib}))
      message(FATAL_ERROR "Building rule for '${target_name}' is broken")
    endif()
  endif()

  if(${_3rd_party_need_release} OR ${_3rd_party_need_debug})
    add_library(${target_name} STATIC IMPORTED)
  else()
    add_library(${target_name} INTERFACE)
  endif()

  target_include_directories(${target_name} SYSTEM INTERFACE ${_3rd_party_include})
  if(${_3rd_party_need_release})
    set_target_properties(${target_name} PROPERTIES
                          IMPORTED_LOCATION_RELEASE ${_3rd_party_release_lib})
  endif()
  if(${_3rd_party_need_debug})
    set_target_properties(${target_name} PROPERTIES
                          IMPORTED_LOCATION_DEBUG ${_3rd_party_debug_lib})
  endif()
endmacro()

# GoogleTests
fetch_3rd_party("https://github.com/google/googletest.git"
                "2f3e2e39cc4c399b66711e6b720bf22373e841b5"
                googletest
                "googletest/include"
                "lib/Release/gtest.lib"
                "lib/Debug/gtestd.lib")
set(GTEST_ROOT  "${CMAKE_CURRENT_LIST_DIR}/googletest")
set(GTest_FOUND true)
include(GoogleTest)

# TinyXML2
fetch_3rd_party("https://github.com/leethomason/tinyxml2.git"
                "321ea883b7190d4e85cae5512a12e5eaa8f8731f"
                tinyxml2
                "."
                "Release/tinyxml2.lib"
                "Debug/tinyxml2.lib")

# GLM
fetch_3rd_party("https://github.com/g-truc/glm.git"
                "bf71a834948186f4097caa076cd2663c69a10e1e"
                glm
                "." "" "")


discover_cpp("${CMAKE_CURRENT_LIST_DIR}/TinyXML" 3RD_PARTY_H 3RD_PARTY_CPP)
add_library(3rdParty STATIC ${3RD_PARTY_H} ${3RD_PARTY_CPP})
configure_cpp(3rdParty "3rd-party")

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/DirectShow.dll
              ${SDK_DIR}/Bin/DirectShow.dll
              EXPECTED_HASH SHA256=A35437E5738B9F0DE014813919F3674B6C76EAE5B17FD01E5471EB73DA08206F)
file(COPY ${SDK_DIR}/Bin/DirectShow.dll DESTINATION ${WORKING_DIR})

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/GSSF.ax
              ${SDK_DIR}/Bin/GSSF.ax
              EXPECTED_HASH SHA256=F49BA427B354DB6852D44437338AA59ED52EAFF9CD16A8BDC36E571711649AE7)

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/SlimDX.dll
              ${SDK_DIR}/Bin/SlimDX.dll
              EXPECTED_HASH SHA256=B0C066F6755097E6DA1E4F54F8630272DE3A69AAF39824228EA8391817C38339)
file(COPY ${SDK_DIR}/Bin/SlimDX.dll DESTINATION ${WORKING_DIR})

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/SpriteTextRenderer.SlimDX.dll
              ${SDK_DIR}/Bin/SpriteTextRenderer.SlimDX.dll
              EXPECTED_HASH SHA256=B714B2E602EF67FA3280F0FD09BECDF3E01BD5E3984AF9931DE88C00A0D74015)
file(COPY ${SDK_DIR}/Bin/SpriteTextRenderer.SlimDX.dll DESTINATION ${WORKING_DIR})

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/SpriteTextRenderer.dll
              ${SDK_DIR}/Bin/SpriteTextRenderer.dll
              EXPECTED_HASH SHA256=943F0357FAD595F1F916B3E53CDC3955B44F3031F0972DE9E1099B818E4A362B)
file(COPY ${SDK_DIR}/Bin/SpriteTextRenderer.dll DESTINATION ${WORKING_DIR})

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/WindowsMediaLib.dll
              ${SDK_DIR}/Bin/WindowsMediaLib.dll
              EXPECTED_HASH SHA256=A96ED1DCB39B06CB37B6CD5834DFB782876753710EA72231DC085053CB92CD3F)
file(COPY ${SDK_DIR}/Bin/WindowsMediaLib.dll DESTINATION ${WORKING_DIR})
