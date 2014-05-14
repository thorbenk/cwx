#include <stdexcept>

#include "cwx/cell.hxx"

inline void test(const bool& pred) {
    if(!pred) throw std::runtime_error("Test failed.");
}

int main() {
    typedef andres::cwx::Cell<int> Cell;

    // order
    {
        Cell cell(0, 0, 0);
        test(cell.order() == 3);
        cell[2] = 1;
        test(cell.order() == 2);
        cell[1] = 1;
        test(cell.order() == 1);
        cell[0] = 1;
        test(cell.order() == 0);
    }

    return 0;
}
