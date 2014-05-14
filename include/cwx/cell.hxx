// TODO: implement arithmetic operators and all comparison operators

#pragma once
#ifndef ANDRES_CWX_CELL_HXX
#define ANDRES_CWX_CELL_HXX

#include <vector>
#include <array>
#include <cassert>

namespace andres {
namespace cwx {

template<class C>
class Cell {
public:
    typedef C Coordinate;
    typedef unsigned char Order;
    typedef std::vector<std::array<Coordinate, 3> > Corners; // TODO: replace vector by stackvector<..., 8>

    // construction
    Cell();
    Cell(const Coordinate, const Coordinate, const Coordinate);
    void assign(const Coordinate, const Coordinate, const Coordinate);

    // query
    bool operator<(const Cell<Coordinate>& other) const;
    bool operator==(const Cell<Coordinate>& other) const;
    bool operator!=(const Cell<Coordinate>& other) const;
    Order order() const;
    void corners(Corners&) const;

    // manipulation
    Coordinate operator[](const size_t) const;
    Coordinate& operator[](const size_t);

private:
    Coordinate c_[3];
};

template<class C>
inline
Cell<C>::Cell() {
    c_[0] = 0;
    c_[1] = 0;
    c_[2] = 0;
}

template<class C>
inline
Cell<C>::Cell
(
    const Coordinate x,
    const Coordinate y,
    const Coordinate z
)
{
    assign(x, y, z);
}

template<class C>
inline void
Cell<C>::assign
(
    const Coordinate x,
    const Coordinate y,
    const Coordinate z
)
{
    c_[0] = x;
    c_[1] = y;
    c_[2] = z;
}

template<class C>
inline typename Cell<C>::Coordinate
Cell<C>::operator[]
(
    const size_t j
) const
{
    assert(j < 3);
    return c_[j];
}

template<class C>
inline typename Cell<C>::Coordinate&
Cell<C>::operator[]
(
    const size_t j
)
{
    assert(j < 3);
    return c_[j];
}

template<class C>
inline bool
Cell<C>::operator<
(
    const Cell<C>& other
) const
{
    if(c_[2] != other.c_[2]) {
        return c_[2] < other.c_[2];
    }
    else if(c_[1] != other.c_[1]) {
        return c_[1] < other.c_[1];
    }
    else {
        return c_[0] < other.c_[0];
    }
}

template<class C>
inline bool
Cell<C>::operator!=
(
    const Cell<C>& other
) const
{
    return c_[0] != other[0] || c_[1] != other[1] || c_[2] != other[2];
}

template<class C>
inline bool
Cell<C>::operator==
(
    const Cell<C>& other
) const
{
    return c_[0] == other[0] && c_[1] == other[1] && c_[2] == other[2];
}

template<class C>
inline typename Cell<C>::Order
Cell<C>::order() const
{
    return 3 - (c_[0] % 2) - (c_[1] % 2) - (c_[2] % 2);
}

// TODO: write unit test for this function
template<class C>
void
Cell<C>::corners(
    Corners& out
) const
{
    unsigned char even[] = {0,0};
    unsigned char odd[] = {0,0};
    Coordinate p;
    Coordinate q;
    Coordinate r;
    switch(order()) {
    case 0:
        out.resize(1);
        for(size_t j=0; j<3; ++j) {
            out[0][j] = (c_[j] + 1) / 2;
        }
        break;
    case 1:
        if(c_[0] % 2 == 0) {
            even[0] = 0;
            odd[0] = 1;
            odd[1] = 2;
        }
        else if(c_[1] % 2 == 0) {
            even[0] = 1;
            odd[0] = 0;
            odd[1] = 2;
        }
        else {
            even[0] = 2;
            odd[0] = 0;
            odd[1] = 1;
        }
        assert(c_[even[0]] % 2 == 0);
        assert(c_[odd[0]] % 2 == 1);
        assert(c_[odd[1]] % 2 == 1);
        p = c_[even[0]] / 2;
        q = (c_[odd[0]] + 1) / 2;
        r = (c_[odd[1]] + 1) / 2;
        out.resize(2);
        out[0][even[0]] = p;
        out[0][odd[0]] = q;
        out[0][odd[1]] = r;
        out[1][even[0]] = p + 1;
        out[1][odd[0]] = q;
        out[1][odd[1]] = r;
        break;
    case 2:
        if(c_[0] % 2 == 1) {
            odd[0] = 0;
            even[0] = 1;
            even[1] = 2;
        }
        else if(c_[1] % 2 == 1) {
            odd[0] = 1;
            even[0] = 2;
            even[1] = 0;
        }
        else {
            odd[0] = 2;
            even[0] = 0;
            even[1] = 1;
        }
        assert(c_[odd[0]] % 2 == 1);
        assert(c_[even[0]] % 2 == 0);
        assert(c_[even[1]] % 2 == 0);
        p = c_[even[0]] / 2;
        q = c_[even[1]] / 2;
        r = (c_[odd[0]] + 1) / 2;
        out.resize(4);
        out[0][odd[0]] = r;
        out[0][even[0]] = p;
        out[0][even[1]] = q;
        out[1][odd[0]] = r;
        out[1][even[0]] = p + 1;
        out[1][even[1]] = q;
        out[2][odd[0]] = r;
        out[2][even[0]] = p + 1;
        out[2][even[1]] = q + 1;
        out[3][odd[0]] = r;
        out[3][even[0]] = p;
        out[3][even[1]] = q + 1;
        break;
    case 3:
        // TODO: return the eight corners of the corresponding open cube
        out.resize(1);
        for(size_t j = 0; j < 3; ++j) {
            out[0][j] = c_[j] / 2;
        }
        break;
    }
}

} // namespace cwx
} // namespace andres

#endif // #ifndef ANDRES_CWX_CELL_HXX
