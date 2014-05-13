add_executable(anchorage anchorage.cxx)
add_test(NAME anchorage COMMAND anchorage)

add_executable(byte-labeled-cellgrid byte-labeled-cellgrid.cxx)
add_test(NAME byte-labeled-cellgrid COMMAND byte-labeled-cellgrid)

add_executable(cell cell.cxx)
add_test(NAME cell COMMAND cell)

add_executable(cellgrid cellgrid.cxx)
add_test(NAME cellgrid COMMAND cellgrid)

add_executable(cwcomplex cwcomplex.cxx)
add_test(NAME cwcomplex COMMAND cwcomplex)

add_executable(cwx cwx.cxx)
add_test(NAME cwx COMMAND cwx)

add_executable(cwx-with-data cwx-with-data.cxx)
target_link_libraries(cwx-with-data ${HDF5_LIBRARIES})

add_executable(latex latex.cxx)

add_executable(sketch sketch.cxx)

