#include <string>
#include <random>
#include <fstream>

#include "andres/marray_hdf5.hxx"
#include "cwx/cwx.hxx"
#include "cwx/sketch.hxx"

#ifdef USE_VIGRA_MULTIARRAY
#include <vigra/hdf5impex.hxx>
#endif

inline void test(const bool& pred) {
    if(!pred) throw std::runtime_error("Test failed.");
}

int main(int argc, char **argv) {
    using namespace andres;
    
    if(argc != 3) {
        std::cerr << "Parameters: <input-hdf5-file> <input-dataset>" << std::endl;
        return 1;
    }
    std::string fileName = argv[1];
    std::string datasetName = argv[2];

    typedef unsigned int Label;
    typedef unsigned int Coordinate;

    // load volume labeling
    Marray<Label> volumeLabeling;
    hid_t file = hdf5::openFile(fileName);
    hdf5::load(file, datasetName, volumeLabeling);
    hdf5::closeFile(file);

    // build CWX data structure
    cwx::CWX<Label, Coordinate> cwx;
    cwx.build(volumeLabeling, true);
    
    // TODO: add tests here
   
#ifndef USE_VIGRA_MULTIARRAY
    hid_t outFile(hdf5::createFile("out.h5"));
    hdf5::save(outFile, "cwx", cwx.grid());
    hdf5::closeFile(outFile);
#else
    using vigra::HDF5File;
    HDF5File f("out.h5", HDF5File::Open);
    f.write("cwx", cwx.grid());
    f.close();
#endif

    return 0;
}


