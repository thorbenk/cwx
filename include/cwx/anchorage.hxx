#pragma once
#ifndef ANDRES_CWX_ANCHORAGE_HXX
#define ANDRES_CWX_ANCHORAGE_HXX

#include <array>
#include <vector>
#include <map>

#include "cwx/cell.hxx"

namespace andres {
namespace cwx {

template<class T, class C>
class Anchorage {
public:
    typedef T Label;
    typedef C Coordinate;
    typedef Cell<Coordinate> CellType;
    typedef typename CellType::Order Order;

    // construction
    Anchorage();

    // query
    Label numberOfCells(const Order) const;
    Label anchor(const CellType&) const;
    void anchor(const Order, const Label, CellType&) const;

    // manipulation
    void anchor(const CellType&, const Label);
    Label push_back(const CellType&);

private:
    std::map<CellType, Label> labelAtCell_;
    std::array<std::vector<CellType>, 4> cellForLabel_;
};

template<class T, class C>
inline
Anchorage<T, C>::Anchorage()
:   labelAtCell_(),
    cellForLabel_()
{
    // inser zero labels
    cellForLabel_.fill(std::vector<CellType>(1));
    assert(cellForLabel_[0].size() == 1);
    assert(cellForLabel_[1].size() == 1);
    assert(cellForLabel_[2].size() == 1);
    assert(cellForLabel_[3].size() == 1);
}

template<class T, class C>
inline typename Anchorage<T, C>::Label
Anchorage<T, C>::numberOfCells(
    const Order order
) const
{
    assert(order < 4);
    return static_cast<Label>(cellForLabel_[order].size() - 1);
}


template<class T, class C>
inline typename Anchorage<T, C>::Label
Anchorage<T, C>::anchor(
    const CellType& cell
) const
{
    typename std::map<CellType, Label>::const_iterator it
        = labelAtCell_.find(cell);
    if(it == labelAtCell_.end()) { // if not found
        return 0;
    }
    else {
        return it->second;
    }
}

template<class T, class C>
inline void
Anchorage<T, C>::anchor(
    const Order order,
    const Label label,
    CellType& cell
) const
{
    assert(order < 4);
    assert(label < cellForLabel_[order].size());
    cell = cellForLabel_[order][label];
}

// add another anchor for a label that already has an anchor (precondition)
template<class T, class C>
inline void
Anchorage<T, C>::anchor(
    const CellType& cell,
    const Label label
)
{
    const Order order = cell.order();
    assert(label <= numberOfCells(order));
#   ifndef NDEBUG
        typename std::map<CellType, Label>::const_iterator it;
        it = labelAtCell_.find(cell);
        if(it != labelAtCell_.end()) { // if this cell is already labeled
            assert(it->second == label); // assert consistent with existing label
        }
#   endif
    labelAtCell_[cell] = label; // over-write or insert
}

template<class T, class C>
inline typename Anchorage<T, C>::Label
Anchorage<T, C>::push_back(
    const CellType& cell
)
{
    assert(labelAtCell_.find(cell) == labelAtCell_.end()); // not already there
    cellForLabel_[cell.order()].push_back(cell);
    const Label label = static_cast<Label>(cellForLabel_[cell.order()].size() - 1);
    assert(label != 0);
    labelAtCell_[cell] = label; // insert
    return label;
}

} // namespace andres
} // namespace cwx

#endif // #ifndef ANDRES_CWX_ANCHORAGE_HXX
