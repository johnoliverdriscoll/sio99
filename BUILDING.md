# Building and integrating sio99

## Integration

Add sio99 to a CMake project and link the required target:

```cmake
add_subdirectory(src/sio99)
target_link_libraries(my_application PRIVATE sio99)
```

| Target | Provides |
| --- | --- |
| `sio99` | Reader and writer interfaces, buffer reader, fixed-capacity writer |
| `sio99_stdio` | File reader |
| `sio99_heap` | Dynamic buffer writer |

Both optional targets publicly link `sio99`.

## Build options

Set options when configuring, such as `-DSIO99_WITH_HEAP=OFF`.

| Option | Default | Purpose |
| --- | --- | --- |
| `BUILD_SHARED_LIBS` | `OFF` | Build shared libraries instead of static libraries |
| `SIO99_BUILD_TESTING` | `ON` for standalone builds | Build and register tests with CTest |
| `SIO99_WITH_STDIO` | `ON` | Build the file reader |
| `SIO99_WITH_HEAP` | `ON` | Build the dynamic buffer writer |

Install a completed build with `cmake --install sio99/build`. Set
`CMAKE_INSTALL_PREFIX` during configuration to choose the installation location.
Installed consumers can use `pkg-config --cflags --libs sio99`.
