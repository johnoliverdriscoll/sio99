# Tests

Configure, build, and run the tests from the repository root:

```sh
cmake -S . -B build -DSIO99_BUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

The core tests cover interface dispatch, callback error propagation, buffer
reading, EOF and seek boundaries, and fixed-capacity writing. The adapter tests
cover file reading and seeking, dynamic buffer growth, binary and string copies,
reset, and deinitialization. Checks remain active in release builds.

The file-reader and dynamic-buffer-writer tests are enabled by
`SIO99_WITH_STDIO` and `SIO99_WITH_HEAP`, respectively. Each adapter test links
only its adapter target, exercising its public dependency on the core library.

To test the core without either optional adapter:

```sh
cmake -S . -B build-core -DSIO99_BUILD_TESTING=ON \
  -DSIO99_WITH_STDIO=OFF -DSIO99_WITH_HEAP=OFF
cmake --build build-core
ctest --test-dir build-core --output-on-failure
```
