#pragma once
#ifndef ANDRES_CWX_BYTE_LABELED_CELLGRID_HXX
#define ANDRES_CWX_BYTE_LABELED_CELLGRID_HXX

#include <cassert>
#include <string>
#include <sstream>

#include "marray.hxx"
#include "cwx/cellgrid.hxx"

namespace cwx {

template<class T, class C>
class ByteLabeledCellgrid 
: public Cellgrid<T, C>
{
public:
    typedef T Label;
    typedef C Coordinate;
    typedef Cellgrid<Label, Coordinate> CellgridType;
    typedef typename CellgridType::CellType CellType;
    typedef typename CellgridType::CellVector CellVector;
    typedef typename CellType::Order Order;
    typedef andres::View<unsigned char> GridViewType;

    // construction and assignment
    ByteLabeledCellgrid();
    ByteLabeledCellgrid(const Coordinate, const Coordinate, const Coordinate);

    // query
    bool isMarked(const CellType&) const;
    bool isAnchored(const CellType&) const;
    std::string asString() const;

    // manipulation
    void resize(const Coordinate, const Coordinate, const Coordinate);
    void mark(const CellType&, const bool);
    void anchor(const CellType&, const bool);
    
    const andres::View<unsigned char> grid() const { return grid_; }

private:
    unsigned char byte(const CellType&) const;
    Coordinate gc(const Coordinate) const;

    andres::Marray<unsigned char> grid_;
    static const unsigned char byte_[2][2][2];
};

template<class T, class C>
const unsigned char ByteLabeledCellgrid<T, C>::byte_[2][2][2] = {
    { {128, 1}, {2, 4} },
    { {8, 16}, {32, 64} }
};

template<class T, class C>
inline
ByteLabeledCellgrid<T, C>::ByteLabeledCellgrid()
:   CellgridType(), 
    grid_()
{}

// Cartesian coordinates (not cell coordinates)
template<class T, class C>
inline
ByteLabeledCellgrid<T, C>::ByteLabeledCellgrid(
    const Coordinate n0,
    const Coordinate n1,
    const Coordinate n2
)
#ifndef _MSC_VER // MSVC does currently not support initializer lists
:   CellgridType(n0, n1, n2), 
    grid_({n0, n1, n2})
#else
:   CellgridType(n0, n1, n2)
#endif
{
    assert(n0 > 0 && n1 > 0 && n2 > 0);
#   ifdef _MSC_VER // MSVC does currently not support initializer lists
        size_t shape[] = {n0, n1, n2};
        grid_.resize(shape, shape + 3);
#   endif
}

// voxel coordinates, not cell coordinates
template<class T, class C>
inline void
ByteLabeledCellgrid<T, C>::resize(
    const Coordinate n0,
    const Coordinate n1,
    const Coordinate n2
)
{
    assert(n0 > 0 && n1 > 0 && n2 > 0);
    static_cast<CellgridType*>(this)->resize(n0, n1, n2);
#   ifdef _MSC_VER // MSVC does currently not support initializer lists
        size_t shape[] = {n0, n1, n2};
        grid_.resize(shape, shape + 3);
#   else
        grid_.resize({n0, n1, n2});
#   endif
}

template<class T, class C>
inline bool
ByteLabeledCellgrid<T, C>::isMarked(
    const CellType& cell
) const
{
    return grid_(gc(cell[0]), gc(cell[1]), gc(cell[2])) & byte(cell);
}

template<class T, class C>
inline bool
ByteLabeledCellgrid<T, C>::isAnchored(
    const CellType& cell
) const
{
    return grid_(gc(cell[0]), gc(cell[1]), gc(cell[2])) & 128;
}

template<class T, class C>
std::string
ByteLabeledCellgrid<T, C>::asString() const
{
    std::stringstream s;
    CellType c;
    for(c[2] = 0; c[2] < 2 * this->shape(2) - 1; ++c[2]) {
        for(c[1] = 0; c[1] < 2 * this->shape(1) - 1; ++c[1]) {
            for(c[0] = 0; c[0] < 2 * this->shape(0) - 1; ++c[0]) {
                if(isMarked(c)) {
                    if(c.order() == 3) {
                        s << "*";
                    }
                    else {
                        s << static_cast<int>(c.order());
                    }
                }
                else {
                    s << '.';
                }
            }
            s << std::endl;
        }
        s << std::endl;
    }
    return s.str();
}

template<class T, class C>
inline void
ByteLabeledCellgrid<T, C>::mark(
    const CellType& cell,
    const bool value
)
{
    if(value) {
        grid_(gc(cell[0]), gc(cell[1]), gc(cell[2])) |= byte(cell);
    }
    else {
        grid_(gc(cell[0]), gc(cell[1]), gc(cell[2])) &= -byte(cell);
    }
}

template<class T, class C>
inline void
ByteLabeledCellgrid<T, C>::anchor(
    const CellType& cell,
    const bool value
)
{
    if(value) {
        grid_(gc(cell[0]), gc(cell[1]), gc(cell[2])) |= 128;
    }
    else {
        grid_(gc(cell[0]), gc(cell[1]), gc(cell[2])) &= 127;
    }
}

template<class T, class C>
inline unsigned char
ByteLabeledCellgrid<T, C>::byte(
    const CellType& cell
) const
{
    return ByteLabeledCellgrid<T, C>::byte_[cell[0] % 2][cell[1] % 2][cell[2] % 2];
}

template<class T, class C>
inline typename ByteLabeledCellgrid<T, C>::Coordinate
ByteLabeledCellgrid<T, C>::gc(
    const Coordinate c
) const
{
    return (c - (c % 2)) / 2;
}

} // namespace cwx

#endif // #ifndef ANDRES_CWX_BYTE_LABELED_CELLGRID_HXX
