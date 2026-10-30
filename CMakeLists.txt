cmake_minimum_required(VERSION 3.22)

project(sio99
  VERSION 0.1.0
  LANGUAGES C
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(SIO99_BUILD_TESTING "Build tests" ${PROJECT_IS_TOP_LEVEL})
option(SIO99_WITH_STDIO "Build the file reader" ON)
option(SIO99_WITH_HEAP "Build the dynamic buffer writer" ON)

function(sio_target_defaults target)

  set_target_properties(${target}
    PROPERTIES
      C_STANDARD 99
      C_STANDARD_REQUIRED ON
      C_EXTENSIONS OFF
  )

  target_include_directories(${target} PUBLIC include)

  target_compile_options(${target}
    PRIVATE
      "$<$<COMPILE_LANG_AND_ID:C,GNU,Clang,AppleClang>:"
        "-Werror;"
        "-Wpedantic;"
        "-Wall;"
        "-Wextra;"
        "-Werror=vla;"
        "-Werror=alloca;"
        "-Werror=implicit-function-declaration;"
        "-Wstrict-prototypes;"
        "-Wmissing-prototypes;"
        "-fno-fast-math;"
        "-ffp-contract=off;"
      ">"
  )

endfunction()

add_library(sio99
  src/sio99/sio.c
  src/sio99/sio_buffer_reader.c
  src/sio99/sio_buffer_writer.c
)
sio_target_defaults(sio99)
set_target_properties(sio99
  PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION 0
)

if(SIO99_WITH_STDIO)
  add_library(sio99_stdio
    src/sio99/sio_file_reader.c
  )
  sio_target_defaults(sio99_stdio)
  target_link_libraries(sio99_stdio
    PUBLIC
      sio99
  )
  set_target_properties(sio99_stdio
    PROPERTIES
      VERSION "${PROJECT_VERSION}"
      SOVERSION 0
  )
endif()

if(SIO99_WITH_HEAP)
  add_library(sio99_heap
    src/sio99/sio_dynamic_buffer_writer.c
  )
  sio_target_defaults(sio99_heap)
  target_link_libraries(sio99_heap
    PUBLIC
      sio99
  )
  set_target_properties(sio99_heap
    PROPERTIES
      VERSION "${PROJECT_VERSION}"
      SOVERSION 0
  )
endif()

set(SIO99_PKGCONFIG_LIBS "")
if(SIO99_WITH_STDIO)
  string(APPEND SIO99_PKGCONFIG_LIBS "-lsio99_stdio ")
endif()
if(SIO99_WITH_HEAP)
  string(APPEND SIO99_PKGCONFIG_LIBS "-lsio99_heap ")
endif()
string(APPEND SIO99_PKGCONFIG_LIBS "-lsio99")

include(GNUInstallDirs)

install(
  TARGETS sio99
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)
install(
  FILES
    include/sio99/sio.h
    include/sio99/sio_buffer_reader.h
    include/sio99/sio_buffer_writer.h
  DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/sio99"
)

if(SIO99_WITH_STDIO)
  install(
    TARGETS sio99_stdio
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )
  install(
    FILES include/sio99/sio_file_reader.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/sio99"
  )
endif()

if(SIO99_WITH_HEAP)
  install(
    TARGETS sio99_heap
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
  )
  install(
    FILES include/sio99/sio_dynamic_buffer_writer.h
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/sio99"
  )
endif()

set(SIO99_PKGCONFIG_LIBS "")
if(SIO99_WITH_STDIO)
  string(APPEND SIO99_PKGCONFIG_LIBS "-lsio99_stdio ")
endif()
if(SIO99_WITH_HEAP)
  string(APPEND SIO99_PKGCONFIG_LIBS "-lsio99_heap ")
endif()
string(APPEND SIO99_PKGCONFIG_LIBS "-lsio99")

configure_file(sio99.pc.in sio99.pc @ONLY)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/sio99.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)

if(SIO99_BUILD_TESTING)
  enable_testing()
  add_subdirectory(tests)
endif()
