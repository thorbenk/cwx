add_executable(bench-cwx cwx.cxx)
set_target_properties(bench-cwx PROPERTIES COMPILE_FLAGS -g)
target_link_libraries(bench-cwx ${HDF5_LIBRARIES})

