add_executable(test-anchorage anchorage.cxx)
add_test(NAME test-anchorage COMMAND test-anchorage)

add_executable(test-byte-labeled-cellgrid byte-labeled-cellgrid.cxx)
add_test(NAME test-byte-labeled-cellgrid COMMAND test-byte-labeled-cellgrid)

add_executable(test-cell cell.cxx)
add_test(NAME test-cell COMMAND test-cell)

add_executable(test-cellgrid cellgrid.cxx)
add_test(NAME test-cellgrid COMMAND test-cellgrid)

add_executable(test-cwcomplex cwcomplex.cxx)
add_test(NAME test-cwcomplex COMMAND test-cwcomplex)

add_executable(test-cwx cwx.cxx)
add_test(NAME test-cwx COMMAND test-cwx)

add_executable(test-cwx-with-data cwx-with-data.cxx)
target_link_libraries(test-cwx-with-data ${HDF5_LIBRARIES})

add_executable(test-latex latex.cxx)

add_executable(test-sketch sketch.cxx)

