#pragma once
#ifndef ANDRES_CWX_CELLGRID_HXX
#define ANDRES_CWX_CELLGRID_HXX

#include <cassert>

#include "stack-vector.hxx"
#include "cwx/cell.hxx"

namespace andres {
namespace cwx {

/// geometry and topology of a 3-dimensional cell grid.
///
/// \tparam T label of cells (e.g. unsigned int).
/// \tparam C coordinate (e.g. unsigned int).
template<class T, class C>
class Cellgrid {
public:
    typedef T Label;
    typedef C Coordinate;
    typedef Cell<Coordinate> CellType;
    typedef typename CellType::Order Order;
    typedef StackVector<CellType, 6> CellVector;

    Cellgrid();
    Cellgrid(const Coordinate, const Coordinate, const Coordinate);
    void resize(const Coordinate, const Coordinate, const Coordinate);

    // query geometry and topology
    Label numberOfCells(const Order) const;

    // query geometry
    Coordinate shape(const Order) const;
    bool firstCell(const Order, CellType&) const;
    bool firstCell(const Order, const Order, const Coordinate, CellType&) const;
    bool orderPreservingIncrement(CellType&) const;
    bool orderPreservingIncrement(const Order, CellType&) const;
    void above(const CellType&, CellVector&) const;
    void below(const CellType&, CellVector&) const;

    // query topology (subset of the interface of CWComplex)
    size_t sizeAbove(const Order, const Label) const;
    size_t sizeBelow(const Order, const Label) const;
    Label above(const Order, const Label, const size_t) const;
    Label below(const Order, const Label, const size_t) const;

    // query the connection between geometry and topology (specific for grids)
    Label label(const CellType&) const;
    void cell(const Order, const Label, CellType&) const;

private:
    unsigned char byte(const CellType&) const;
    Coordinate gc(const Coordinate) const;

    Coordinate shape_[3];
};

template<class T, class C>
inline
Cellgrid<T, C>::Cellgrid() {
    shape_[0] = 0;
    shape_[1] = 0;
    shape_[2] = 0;
}

// Cartesian coordinates (not cell coordinates)
template<class T, class C>
inline
Cellgrid<T, C>::Cellgrid(
    const Coordinate shape0, 
    const Coordinate shape1, 
    const Coordinate shape2
) {
    assert(shape0 >= 0 && shape1 >= 0 && shape2 >= 0);
    shape_[0] = shape0;
    shape_[1] = shape1;
    shape_[2] = shape2;
}

// Cartesian coordinates (not cell coordinates)
template<class T, class C>
inline typename Cellgrid<T, C>::Coordinate
Cellgrid<T, C>::shape(
    const Order dimension
) const {
    assert(dimension >= 0 && dimension <= 3);
    return shape_[dimension];
}

template<class T, class C>
inline typename Cellgrid<T, C>::Label
Cellgrid<T, C>::numberOfCells(
    const Order order
) const
{
    switch(order) {
    case 0:
        return (shape(0) - 1) * (shape(1) - 1) * (shape(2) - 1);
        break;
    case 1:
        return    (shape(0) - 1) * (shape(1) - 1) *  shape(2)
                + (shape(0) - 1) *  shape(1)      * (shape(2) - 1)
                +  shape(0)      * (shape(1) - 1) * (shape(2) - 1);
        break;
    case 2:
        return    shape(0) * shape(1) * (shape(2) - 1)
                + shape(0) * (shape(1) - 1) * shape(2)
                + (shape(0) - 1) * shape(1) * shape(2);
        break;
    case 3:
        return shape(0) * shape(1) * shape(2);
        break;
    default:
        throw std::runtime_error("invalid order");
        break;
    }
}

// writes the first cell of the given order to the parameter cell
// and returns true if such a cell exists. returns false and leaves
// cell in an undefined state, otherwise.
// TODO: implement this function more efficiently using the functions label and cell of Cellgrid
template<class T, class C>
inline bool
Cellgrid<T, C>::firstCell(
    const Order order,
    CellType& cell
) const
{
    assert(order < 4);
    cell.assign(0, 0, 0);
    while(cell.order() != order) {
        // increment cell
        for(Order j = 0; j < 3; ++j) {
            if(cell[j] < 2 * shape(j) - 2) {
                ++cell[j];
                break;
            }
            else {
                if(j == 2) {
                    return false;
                }
                else {
                    cell[j] = 0;
                }
            }
        }
    }
    return true;
}

// writes the first cell of the given order to the parameter cell
// and returns true if such a cell exists. returns false and leaves
// cell in an undefined state, otherwise.
// TODO: implement this function more efficiently
template<class T, class C>
inline bool
Cellgrid<T, C>::firstCell(
    const Order order,
    const Order fixedDimension,
    const Coordinate fixedValue,
    CellType& cell
) const
{
    assert(order < 4);
    assert(fixedValue < 2*shape(fixedDimension)-1);
    cell.assign(0, 0, 0);
    cell[fixedDimension] = fixedValue;
    while(cell.order() != order) {
        // increment cell
        for(Order j = 0; j < 3; ++j) {
            if(j == fixedDimension) {
                if(j == 2) {
                    return false;
                }
                else {
                    continue;
                }
            }
            else {
                if(cell[j] < 2 * shape(j) - 2) {
                    ++cell[j];
                    break;
                }
                else {
                    if(j == 2) {
                        return false;
                    }
                    else {
                        cell[j] = 0;
                    }
                }
            }
        }
    }
    return true;
}

// returns true if a succeeding cell of the same order was
// found and false if the end of the grid has been reached
// TODO: implement this function more efficiently
template<class T, class C>
inline bool
Cellgrid<T, C>::orderPreservingIncrement(
    CellType& cell
) const
{
    const Order order = cell.order();
    do {
        // increment cell
        for(Order j = 0; j < 3; ++j) {
            if(cell[j] < 2 * shape(j) - 2) {
                ++cell[j];
                break;
            }
            else {
                if(j == 2) {
                    return false;
                }
                else {
                    cell[j] = 0;
                }
            }
        }
    } while(cell.order() != order);
    return true;
}

// returns true if a succeeding cell of the same order was
// found and false if the end of the grid has been reached.
// one coordinate (fix) is fixed.
// TODO: implement this function more efficiently
template<class T, class C>
inline bool
Cellgrid<T, C>::orderPreservingIncrement(
    const Order fixedDimension,
    CellType& cell
) const
{
    const Order order = cell.order();
    do {
        // increment cell
        for(Order j = 0; j < 3; ++j) {
            if(j == fixedDimension) {
                if(j == 2) {
                    return false;
                }
                else {
                    continue;
                }
            }
            else {
                if(cell[j] < 2 * shape(j) - 2) {
                    ++cell[j];
                    break;
                }
                else {
                    if(j == 2) {
                        return false;
                    }
                    else {
                        cell[j] = 0;
                    }
                }
            }
        }
    } while(cell.order() != order);
    return true;
}

template<class T, class C>
void
Cellgrid<T, C>::above(
    const CellType& cell,
    CellVector& cellsAbove
) const
{
    size_t d[2];
    switch(cell.order()) {
    case 0:
        cellsAbove.resize(6);
        cellsAbove[0][0] = cell[0] - 1;
        cellsAbove[0][1] = cell[1];
        cellsAbove[0][2] = cell[2];
        cellsAbove[1][0] = cell[0] + 1;
        cellsAbove[1][1] = cell[1];
        cellsAbove[1][2] = cell[2];
        cellsAbove[2][0] = cell[0];
        cellsAbove[2][1] = cell[1] - 1;
        cellsAbove[2][2] = cell[2];
        cellsAbove[3][0] = cell[0];
        cellsAbove[3][1] = cell[1] + 1;
        cellsAbove[3][2] = cell[2];
        cellsAbove[4][0] = cell[0];
        cellsAbove[4][1] = cell[1];
        cellsAbove[4][2] = cell[2] - 1;
        cellsAbove[5][0] = cell[0];
        cellsAbove[5][1] = cell[1];
        cellsAbove[5][2] = cell[2] + 1;
        assert(cellsAbove[0].order() == 1);
        assert(cellsAbove[1].order() == 1);
        assert(cellsAbove[2].order() == 1);
        assert(cellsAbove[3].order() == 1);
        assert(cellsAbove[4].order() == 1);
        assert(cellsAbove[5].order() == 1);
        break;
    case 1:
        cellsAbove.resize(4);
        // determine the two dimensions d[0] and d[1] such that
        // cell[d[0]] % 2 == 1 and cell[d[1]] % 2 == 1
        if(cell[0] % 2 == 0) {
            d[0] = 1;
            d[1] = 2;
        }
        else if(cell[1] % 2 == 0) {
            d[0] = 0;
            d[1] = 2;
        }
        else {
            d[0] = 0;
            d[1] = 1;
        }
        for(size_t j=0; j<4; ++j) {
            cellsAbove[j] = cell;
        }
        --cellsAbove[0][d[0]];
        ++cellsAbove[1][d[0]];
        --cellsAbove[2][d[1]];
        ++cellsAbove[3][d[1]];
        assert(cellsAbove[0].order() == 2);
        assert(cellsAbove[1].order() == 2);
        assert(cellsAbove[2].order() == 2);
        assert(cellsAbove[3].order() == 2);
        break;
    case 2:
        cellsAbove.resize(2);
        // determine the one dimension d such that cell[d] % 2 == 1
        if(cell[0] % 2 == 1) {
            d[0] = 0;
        }
        else if(cell[1] % 2 == 1) {
            d[0] = 1;
        }
        else {
            d[0] = 2;
        }
        cellsAbove[0] = cell;
        cellsAbove[1] = cell;
        --cellsAbove[0][d[0]];
        ++cellsAbove[1][d[0]];
        assert(cellsAbove[0].order() == 3);
        assert(cellsAbove[1].order() == 3);
        break;
    case 3:
        cellsAbove.resize(0);
        break;
    }
}

template<class T, class C>
void
Cellgrid<T, C>::below(
    const CellType& cell,
    CellVector& cellsBelow
) const
{
    cellsBelow.resize(0);
    Order d[2];
    switch(cell.order()) {
    case 0:
        break;
    case 1:
        if(cell[0] % 2 == 0) {
            d[0] = 0;
        }
        else if(cell[1] % 2 == 0) {
            d[0] = 1;
        }
        else {
            d[0] = 2;
        }
        if(cell[d[0]] > 0) {
            CellType c = cell;
            --c[d[0]];
            assert(c.order() == 0);
            cellsBelow.push_back(c);
        }
        if(cell[d[0]] < 2 * shape(d[0]) - 2) {
            CellType c = cell;
            ++c[d[0]];
            assert(c.order() == 0);
            cellsBelow.push_back(c);
        }
        break;
    case 2:
        if(cell[0] % 2 == 1) {
            d[0] = 1;
            d[1] = 2;
        }
        else if(cell[1] % 2 == 1) {
            d[0] = 0;
            d[1] = 2;
        }
        else {
            d[0] = 0;
            d[1] = 1;
        }
        for(size_t j=0; j<2; ++j) {
            if(cell[d[j]] > 0) {
                CellType c = cell;
                --c[d[j]];
                assert(c.order() == 1);
                cellsBelow.push_back(c);
            }
            if(cell[d[j]] < 2 * shape(d[j]) - 2) {
                CellType c = cell;
                ++c[d[j]];
                assert(c.order() == 1);
                cellsBelow.push_back(c);
            }
        }
        break;
    case 3:
        for(Order j = 0; j < 3; ++j) {
            if(cell[j] > 0) {
                CellType c = cell;
                --c[j];
                assert(c.order() == 2);
                cellsBelow.push_back(c);
            }
            if(cell[j] < 2 * shape(j) - 2) {
                CellType c = cell;
                ++c[j];
                assert(c.order() == 2);
                cellsBelow.push_back(c);
            }
        }
        break;
    }
}

// Cartesian coordinates (not cell coordinates)
template<class T, class C>
inline void
Cellgrid<T, C>::resize(
    const Coordinate shape0,
    const Coordinate shape1,
    const Coordinate shape2
) {
    assert(shape0 > 0 && shape1 > 0 && shape2 > 0);
    shape_[0] = shape0;
    shape_[1] = shape1;
    shape_[2] = shape2;
}

template<class T, class C>
inline size_t 
Cellgrid<T, C>::sizeAbove(
    const Order order,  
    const Label label
) const {
    switch(order) {
    case 0:
        return 6;
        break;
    case 1:
        return 4;
        break;
    case 2:
        return 2;
        break;
    case 3:
        return 0;
        break;
    default:
        throw std::runtime_error("invalid order.");
        break;
    }
}

template<class T, class C>
inline size_t 
Cellgrid<T, C>::sizeBelow(
    const Order order, 
    const Label cellLabel
) const {
    assert(order >= 0 && order <= 3);
    assert(cellLabel > 0 && cellLabel <= numberOfCells(order));
    if(order == 0) {
        return 0;
    }
    else {
        // sizeBelow if not at boundary of grid:
        size_t n = 2 * static_cast<size_t>(order); 
        // subtract if at boundary:
        Cell<Coordinate> c;
        cell(order, cellLabel, c);        
        for(Order j = 0; j < 3; ++j) {
            if(c[j] == 0) {
                --n;
            }
            if(c[j] == 2 * shape(j) - 2) {
                --n;
            }
        }
        return n;
    }
}

template<class T, class C>
inline typename Cellgrid<T, C>::Label 
Cellgrid<T, C>::above(
    const Order order,   
    const Label cellLabel, 
    const size_t j
) const {
    assert(order >= 0 && order <= 3);
    assert(cellLabel > 0 && cellLabel <= numberOfCells(order));
    CellType c;
    cell(order, cellLabel, c);
    CellVector cellsAbove;
    above(c, cellsAbove);
    return label(cellsAbove[j]);
}

template<class T, class C>
inline typename Cellgrid<T, C>::Label 
Cellgrid<T, C>::below(
    const Order order, 
    const Label cellLabel, 
    const size_t j
) const {
    assert(order >= 0 && order <= 3);
    assert(cellLabel > 0 && cellLabel <= numberOfCells(order));
    CellType c;
    cell(order, cellLabel, c);
    CellVector cellsBelow;
    below(c, cellsBelow);
    return label(cellsBelow[j]);
}

template<class T, class C>
typename Cellgrid<T, C>::Label 
Cellgrid<T, C>::label(
    const CellType& c
) const {
    assert(c[0] >= 0 && c[0] < 2 * shape(0) - 1);
    assert(c[1] >= 0 && c[1] < 2 * shape(1) - 1);
    assert(c[2] >= 0 && c[2] < 2 * shape(2) - 1);
    const Order order = c.order();
    if(order == 0) {
        const Coordinate t0 = shape(0) - 1;
        const Coordinate t1 = shape(1) - 1;
        const Label label = (c[0] - 1) / 2 
            + t0 * ((c[1] - 1) / 2)
            + t0 * t1 * ((c[2] - 1) / 2)
            + 1;
        assert(label > 0 && label <= numberOfCells(0));
        return label;
    }
    else if(order == 1) {
        // 1-cells have one even coordinate.
        // first come the 1-cells who's first coordinate is even,
        // followed by those who's second coordinate is even,
        // followed by those who's third coordinate is even.
        Order even;
        Order odd[2];
        {
            Order index = 0;
            for(Order j = 0; j < 3; ++j) {
                if(c[j] % 2 == 1) {
                    odd[index] = j;
                    ++index;
                }
                else {
                    even = j;
                }
            }
        }
        Coordinate label = ((c[odd[0]] - 1) / 2)
            + (shape(odd[0]) - 1) * ((c[odd[1]] - 1) / 2)
            + (shape(odd[0]) - 1) * (shape(odd[1]) - 1) * (c[even] / 2)
            + 1;
        if(even == 1) {
            label += shape(0) * (shape(1) - 1) * (shape(2) - 1);
        }
        else if(even == 2) {
            label += shape(0) * (shape(1) - 1) * (shape(2) - 1)
                + (shape(0) - 1) * shape(1) * (shape(2) - 1);
        }
        assert(label > 0 && label <= numberOfCells(1));
        return label;
    }
    else if(order == 2) {
        // 2-cells have one odd coordinate.
        // first come the 2-cells who's first coordinate is odd,
        // followed by those who's second coordinate is odd,
        // followed by those who's third coordinate is odd.
        Order odd;
        Order even[2];
        {
            Order index = 0;
            for(Order j = 0; j < 3; ++j) {
                if(c[j] % 2 == 0) {
                    even[index] = j;
                    ++index;
                }
                else {
                    odd = j;
                }
            }
        }      
        Coordinate label = (c[even[0]] / 2)
            + shape(even[0]) * (c[even[1]] / 2)
            + shape(even[0]) * shape(even[1]) * ((c[odd] - 1) / 2)
            + 1;
        if(odd == 1) {
            label += (shape(0) - 1) * shape(1) * shape(2);
        }
        else if(odd == 2) {
            label += (shape(0) - 1) * shape(1) * shape(2)
                + shape(0) * (shape(1) - 1) * shape(2);
        }
        assert(label > 0 && label <= numberOfCells(2));
        return label;
    }
    else { // order == 3
        const Label label = (c[0] / 2)
            + shape(0) * (c[1] / 2)
            + shape(0) * shape(1) * (c[2] / 2)
            + 1;
        assert(label > 0 && label <= numberOfCells(3));
        return label;
    }
}

template<class T, class C>
void 
Cellgrid<T, C>::cell(
    const Order order,
    const Label cellLabel, 
    CellType& cell
) const {
    assert(cellLabel > 0 && cellLabel <= numberOfCells(order));
    Label index = cellLabel - 1;
    if(order == 0) {
        const Coordinate t0 = shape(0) - 1;
        const Coordinate t1 = shape(1) - 1;
        const Coordinate stride2 = t0 * t1;
        cell[2] = 2 * (index / stride2) + 1;
        index %= stride2;
        cell[1] = 2 * (index / t0) + 1;
        cell[0] = 2 * (index % t0) + 1;
    }
    else if(order == 1) {
        // 1-cells have one even coordinate.
        // first come the 1-cells who's first coordinate is even,
        // followed by those who's second coordinate is even,
        // followed by those who's third coordinate is even.
        Order even;
        Order odd[2];
        const Coordinate nn = shape(0) * (shape(1) - 1) * (shape(2) - 1) + (shape(0) - 1) * shape(1) * (shape(2) - 1);
        const Coordinate n = shape(0) * (shape(1) - 1) * (shape(2) - 1);
        if(index >= nn) {
            even = 2;
            odd[0] = 0;
            odd[1] = 1;
            index -= nn;
        }
        else if(index >= n) {
            even = 1;
            odd[0] = 0;
            odd[1] = 2;
            index -= n;
        }
        else {
            even = 0;
            odd[0] = 1;
            odd[1] = 2;
        }
        const Coordinate stride0 = (shape(odd[0]) - 1) * (shape(odd[1]) - 1);
        cell[even] = (index / stride0) * 2;
        index %= stride0;
        const Coordinate stride1 = shape(odd[0]) - 1;
        cell[odd[1]] = (index / stride1) * 2 + 1;
        index %= stride1;
        cell[odd[0]] = index * 2 + 1;
    }
    else if(order == 2) {
        // 2-cells have one odd coordinate.
        // first come the 2-cells who's first coordinate is odd,
        // followed by those who's second coordinate is odd,
        // followed by those who's third coordinate is odd.        
        Order odd;
        Order even[2];
        const Coordinate nn = (shape(0) - 1) * shape(1) * shape(2) + shape(0) * (shape(1) - 1) * shape(2);
        const Coordinate n = (shape(0) - 1) * shape(1) * shape(2);
        if(index >= nn) {
            odd = 2;
            even[0] = 0;
            even[1] = 1;
            index -= nn;
        }
        else if(index >= n) {
            odd = 1;
            even[0] = 0;
            even[1] = 2;
            index -= n;
        }
        else {
            odd = 0;
            even[0] = 1;
            even[1] = 2;
        }
        const Coordinate stride0 = shape(even[0]) * shape(even[1]);
        cell[odd] = (index / stride0) * 2 + 1;
        index %= stride0;
        const Coordinate stride1 = shape(even[0]);
        cell[even[1]] = (index / stride1) * 2;
        index %= stride1;
        cell[even[0]] = index * 2;
    }
    else if(order == 3) {
        const Coordinate stride2 = shape(0) * shape(1);
        cell[2] = 2 * (index / stride2);
        index %= stride2;
        cell[1] = 2 * (index / shape(0));
        cell[0] = 2 * (index % shape(0));
    }
    else {
        throw std::runtime_error("invalid order");
    }
    assert(cell.order() == order);
}

} // namespace cwx
} // namespace andres

#endif // #ifndef ANDRES_CWX_CELLGRID_HXX
