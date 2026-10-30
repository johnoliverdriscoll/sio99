function(sio_add_test name library)
  add_executable(${name} ${name}.c)
  sio_target_defaults(${name})
  target_link_libraries(${name} PRIVATE ${library})
  add_test(NAME ${name} COMMAND ${name})
endfunction()

sio_add_test(test_sio sio99)

if(SIO99_WITH_STDIO)
  sio_add_test(test_sio_file_reader sio99_stdio)
endif()

if(SIO99_WITH_HEAP)
  sio_add_test(test_sio_dynamic_buffer_writer sio99_heap)
endif()
