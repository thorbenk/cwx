#include <stdexcept>
#include <random>

#include "cwx/byte-labeled-cellgrid.hxx"

inline void test(const bool& pred) {
    if(!pred) throw std::runtime_error("Test failed.");
}

int main() {
    typedef unsigned int Label;
    typedef unsigned int Coordinate;
    typedef andres::cwx::ByteLabeledCellgrid<Label, Coordinate> ByteLabeledCellgrid;
    typedef ByteLabeledCellgrid::Order Order;
    typedef ByteLabeledCellgrid::CellType Cell;
    typedef ByteLabeledCellgrid::CellVector CellVector;

    {
        // construction of empty grid
        ByteLabeledCellgrid grid;
        test(grid.shape(0) == 0);
        test(grid.shape(1) == 0);
        test(grid.shape(2) == 0);

        // resize of empty grid
        grid.resize(2, 1, 3);
        test(grid.shape(0) == 2);
        test(grid.shape(1) == 1);
        test(grid.shape(2) == 3);

        // resize of non-empty grid
        grid.resize(1, 4, 3);
        test(grid.shape(0) == 1);
        test(grid.shape(1) == 4);
        test(grid.shape(2) == 3);

        std::string s = grid.asString();
    }

    {
        // construction of non-empty grid
        ByteLabeledCellgrid grid(3, 2, 4);
        test(grid.shape(0) == 3);
        test(grid.shape(1) == 2);
        test(grid.shape(2) == 4);
        test(grid.numberOfCells(0) == 2*1*3);
        test(grid.numberOfCells(1) == 2*1*4 + 2*2*3 + 3*1*3);
        test(grid.numberOfCells(2) == 3*2*3 + 3*1*4 + 2*2*4);
        test(grid.numberOfCells(3) == 3*2*4);
    }

    // first cell
    {
        ByteLabeledCellgrid grid(5, 5, 5);
        Cell c(20, 20, 20);
        bool response = false;

        response = grid.firstCell(0, c);
        test(response == true);
        test(c[0] == 1 && c[1] == 1 && c[2] == 1);

        response = grid.firstCell(1, c);
        test(response == true);
        test(c[0] == 1 && c[1] == 1 && c[2] == 0);

        response = grid.firstCell(2, c);
        test(response == true);
        test(c[0] == 1 && c[1] == 0 && c[2] == 0);

        response = grid.firstCell(3, c);
        test(response == true);
        test(c[0] == 0 && c[1] == 0 && c[2] == 0);
    }

    // firstCell, orderPreservingIncrement, mark, isMarked, anchor, isAnchored
    size_t counters[] = {4*4*4, (4*4)*5*3, (5*5)*4*3 , 5*5*5};
    for(unsigned char order = 0; order < 3; ++order) {
        size_t counter = 0;
        ByteLabeledCellgrid grid(5, 5, 5);
        Cell c;
        if(grid.firstCell(order, c)) {
            do {
                test(c.order() == order);
                test(!grid.isMarked(c));
                grid.mark(c, true);
                test(grid.isMarked(c));
                grid.anchor(c, true);
                test(grid.isAnchored(c));
                ++counter;
            } while(grid.orderPreservingIncrement(c));
        }
        test(counter == counters[order]);
        for(c[0]=0; c[0]<grid.shape(0) * 2 - 1; ++c[0]) {
            for(c[1]=0; c[1]<grid.shape(1) * 2 - 1; ++c[1]) {
                for(c[2]=0; c[2]<grid.shape(2) * 2 - 1; ++c[2]) {
                    if(c.order() == order) {
                        test(grid.isMarked(c));
                        test(grid.isAnchored(c));
                    }
                    else if(c.order() != 3) { // used for anchoring
                        test(!grid.isMarked(c));
                    }
                }
            }
        }
    }

    // firstCell, orderPreservingIncrement with fixed dimension
    {
        const Coordinate s = 5;
        for(Order d=0; d<3; ++d) {
            for(Coordinate v=0; v<2*s-1; ++v) {
                for(unsigned char order = 0; order < 4; ++order) {
                    ByteLabeledCellgrid grid(s, s, s);
                    Cell c;
                    // mark all cells of order 'order' with x_d = v
                    if(grid.firstCell(order, d, v, c)) {
                        do {
                            grid.mark(c, true);
                        } while(grid.orderPreservingIncrement(d, c));
                    }
                    // test if exactly the correct cells have been marked
                    if(grid.firstCell(order, c)) {
                        do {
                            if(c.order() == order && c[d] == v) {
                                test(grid.isMarked(c));
                            }
                            else {
                                test(!grid.isMarked(c));
                            }
                        } while(grid.orderPreservingIncrement(c));
                    }
                }
            }
        }
    }

    // above
    {
        ByteLabeledCellgrid grid(10, 20, 30);
        // 3-cell
        {
            Cell c(2,4,6);
            CellVector v;
            grid.above(c, v);
            assert(v.size() == 0);
        }
        // 2-cell
        {
            Cell c(2,4,7);
            CellVector v;
            grid.above(c, v);
            assert(v.size() == 2);
            assert(v[0][0] == 2);
            assert(v[0][1] == 4);
            assert(v[0][2] == 6);
            assert(v[1][0] == 2);
            assert(v[1][1] == 4);
            assert(v[1][2] == 8);
        }
        {
            Cell c(2,5,6);
            CellVector v;
            grid.above(c, v);
            assert(v.size() == 2);
            assert(v[0][0] == 2);
            assert(v[0][1] == 4);
            assert(v[0][2] == 6);
            assert(v[1][0] == 2);
            assert(v[1][1] == 6);
            assert(v[1][2] == 6);
        }
        {
            Cell c(3,4,6);
            CellVector v;
            grid.above(c, v);
            assert(v.size() == 2);
            assert(v[0][0] == 2);
            assert(v[0][1] == 4);
            assert(v[0][2] == 6);
            assert(v[1][0] == 4);
            assert(v[1][1] == 4);
            assert(v[1][2] == 6);
        }
        // 1-cell
        {
            Cell c(0,1,1);
            CellVector v;
            grid.above(c, v);
            assert(v.size() == 4);
            assert(v[0][0] == 0);
            assert(v[0][1] == 0);
            assert(v[0][2] == 1);
            assert(v[1][0] == 0);
            assert(v[1][1] == 2);
            assert(v[1][2] == 1);
            assert(v[2][0] == 0);
            assert(v[2][1] == 1);
            assert(v[2][2] == 0);
            assert(v[3][0] == 0);
            assert(v[3][1] == 1);
            assert(v[3][2] == 2);
        }
        {
            Cell c(1,0,1);
            CellVector v;
            grid.above(c, v);
            assert(v.size() == 4);
            assert(v[0][0] == 0);
            assert(v[0][1] == 0);
            assert(v[0][2] == 1);
            assert(v[1][0] == 2);
            assert(v[1][1] == 0);
            assert(v[1][2] == 1);
            assert(v[2][0] == 1);
            assert(v[2][1] == 0);
            assert(v[2][2] == 0);
            assert(v[3][0] == 1);
            assert(v[3][1] == 0);
            assert(v[3][2] == 2);
        }
        {
            Cell c(1,1,0);
            CellVector v;
            grid.above(c, v);
            assert(v.size() == 4);
            assert(v[0][0] == 0);
            assert(v[0][1] == 1);
            assert(v[0][2] == 0);
            assert(v[1][0] == 2);
            assert(v[1][1] == 1);
            assert(v[1][2] == 0);
            assert(v[2][0] == 1);
            assert(v[2][1] == 0);
            assert(v[2][2] == 0);
            assert(v[3][0] == 1);
            assert(v[3][1] == 2);
            assert(v[3][2] == 0);
        }
        // 0-cell
        {
            Cell c(1,3,5);
            CellVector v;
            grid.above(c, v);
            assert(v.size() == 6);
            assert(v[0][0] == 0);
            assert(v[0][1] == 3);
            assert(v[0][2] == 5);
            assert(v[1][0] == 2);
            assert(v[1][1] == 3);
            assert(v[1][2] == 5);
            assert(v[2][0] == 1);
            assert(v[2][1] == 2);
            assert(v[2][2] == 5);
            assert(v[3][0] == 1);
            assert(v[3][1] == 4);
            assert(v[3][2] == 5);
            assert(v[4][0] == 1);
            assert(v[4][1] == 3);
            assert(v[4][2] == 4);
            assert(v[5][0] == 1);
            assert(v[5][1] == 3);
            assert(v[5][2] == 6);
        }
    }
    // below
    {
        ByteLabeledCellgrid grid(10, 20, 30);
        // 3-cell in the middle
        {
            Cell c(2,4,6);
            CellVector v;
            grid.below(c, v);
            assert(v.size() == 6);
            assert(v[0][0] == 1);
            assert(v[0][1] == 4);
            assert(v[0][2] == 6);
            assert(v[1][0] == 3);
            assert(v[1][1] == 4);
            assert(v[1][2] == 6);
            assert(v[2][0] == 2);
            assert(v[2][1] == 3);
            assert(v[2][2] == 6);
            assert(v[3][0] == 2);
            assert(v[3][1] == 5);
            assert(v[3][2] == 6);
            assert(v[4][0] == 2);
            assert(v[4][1] == 4);
            assert(v[4][2] == 5);
            assert(v[5][0] == 2);
            assert(v[5][1] == 4);
            assert(v[5][2] == 7);
        }
        // 3-cell at lower corner
        {
            Cell c(0,0,0);
            CellVector v;
            grid.below(c, v);
            assert(v.size() == 3);
            assert(v[0][0] == 1);
            assert(v[0][1] == 0);
            assert(v[0][2] == 0);
            assert(v[1][0] == 0);
            assert(v[1][1] == 1);
            assert(v[1][2] == 0);
            assert(v[2][0] == 0);
            assert(v[2][1] == 0);
            assert(v[2][2] == 1);
        }
        // 3-cell at upper corner
        {
            Cell c(18,38,58);
            CellVector v;
            grid.below(c, v);
            assert(v.size() == 3);
            assert(v[0][0] == 17);
            assert(v[0][1] == 38);
            assert(v[0][2] == 58);
            assert(v[1][0] == 18);
            assert(v[1][1] == 37);
            assert(v[1][2] == 58);
            assert(v[2][0] == 18);
            assert(v[2][1] == 38);
            assert(v[2][2] == 57);
        }
        // 2-cell in the middle
        {
            Cell c(5,6,8);
            CellVector v;
            grid.below(c, v);
            assert(v.size() == 4);
            assert(v[0][0] == 5);
            assert(v[0][1] == 5);
            assert(v[0][2] == 8);
            assert(v[1][0] == 5);
            assert(v[1][1] == 7);
            assert(v[1][2] == 8);
            assert(v[2][0] == 5);
            assert(v[2][1] == 6);
            assert(v[2][2] == 7);
            assert(v[3][0] == 5);
            assert(v[3][1] == 6);
            assert(v[3][2] == 9);
        }
        {
            Cell c(4,5,6);
            CellVector v;
            grid.below(c, v);
            assert(v.size() == 4);
            assert(v[0][0] == 3);
            assert(v[0][1] == 5);
            assert(v[0][2] == 6);
            assert(v[1][0] == 5);
            assert(v[1][1] == 5);
            assert(v[1][2] == 6);
            assert(v[2][0] == 4);
            assert(v[2][1] == 5);
            assert(v[2][2] == 5);
            assert(v[3][0] == 4);
            assert(v[3][1] == 5);
            assert(v[3][2] == 7);
        }
        {
            Cell c(4,6,5);
            CellVector v;
            grid.below(c, v);
            assert(v.size() == 4);
            assert(v[0][0] == 3);
            assert(v[0][1] == 6);
            assert(v[0][2] == 5);
            assert(v[1][0] == 5);
            assert(v[1][1] == 6);
            assert(v[1][2] == 5);
            assert(v[2][0] == 4);
            assert(v[2][1] == 5);
            assert(v[2][2] == 5);
            assert(v[3][0] == 4);
            assert(v[3][1] == 7);
            assert(v[3][2] == 5);
        }
        // 1-cell in the middle
        {
            Cell c(5,7,8);
            CellVector v;
            grid.below(c, v);
            assert(v.size() == 2);
            assert(v[0][0] == 5);
            assert(v[0][1] == 7);
            assert(v[0][2] == 7);
            assert(v[1][0] == 5);
            assert(v[1][1] == 7);
            assert(v[1][2] == 9);
        }
        {
            Cell c(5,8,7);
            CellVector v;
            grid.below(c, v);
            assert(v.size() == 2);
            assert(v[0][0] == 5);
            assert(v[0][1] == 7);
            assert(v[0][2] == 7);
            assert(v[1][0] == 5);
            assert(v[1][1] == 9);
            assert(v[1][2] == 7);
        }
        {
            Cell c(5,7,8);
            CellVector v;
            grid.below(c, v);
            assert(v.size() == 2);
            assert(v[0][0] == 5);
            assert(v[0][1] == 7);
            assert(v[0][2] == 7);
            assert(v[1][0] == 5);
            assert(v[1][1] == 7);
            assert(v[1][2] == 9);
        }
    }

    return 0;
}
