#include <stdexcept>
#include <iostream>

#include "cwx/cwcomplex.hxx"

inline void test(const bool& pred) {
    if(!pred) throw std::runtime_error("Test failed.");
}

int main() {
    typedef unsigned int Label;
    typedef andres::cwx::CWComplex<Label> CWComplex;

    {
        // construction of empty CW-complex
        CWComplex complex;
        test(complex.numberOfCells(0) == 0);
        test(complex.numberOfCells(1) == 0);
        test(complex.numberOfCells(2) == 0);
        test(complex.numberOfCells(3) == 0);

        // push_back
        for(Label j = 0; j < 4; ++j) {
            Label p = complex.push_back(j);
            test(p == 1);
            test(complex.numberOfCells(j) == 1);
            p = complex.push_back(j);
            test(p == 2);
            test(complex.numberOfCells(j) == 2);
        }
    }

    {
        CWComplex complex(1, 4, 6, 4);

        test(complex.numberOfCells(0) == 1);
        test(complex.numberOfCells(1) == 4);
        test(complex.numberOfCells(2) == 6);
        test(complex.numberOfCells(3) == 4);

        complex.connect(0, 1, 1);
        complex.connect(0, 1, 2);
        complex.connect(0, 1, 4);
        complex.connect(0, 1, 3);
        complex.connect(1, 1, 4);
        complex.connect(1, 1, 5);
        complex.connect(1, 1, 3);
        complex.connect(1, 2, 3);
        complex.connect(1, 2, 1);
        complex.connect(1, 2, 2);
        complex.connect(1, 3, 2);
        complex.connect(1, 3, 4);
        complex.connect(1, 3, 6);
        complex.connect(1, 4, 6);
        complex.connect(1, 4, 5);
        complex.connect(1, 4, 1);
        complex.connect(2, 1, 1);
        complex.connect(2, 1, 2);
        complex.connect(2, 2, 3);
        complex.connect(2, 2, 1);
        complex.connect(2, 3, 3);
        complex.connect(2, 3, 2);
        complex.connect(2, 4, 3);
        complex.connect(2, 4, 4);
        complex.connect(2, 5, 2);
        complex.connect(2, 5, 4);
        complex.connect(2, 6, 4);
        complex.connect(2, 6, 1);

        test(complex.numberOfCells(0) == 1);
        test(complex.numberOfCells(1) == 4);
        test(complex.numberOfCells(2) == 6);
        test(complex.numberOfCells(3) == 4);

        test(complex.sizeAbove(0, 1) == 4);
        test(complex.sizeAbove(1, 1) == 3);
        test(complex.sizeAbove(1, 2) == 3);
        test(complex.sizeAbove(1, 3) == 3);
        test(complex.sizeAbove(1, 4) == 3);
        test(complex.sizeAbove(2, 1) == 2);
        test(complex.sizeAbove(2, 2) == 2);
        test(complex.sizeAbove(2, 3) == 2);
        test(complex.sizeAbove(2, 4) == 2);
        test(complex.sizeAbove(2, 5) == 2);
        test(complex.sizeAbove(2, 6) == 2);
        test(complex.sizeAbove(3, 1) == 0);
        test(complex.sizeAbove(3, 2) == 0);
        test(complex.sizeAbove(3, 3) == 0);
        test(complex.sizeAbove(3, 4) == 0);

        test(complex.sizeBelow(0, 1) == 0);
        test(complex.sizeBelow(1, 1) == 1);
        test(complex.sizeBelow(1, 2) == 1);
        test(complex.sizeBelow(1, 3) == 1);
        test(complex.sizeBelow(1, 4) == 1);
        test(complex.sizeBelow(2, 1) == 2);
        test(complex.sizeBelow(2, 2) == 2);
        test(complex.sizeBelow(2, 3) == 2);
        test(complex.sizeBelow(2, 4) == 2);
        test(complex.sizeBelow(2, 5) == 2);
        test(complex.sizeBelow(2, 6) == 2);
        test(complex.sizeBelow(3, 1) == 3);
        test(complex.sizeBelow(3, 2) == 3);
        test(complex.sizeBelow(3, 3) == 3);
        test(complex.sizeBelow(3, 4) == 3);

        test(complex.above(0, 1, 0) == 1);
        test(complex.above(0, 1, 1) == 2);
        test(complex.above(0, 1, 2) == 3);
        test(complex.above(0, 1, 3) == 4);
        test(complex.above(0, 1, 4) == 0);
        test(complex.above(0, 1, 5) == 0);
        test(complex.above(1, 1, 0) == 3);
        test(complex.above(1, 1, 1) == 4);
        test(complex.above(1, 1, 2) == 5);
        test(complex.above(1, 1, 3) == 0);
        test(complex.above(1, 2, 0) == 1);
        test(complex.above(1, 2, 1) == 2);
        test(complex.above(1, 2, 2) == 3);
        test(complex.above(1, 2, 3) == 0);
        test(complex.above(1, 3, 0) == 2);
        test(complex.above(1, 3, 1) == 4);
        test(complex.above(1, 3, 2) == 6);
        test(complex.above(1, 3, 3) == 0);
        test(complex.above(1, 4, 0) == 1);
        test(complex.above(1, 4, 1) == 5);
        test(complex.above(1, 4, 2) == 6);
        test(complex.above(1, 4, 3) == 0);
        test(complex.above(2, 1, 0) == 1);
        test(complex.above(2, 1, 1) == 2);
        test(complex.above(2, 2, 0) == 1);
        test(complex.above(2, 2, 1) == 3);
        test(complex.above(2, 3, 0) == 2);
        test(complex.above(2, 3, 1) == 3);
        test(complex.above(2, 4, 0) == 3);
        test(complex.above(2, 4, 1) == 4);
        test(complex.above(2, 5, 0) == 2);
        test(complex.above(2, 5, 1) == 4);
        test(complex.above(2, 6, 0) == 1);
        test(complex.above(2, 6, 1) == 4);

        test(complex.below(1, 1, 0) == 1);
        test(complex.below(1, 2, 0) == 1);
        test(complex.below(1, 3, 0) == 1);
        test(complex.below(1, 4, 0) == 1);

        test(complex.below(2, 1, 0) == 2);
        test(complex.below(2, 1, 1) == 4);
        test(complex.below(2, 2, 0) == 2);
        test(complex.below(2, 2, 1) == 3);
        test(complex.below(2, 3, 0) == 1);
        test(complex.below(2, 3, 1) == 2);
        test(complex.below(2, 4, 0) == 1);
        test(complex.below(2, 4, 1) == 3);
        test(complex.below(2, 5, 0) == 1);
        test(complex.below(2, 5, 1) == 4);
        test(complex.below(2, 6, 0) == 3);
        test(complex.below(2, 6, 1) == 4);
        test(complex.below(3, 1, 0) == 1);
        test(complex.below(3, 1, 1) == 2);
        test(complex.below(3, 1, 2) == 6);
        test(complex.below(3, 2, 0) == 1);
        test(complex.below(3, 2, 1) == 3);
        test(complex.below(3, 2, 2) == 5);
        test(complex.below(3, 3, 0) == 2);
        test(complex.below(3, 3, 1) == 3);
        test(complex.below(3, 3, 2) == 4);
        test(complex.below(3, 4, 0) == 4);
        test(complex.below(3, 4, 1) == 5);
        test(complex.below(3, 4, 2) == 6);
    }

    return 0;
}
