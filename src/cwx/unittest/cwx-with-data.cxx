#include <string>
#include <random>
#include <fstream>

#include "andres/marray_hdf5.hxx"
#include "andres/cwx/cwx.hxx"
#include "andres/cwx/sketch.hxx"

inline void test(const bool& pred) {
    if(!pred) throw std::runtime_error("Test failed.");
}

int main(int argc, char **argv) {
    if(argc != 3) {
        std::cerr << "Parameters: <input-hdf5-file> <input-dataset>" << std::endl;
        return 1;
    }
    std::string fileName = argv[1];
    std::string datasetName = argv[2];

    typedef unsigned int Label;
    typedef unsigned int Coordinate;

    // load volume labeling
    andres::Marray<Label> volumeLabeling;
    hid_t file = andres::hdf5::openFile(fileName);
    andres::hdf5::load(file, datasetName, volumeLabeling);
    andres::hdf5::closeFile(file);

    // build CWX data structure
    andres::cwx::CWX<Label, Coordinate> cwx;
    cwx.build(volumeLabeling);

    // TODO: add tests here

    return 0;
}

