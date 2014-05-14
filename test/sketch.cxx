#include "cwx/cwx.hxx"
#include "cwx/sketch.hxx"

int main() {
    typedef unsigned int Label;
    typedef unsigned int Coordinate;
    typedef andres::cwx::Cell<Coordinate> Cell;
    typedef andres::cwx::CWX<Label, Coordinate> CWX;

    size_t size[] = {4, 4, 4};
    andres::Marray<Label> seg(size, size + 3);

    for(size_t z = 0; z < 4; ++ z)
    for(size_t y = 0; y < 4; ++ y)
    for(size_t x = 0; x < 4; ++ x) {
        if(z < 2) {
            if(y < 2) {
                if(x < 2) {
                    seg(x, y, z) = 1;
                }
                else {
                    seg(x, y, z) = 1;
                }
            }
            else {
                if(x < 2) {
                    seg(x, y, z) = 1;
                }
                else {
                    seg(x, y, z) = 2;
                }
            }
        }
        else {
            if(y < 2) {
                if(x < 2) {
                    seg(x, y, z) = 2;
                }
                else {
                    seg(x, y, z) = 2;
                }
            }
            else {
                if(x < 2) {
                    seg(x, y, z) = 3;
                }
                else {
                    seg(x, y, z) = 3;
                }
            }
        }
    }

    CWX cwx;
    cwx.build(seg);

    // export sketch
    andres::cwx::Sketch<Coordinate> sketch;
    for(unsigned char order = 0; order < 3; ++order) {
        for(Label label = 1; label <= cwx.numberOfCells(order); ++label) {
            cwx.process(order, label, sketch);
        }
    }

    return 0;
}
