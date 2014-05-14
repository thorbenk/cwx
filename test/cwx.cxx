#include "cwx/cwx.hxx"

inline void test(const bool& pred) {
    if(!pred) throw std::runtime_error("Test failed.");
}

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
                    seg(x, y, z) = 2;
                }
            }
            else {
                if(x < 2) {
                    seg(x, y, z) = 3;
                }
                else {
                    seg(x, y, z) = 4;
                }
            }
        }
        else {
            if(y < 2) {
                if(x < 2) {
                    seg(x, y, z) = 5;
                }
                else {
                    seg(x, y, z) = 6;
                }
            }
            else {
                if(x < 2) {
                    seg(x, y, z) = 7;
                }
                else {
                    seg(x, y, z) = 8;
                }
            }
        }
    }

    CWX cwx;
    cwx.build(seg);

    test(cwx.numberOfCells(3) == 8);
    test(cwx.numberOfCells(2) == 12);
    test(cwx.numberOfCells(1) == 6);
    test(cwx.numberOfCells(0) == 1);

    // labels
    // 3-cells
    for(Coordinate z = 0; z < 4; z+=2)
    for(Coordinate y = 0; y < 4; y+=2)
    for(Coordinate x = 0; x < 4; x+=2) {
        test(cwx.atVoxel(x, y, z) == cwx.atVoxel(x+1, y, z));
        test(cwx.atVoxel(x, y, z) == cwx.atVoxel(x, y+1, z));
        test(cwx.atVoxel(x, y, z) == cwx.atVoxel(x+1, y+1, z));
        test(cwx.atVoxel(x, y, z) == cwx.atVoxel(x, y, z+1));
        test(cwx.atVoxel(x, y, z) == cwx.atVoxel(x+1, y, z+1));
        test(cwx.atVoxel(x, y, z) == cwx.atVoxel(x, y+1, z+1));
        test(cwx.atVoxel(x, y, z) == cwx.atVoxel(x+1, y+1, z+1));
    }

    // process
    {
        andres::cwx::CellCollector<Coordinate> mesh;
        cwx.process(0, 1, mesh);
        test(mesh.cells().size() == 1);
        test(mesh.cells()[0][0] == 3);
        test(mesh.cells()[0][1] == 3);
        test(mesh.cells()[0][2] == 3);
    }
    for(unsigned char order = 1; order < 4; ++order) {
        const size_t expectedSize = 1 << order;
        for(Label j = 1; j <= cwx.numberOfCells(order); ++j) {
            andres::cwx::CellCollector<Coordinate> mesh;
            cwx.process(order, j, mesh);
            test(mesh.cells().size() == expectedSize);
            for(size_t k=0; k<mesh.cells().size(); ++k) {
                test(mesh.cells()[k].order() == order);
            }
        }
    }

    // labeledCellGrid
    {
        andres::Marray<float> labeledCellGrid;
        cwx.labeledCellGrid(labeledCellGrid);
        test(labeledCellGrid.dimension() == 3);
        test(labeledCellGrid.shape(0) == cwx.shape(0) * 2 - 1);
        test(labeledCellGrid.shape(1) == cwx.shape(1) * 2 - 1);
        test(labeledCellGrid.shape(2) == cwx.shape(2) * 2 - 1);
        Cell c;
        for(c[2] = 0; c[2] < labeledCellGrid.shape(2); ++c[2]) 
        for(c[1] = 0; c[1] < labeledCellGrid.shape(1); ++c[1]) 
        for(c[0] = 0; c[0] < labeledCellGrid.shape(0); ++c[0]) {
            test(labeledCellGrid(c[0], c[1], c[2]) == cwx.atCell(c));
        }
    }

    // labeledVoxelGrid
    {
        andres::Marray<float> labeledVoxelGrid;
        cwx.labeledVoxelGrid(labeledVoxelGrid);
        test(labeledVoxelGrid.dimension() == 3);
        test(labeledVoxelGrid.shape(0) == cwx.shape(0));
        test(labeledVoxelGrid.shape(1) == cwx.shape(1));
        test(labeledVoxelGrid.shape(2) == cwx.shape(2));
        Cell c;
        for(c[2] = 0; c[2] < 2 * cwx.shape(2) - 1; c[2] += 2) 
        for(c[1] = 0; c[1] < 2 * cwx.shape(1) - 1; c[1] += 2) 
        for(c[0] = 0; c[0] < 2 * cwx.shape(0) - 1; c[0] += 2) {
            test(labeledVoxelGrid(c[0] / 2, c[1] / 2, c[2] / 2) == cwx.atCell(c));
        }
    }

    return 0;
}
