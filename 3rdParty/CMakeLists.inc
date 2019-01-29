set(CURRENT_FILTER "")

discover_cpp(${CMAKE_CURRENT_LIST_DIR} 3RD_PARTY_H 3RD_PARTY_CPP)
add_library(3rdParty STATIC ${3RD_PARTY_H} ${3RD_PARTY_CPP})
configure_cpp(3rdParty "3rd-party")

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/DirectShow.dll
              ${CMAKE_SOURCE_DIR}/SDK/Bin/DirectShow.dll
              EXPECTED_HASH SHA256=A35437E5738B9F0DE014813919F3674B6C76EAE5B17FD01E5471EB73DA08206F)

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/GSSF.ax
              ${CMAKE_SOURCE_DIR}/SDK/Bin/GSSF.ax
              EXPECTED_HASH SHA256=F49BA427B354DB6852D44437338AA59ED52EAFF9CD16A8BDC36E571711649AE7)

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/SlimDX.dll
              ${CMAKE_SOURCE_DIR}/SDK/Bin/SlimDX.dll
              EXPECTED_HASH SHA256=B0C066F6755097E6DA1E4F54F8630272DE3A69AAF39824228EA8391817C38339)

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/SpriteTextRenderer.SlimDX.dll
              ${CMAKE_SOURCE_DIR}/SDK/Bin/SpriteTextRenderer.SlimDX.dll
              EXPECTED_HASH SHA256=B714B2E602EF67FA3280F0FD09BECDF3E01BD5E3984AF9931DE88C00A0D74015)

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/SpriteTextRenderer.dll
              ${CMAKE_SOURCE_DIR}/SDK/Bin/SpriteTextRenderer.dll
              EXPECTED_HASH SHA256=943F0357FAD595F1F916B3E53CDC3955B44F3031F0972DE9E1099B818E4A362B)

file(DOWNLOAD http://ttglabs.com/static_data/mitosis/3rd-party/WindowsMediaLib.dll
              ${CMAKE_SOURCE_DIR}/SDK/Bin/WindowsMediaLib.dll
              EXPECTED_HASH SHA256=A96ED1DCB39B06CB37B6CD5834DFB782876753710EA72231DC085053CB92CD3F)
