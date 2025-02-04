
project(vt_vox)
cmake_policy(SET CMP0072 NEW)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")


if (WIN32)
    set(LIBS_DIR "E:/windows/libs")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -D_CRT_SECURE_NO_WARNINGS")
    if (MSVC)
        if(CMAKE_BUILD_TYPE MATCHES Debug)
            add_compile_options(/RTC1 /Od /Zi)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
        endif()     
    endif()

endif() 
  

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_compile_options(
    -Wall 
    -Wextra 
    -Werror 
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-but-set-variable
    -Wno-unused-result
)


file(GLOB SOURCES "src/*.cpp")
add_executable(vt_vox  ${SOURCES})

if(CMAKE_BUILD_TYPE MATCHES Debug)

    if (UNIX)
        target_compile_options(vt_vox PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak   -fno-omit-frame-pointer -g  -D_DEBUG )
        target_link_options(vt_vox PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak    -D_DEBUG) 
    endif()


elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(vt_vox PRIVATE -O3   -DNDEBUG )
    target_link_options(vt_vox PRIVATE -O3   -DNDEBUG )
endif()

find_package(OpenGL REQUIRED)

 
if (WIN32)
    target_link_libraries(vt_vox core m  "${LIBS_DIR}/lib/x64/SDL2main.lib" "${LIBS_DIR}/lib/x64/SDL2.lib"  Winmm.lib opengl32.lib)
endif()

if (UNIX)
    target_link_libraries(vt_vox
    PRIVATE
        core
        m
        SDL2
        OpenGL::GL
)
endif()
