#include <stdexcept>
#include <random>

#include "cwx/cellgrid.hxx"

inline void test(const bool& pred) {
    if(!pred) throw std::runtime_error("Test failed.");
}

typedef unsigned int Label;
typedef unsigned int Coordinate;
typedef andres::cwx::Cellgrid<Label, Coordinate> Cellgrid;
typedef Cellgrid::Order Order;
typedef Cellgrid::CellType Cell;
typedef Cellgrid::CellVector CellVector;

void testGeometry() {
    {
        // construction of empty grid
        Cellgrid grid;
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
    }

    {
        // construction of non-empty grid
        Cellgrid grid(3, 2, 4);
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
        Cellgrid grid(5, 5, 5);
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

    // firstCell, orderPreservingIncrement
    size_t counters[] = {4*4*4, (4*4)*5*3, (5*5)*4*3 , 5*5*5};
    for(unsigned char order = 0; order < 3; ++order) {
        size_t counter = 0;
        Cellgrid grid(5, 5, 5);
        Cell c;
        if(grid.firstCell(order, c)) {
            do {
                test(c.order() == order);
                ++counter;
            } while(grid.orderPreservingIncrement(c));
        }
        test(counter == counters[order]);
    }

    // firstCell, orderPreservingIncrement with fixed dimension
    // is tested in unit test of ByteLabeledCellgrid

    // above
    {
        Cellgrid grid(10, 20, 30);
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
        Cellgrid grid(10, 20, 30);
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
}

void testTopology() {
    // sizeAbove, sizeBelow, above, below
    Coordinate shape[] = {3, 5, 4};
    Cellgrid cellgrid(shape[0], shape[1], shape[2]);
    andres::cwx::Cell<Coordinate> cell;
    for(cell[2] = 0; cell[2] < 2 * shape[2] - 1; ++cell[2]) 
    for(cell[1] = 0; cell[1] < 2 * shape[1] - 1; ++cell[1])
    for(cell[0] = 0; cell[0] < 2 * shape[0] - 1; ++cell[0]) {
        {
            CellVector cellVector;
            cellgrid.above(cell, cellVector);
            test(cellgrid.sizeAbove(cell.order(), cellgrid.label(cell)) == cellVector.size());
            for(size_t j = 0; j < cellVector.size(); ++j) {
                test(cellgrid.label(cellVector[j]) == cellgrid.above(cell.order(), cellgrid.label(cell), j));
            }
        }
        {
            CellVector cellVector;
            cellgrid.below(cell, cellVector);
            test(cellgrid.sizeBelow(cell.order(), cellgrid.label(cell)) == cellVector.size());
            for(size_t j = 0; j < cellVector.size(); ++j) {
                test(cellgrid.label(cellVector[j]) == cellgrid.below(cell.order(), cellgrid.label(cell), j));
            }
        }
    }
}

void testGeometryCombinedWithTopology() {
    // labelOfCell, cellOfLabel
    Coordinate shape[] = {3, 5, 4};
    andres::cwx::Cellgrid<Label, Coordinate> cellgrid(shape[0], shape[1], shape[2]);
    for(unsigned char order = 0; order <= 3; ++order) {        
        std::vector<bool> seenLabels(cellgrid.numberOfCells(order), false);
        andres::cwx::Cell<Coordinate> cell;
        for(cell[2] = 0; cell[2] < 2 * shape[2] - 1; ++cell[2]) 
        for(cell[1] = 0; cell[1] < 2 * shape[1] - 1; ++cell[1])
        for(cell[0] = 0; cell[0] < 2 * shape[0] - 1; ++cell[0]) {
            if(cell.order() == order) {
                const Label label = cellgrid.label(cell);
                test(label > 0 && label <= cellgrid.numberOfCells(order));
                const Label index = label - 1;
                test(seenLabels[index] == 0);
                seenLabels[index] = true;
                andres::cwx::Cell<Coordinate> cellOfLabel;
                cellgrid.cell(order, label, cellOfLabel);
                for(unsigned char j = 0; j < 3; ++j) {
                    test(cell[j] == cellOfLabel[j]);
                }
            }
        }
        for(size_t j = 0; j < cellgrid.numberOfCells(order); ++j) {
            test(seenLabels[j]);
        }
    }
}

int main() {
    testGeometry();
    testTopology();
    testGeometryCombinedWithTopology();

    return 0;
}
