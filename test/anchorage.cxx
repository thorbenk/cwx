#include <stdexcept>

#include "cwx/anchorage.hxx"

inline void test(const bool& pred) {
    if(!pred) throw std::runtime_error("Test failed.");
}

int main() {
    typedef unsigned int Label;
    typedef andres::cwx::Anchorage<Label, unsigned int> Anchorage;
    typedef Anchorage::CellType CellType;

    Anchorage anchorage;
    test(anchorage.numberOfCells(0) == 0);
    test(anchorage.numberOfCells(1) == 0);
    test(anchorage.numberOfCells(2) == 0);
    test(anchorage.numberOfCells(3) == 0);

    {
        Label l = 0;
        CellType c;
        c[0] = 2; c[1] = 4; c[2] = 6;
        test(anchorage.anchor(c) == 0);
        l = anchorage.push_back(c);
        test(l == 1);
        test(anchorage.anchor(c) == l);
        test(anchorage.numberOfCells(3) == 1);

        c[0] = 6; c[1] = 4; c[2] = 2;
        test(anchorage.anchor(c) == 0);
        l = anchorage.push_back(c);
        test(l == 2);
        test(anchorage.anchor(c) == l);
        test(anchorage.numberOfCells(3) == 2);

        c[0] = 2; c[1] = 2; c[2] = 1;
        test(anchorage.anchor(c) == 0);
        l = anchorage.push_back(c);
        test(l == 1);
        test(anchorage.anchor(c) == l);
        test(anchorage.numberOfCells(2) == 1);

        c[0] = 2; c[1] = 1; c[2] = 1;
        test(anchorage.anchor(c) == 0);
        l = anchorage.push_back(c);
        test(l == 1);
        test(anchorage.anchor(c) == l);
        test(anchorage.numberOfCells(1) == 1);

        c[0] = 1; c[1] = 1; c[2] = 1;
        test(anchorage.anchor(c) == 0);
        l = anchorage.push_back(c);
        test(l == 1);
        test(anchorage.anchor(c) == l);
        test(anchorage.numberOfCells(0) == 1);
    }
    {
        CellType c;
        anchorage.anchor(3, 1, c);
        test(c[0] == 2 && c[1] == 4 && c[2] == 6);
        anchorage.anchor(3, 2, c);
        test(c[0] == 6 && c[1] == 4 && c[2] == 2);
        anchorage.anchor(2, 1, c);
        test(c[0] == 2 && c[1] == 2 && c[2] == 1);
        anchorage.anchor(1, 1, c);
        test(c[0] == 2 && c[1] == 1 && c[2] == 1);
        anchorage.anchor(0, 1, c);
        test(c[0] == 1 && c[1] == 1 && c[2] == 1);
    }

    return 0;
}
