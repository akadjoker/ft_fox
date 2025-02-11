project(core)

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

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

 
file(GLOB SOURCES "src/*.cpp")
add_library(core STATIC  ${SOURCES})
add_compile_options(
    -Wall 
    -Wextra 
    -Werror 
    -Wno-unused-parameter
    -Wno-unused-variable
    -Wno-unused-but-set-variable
    -Wno-unused-result
)

 
target_include_directories(core PUBLIC "${LIBS_DIR}/include" include include/std src)




if(CMAKE_BUILD_TYPE MATCHES Debug)

if (UNIX)
    target_compile_options(core PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak  -fno-omit-frame-pointer  -g -Winvalid-pch -D_DEBUG)
    target_link_options(core PRIVATE -fsanitize=address -fsanitize=undefined -fsanitize=leak -D_DEBUG) 
endif()

elseif(CMAKE_BUILD_TYPE MATCHES Release)
    target_compile_options(core PRIVATE -O3 -march=native -flto -funroll-loops -DNDEBUG)
    target_link_options(core PRIVATE -O3 -march=native -flto -funroll-loops -DNDEBUG)
endif()