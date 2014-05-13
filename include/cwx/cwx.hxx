#pragma once
#ifndef ANDRES_CWX_HXX
#define ANDRES_CWX_HXX

#include <stdexcept>
#include <array>
#include <map>
#include <queue>
#include <vector>

#include "cwx/byte-labeled-cellgrid.hxx"
#include "cwx/cwcomplex.hxx"
#include "cwx/anchorage.hxx"

namespace andres {
namespace cwx {

// forward declarations
template<class T> class CWComplexLatex;
namespace detail {
    template<class T, class C> class Labeler; // functor for INTERNAL use with CWX<T, C>::process(const Order, FUNCTOR&)
    template<class T, class C> class Anchorer; // functor for INTERNAL use with CWX<T, C>::process(const Order, const Order, const Coordinate, FUNCTOR&)
    template<class T, class C> class AnchorTester; // functor for INTERNAL use with CWX<T, C>::process(const Order, const Order, const Coordinate, FUNCTOR&)
}

template<class T, class C>
class CWX {
public:
    typedef T Label;
    typedef C Coordinate;

private:
    typedef ByteLabeledCellgrid<Label, Coordinate> ByteLabeledCellgridType;
    typedef CWComplex<Label> CWComplexType;
    typedef Anchorage<Label, Coordinate> AnchorageType;
    typedef detail::Labeler<T, C> Labeler;
    typedef detail::Anchorer<T, C> Anchorer;
    typedef detail::AnchorTester<T, C> AnchorTester;

public:
    typedef typename ByteLabeledCellgridType::Order Order;
    typedef typename ByteLabeledCellgridType::CellType CellType;
    typedef typename ByteLabeledCellgridType::CellVector CellVector;

    // manipulation
    CWX(const bool = true);
    template<class U, bool B> void build(const View<U, B>&);

    // query
    Coordinate shape(const Order) const;
    Label numberOfCells(const Order) const;
    size_t sizeAbove(const Order, const Label) const;
    size_t sizeBelow(const Order, const Label) const;
    Label above(const Order, const Label, const size_t) const;
    void above(const CellType&, CellVector&) const;
    Label below(const Order, const Label, const size_t) const;
    void below(const CellType&, CellVector&) const;
    Label atVoxel(const Coordinate, const Coordinate, const Coordinate) const;
    Label atCell(const CellType&) const;
    bool isMarked(const CellType&) const;

    template<class FUNCTOR> void process(const Order, const Label, FUNCTOR&) const;
    template<class FUNCTOR> void process(const Order, FUNCTOR&) const;
    template<class FUNCTOR> void process(const Order, const Order, const Coordinate, FUNCTOR&) const;

    template<class U> void labeledCellGrid(Marray<U>&) const; 
    template<class U> void labeledCellGrid(View<U>&) const; 
    template<class U> void labeledCellSlice(const Order, const Coordinate, Marray<U>&) const; // TODO: implement
    template<class U> void labeledCellSlice(const Order, const Coordinate, View<U>&) const; // TODO: implement

    template<class U> void labeledVoxelGrid(Marray<U>&) const; 
    template<class U> void labeledVoxelGrid(View<U>&) const; 
    template<class U> void labeledVoxelSlice(const Order, const Coordinate, Marray<U>&) const; // TODO: implement
    template<class U> void labeledVoxelSlice(const Order, const Coordinate, View<U>&) const; // TODO: implement

private:
    void connect(const CellType&, const Label&);
    void testInvariant() const;

    ByteLabeledCellgridType byteLabeledCellgrid_;
    CWComplexType cwcomplex_;
    AnchorageType anchorage_;
    bool redundantAnchors_;
    // anchorage_  is a data structure for labeling a subset of cells which are
    //             called anchors
    // byteLabeledCellgrid_   also has a concept called anchors which is different. an
    //             anchor in cwcomplex_ refers to one 3-cell, three adjacent
    //             2-cells, three adjacent 1-cells and one adjacent 0-cell
    // these two concepts are used together in the following way: whenever
    // there is an anchor in byteLabeledCellgrid_, at least one of the cells it refers
    // to exists as an entry in anchorage_
    //
    // redundantAnchors_: if is set to true, not just one anchor per
    // connect component of cells is created but so many such that each
    // connected component in each slice of the volume contains at least one
    // anchor

friend class detail::Labeler<T, C>;
friend class detail::Anchorer<T, C>;
friend class detail::AnchorTester<T, C>;
friend class CWComplexLatex<Label>;
};

// functor to be used with CWX::process
template<class C>
class CellCollector {
public:
    typedef C Coordinate;
    typedef Cell<Coordinate> CellType;
    typedef std::vector<CellType> Container;

    CellCollector()
        : cells_()
        {}
    const Container& cells() const
        { return cells_; }
    bool operator()(const CellType& cell)
        { cells_.push_back(cell); return true; }

private:
    Container cells_;
};

namespace detail {

// functor for INTERNAL use with CWX::process
template<class T, class C>
class Labeler {
public:
    typedef T Label;
    typedef C Coordinate;
    typedef CWX<T, C> CWXType;
    typedef typename CWXType::Order Order;
    typedef typename CWXType::CellType CellType;

    Labeler(CWXType&, const Order);
    bool preprocess(const CellType&);
    bool operator()(const CellType&);
    bool postprocess();

private:
    CWXType& cwx_;
    Label label_;
    const Order order_;
};

// functor for INTERNAL use with CWX::process
template<class T, class C>
class Anchorer {
public:
    typedef T Label;
    typedef C Coordinate;
    typedef CWX<T, C> CWXType;
    typedef typename CWXType::Order Order;
    typedef typename CWXType::CellType CellType;

    Anchorer(CWXType&);
    bool preprocess(const CellType&);
    bool operator()(const CellType&);
    bool postprocess();

private:
    CWXType& cwx_;
    CellType anchor_;
    Label label_;
    bool anchorFound_;
};

// functor for INTERNAL use with CWX::process
template<class T, class C>
class AnchorTester {
public:
    typedef T Label;
    typedef C Coordinate;
    typedef CWX<T, C> CWXType;
    typedef typename CWXType::Order Order;
    typedef typename CWXType::CellType CellType;

    AnchorTester(const CWXType&);
    bool preprocess(const CellType&);
    bool operator()(const CellType&);
    bool postprocess();

private:
    const CWXType& cwx_;
    bool labeledAnchorFound_;
};

// functor for INTERNAL use with CWX::process
template<class T, class C, class U>
class ExportLabeler {
public:
    typedef T Label;
    typedef C Coordinate;
    typedef U ExportLabel;
    typedef CWX<T, C> CWXType;
    typedef typename CWXType::Order Order;
    typedef typename CWXType::CellType CellType;
    typedef View<U> ViewType;

    ExportLabeler(const CWXType&, ViewType&);
    void setLabel(const ExportLabel);
    bool operator()(const CellType&);

private:
    const CWXType& cwx_;
    ViewType& view_;
    ExportLabel label_;
};

// functor for INTERNAL use with CWX::process
template<class T, class C, class U>
class ExportVoxelLabeler {
public:
    typedef T Label;
    typedef C Coordinate;
    typedef U ExportLabel;
    typedef CWX<T, C> CWXType;
    typedef typename CWXType::Order Order;
    typedef typename CWXType::CellType CellType;
    typedef View<U> ViewType;

    ExportVoxelLabeler(const CWXType&, ViewType&);
    void setLabel(const ExportLabel);
    bool operator()(const CellType&);

private:
    const CWXType& cwx_;
    ViewType& view_;
    ExportLabel label_;
};

} // namespace detail

template<class T, class C>
inline
CWX<T,C>::CWX(
    const bool redundantAnchors
)
:   byteLabeledCellgrid_(),
    cwcomplex_(),
    anchorage_(),
    redundantAnchors_(redundantAnchors)
{}

template<class T, class C>
inline typename CWX<T,C>::Coordinate
CWX<T,C>::shape(
    const Order dimension
) const
{
    return byteLabeledCellgrid_.shape(dimension);
}

template<class T, class C>
inline typename CWX<T,C>::Label
CWX<T,C>::numberOfCells(
    const Order order
) const
{
    return cwcomplex_.numberOfCells(order);
}

template<class T, class C>
inline size_t
CWX<T,C>::sizeAbove(
    const Order order,
    const Label label
) const
{
    return cwcomplex_.sizeAbove(order, label);
}

template<class T, class C>
inline size_t
CWX<T,C>::sizeBelow(
    const Order order,
    const Label label
) const
{
    return cwcomplex_.sizeBelow(order, label);
}

template<class T, class C>
inline typename CWX<T,C>::Label
CWX<T,C>::above(
    const Order order,
    const Label label,
    const size_t j
) const
{
    return cwcomplex_.above(order, label, j);
}

template<class T, class C>
inline void
CWX<T,C>::above(
    const CellType& cell,
    CellVector& above
) const
{
    byteLabeledCellgrid_.above(cell, above);
}

template<class T, class C>
inline typename CWX<T,C>::Label
CWX<T,C>::below(
    const Order order,
    const Label label,
    const size_t j
) const
{
    return cwcomplex_.below(order, label, j);
}

template<class T, class C>
inline void
CWX<T,C>::below(
    const CellType& cell,
    CellVector& below
) const
{
    byteLabeledCellgrid_.below(cell, below);
}

template<class T, class C>
inline typename CWX<T,C>::Label
CWX<T,C>::atVoxel(
    const Coordinate x,
    const Coordinate y,
    const Coordinate z
) const
{
    CellType cell(2*x, 2*y, 2*z);
    return atCell(cell);
}

template<class T, class C>
typename CWX<T,C>::Label
CWX<T,C>::atCell(
    const CellType& cell
) const
{
    // TODO: assert that inside bounds
    const Order order = cell.order();
    if(order == 0) {
        assert(byteLabeledCellgrid_.isAnchored(cell));
        return anchorage_.anchor(cell);
    }
    else if(order == 3 || byteLabeledCellgrid_.isMarked(cell)) {
        CellVector below;
        CellVector above;
        std::queue<CellType> queue;
        ByteLabeledCellgridType visited(byteLabeledCellgrid_.shape(0), byteLabeledCellgrid_.shape(1), byteLabeledCellgrid_.shape(2));
        // TODO: improve this, it's a waste of RAM
        visited.mark(cell, true);
        queue.push(cell);
        while(!queue.empty()) {
            if(byteLabeledCellgrid_.isAnchored(queue.front())) { // if anchor found
                const Label label = anchorage_.anchor(queue.front());
                if(label != 0) { // if anchor has a label for this cell
                    return label;
                }
            }
            byteLabeledCellgrid_.below(queue.front(), below);
            queue.pop();
            for(size_t j=0; j<below.size(); ++j) {
                assert(below[j].order() == order - 1);
                if(!byteLabeledCellgrid_.isMarked(below[j])) { // if not a boundary
                    byteLabeledCellgrid_.above(below[j], above);
                    for(size_t k=0; k<above.size(); ++k) {
                        assert(above[k].order() == order);
                        if((order == 3 || byteLabeledCellgrid_.isMarked(above[k])) && !visited.isMarked(above[k])) {
                            visited.mark(above[k], true);
                            queue.push(above[k]);
                        }
                    }
                }
            }
        }
        throw std::runtime_error("no anchor found.");
    }
    else {
        return 0;
    }
}

template<class T, class C>
inline bool
CWX<T,C>::isMarked(
    const CellType& cell
) const
{
    return byteLabeledCellgrid_.isMarked(cell);
}

// process one connected component
template<class T, class C>
template<class FUNCTOR>
void
CWX<T,C>::process(
    const Order order,
    const Label label,
    FUNCTOR& functor
) const
{
    assert(label > 0 && label <= numberOfCells(order));
    if(order == 0) {
        CellType cell;
        anchorage_.anchor(order, label, cell);
        functor(cell);
    }
    else {
        CellType cell;
        anchorage_.anchor(order, label, cell);
        CellVector below;
        CellVector above;
        std::queue<CellType> queue;
        ByteLabeledCellgridType visited(byteLabeledCellgrid_.shape(0), byteLabeledCellgrid_.shape(1), byteLabeledCellgrid_.shape(2)); // TODO: improve this, it's a waste of RAM
        visited.mark(cell, true);
        queue.push(cell);
        while(!queue.empty()) {
            const bool proceed = functor(queue.front());
            if(!proceed) {
                return;
            }
            byteLabeledCellgrid_.below(queue.front(), below);
            queue.pop();
            for(size_t j=0; j<below.size(); ++j) {
                assert(below[j].order() == order - 1);
                if(!byteLabeledCellgrid_.isMarked(below[j])) { // if not a boundary
                    byteLabeledCellgrid_.above(below[j], above);
                    for(size_t k=0; k<above.size(); ++k) {
                        assert(above[k].order() == order);
                        if((order == 3 || byteLabeledCellgrid_.isMarked(above[k])) && !visited.isMarked(above[k])) {
                            visited.mark(above[k], true);
                            queue.push(above[k]);
                        }
                    }
                }
            }
        }
    }
}

// process all connected components of the given order
// - the functor is expected to have three functions
//   - bool operator(const CellType&)
//   - bool preprocess(const CellType&)
//   - bool postprocess()
// - if the return value is false, the connected component analysis is stopped
//   completely
// - internally, this function iterates over all cells. 
//   if you prefer to iterate over all labels of connected components, use
//   the function process(order, label, functor) within a loop over all labels
template<class T, class C>
template<class FUNCTOR>
void
CWX<T,C>::process(
    const Order order,
    FUNCTOR& functor
) const
{
    CellType cell;
    if(order == 0) {
        if(byteLabeledCellgrid_.firstCell(order, cell)) {
            do {
                const bool proceed = functor.preprocess(cell);
                if(!proceed) {
                    return;
                }
            } while(byteLabeledCellgrid_.orderPreservingIncrement(cell));
        }
    }
    else {
        ByteLabeledCellgridType visited(byteLabeledCellgrid_.shape(0), byteLabeledCellgrid_.shape(1), byteLabeledCellgrid_.shape(2));
        CellVector above;
        CellVector below;
        std::queue<CellType> queue;
        if(byteLabeledCellgrid_.firstCell(order, cell)) {
            do {
                assert(cell.order() == order);
                if((order == 3 || byteLabeledCellgrid_.isMarked(cell)) && !visited.isMarked(cell)) {
                    {
                        const bool proceed = functor.preprocess(cell);
                        if(!proceed) {
                            return;
                        }
                    }
                    visited.mark(cell, true);
                    queue.push(cell);
                    while(!queue.empty()) {
                        {
                            const bool proceed = functor(queue.front());
                            if(!proceed) {
                                return;
                            }
                        }
                        byteLabeledCellgrid_.below(queue.front(), below);
                        queue.pop();
                        for(size_t j=0; j<below.size(); ++j) {
                            assert(below[j].order() == order - 1);
                            if(!byteLabeledCellgrid_.isMarked(below[j])) { // if not a boundary
                                byteLabeledCellgrid_.above(below[j], above);
                                for(size_t k=0; k<above.size(); ++k) {
                                    assert(above[k].order() == order);
                                    if((order == 3 || byteLabeledCellgrid_.isMarked(above[k])) && !visited.isMarked(above[k])) {
                                        visited.mark(above[k], true);
                                        queue.push(above[k]);
                                    }
                                }
                            }
                        }
                    }
                    {
                        const bool proceed = functor.postprocess();
                        if(!proceed) {
                            return;
                        }
                    }
                }
            } while(byteLabeledCellgrid_.orderPreservingIncrement(cell));
        }
    }
}

// process all connected components of the given order in the slice x_d = v
// the functor is expected to have three functions
// - bool operator(const CellType&)
// - bool preprocess(const CellType&)
// - bool postprocess()
// if the return value is false, the connected component analysis is stopped
// completely.
template<class T, class C>
template<class FUNCTOR>
void
CWX<T,C>::process(
    const Order order,
    const Order d,
    const Coordinate v,
    FUNCTOR& functor
) const
{
    // TODO: implement special case for 0-cells
    CellType cell;
    ByteLabeledCellgridType visited(byteLabeledCellgrid_.shape(0), byteLabeledCellgrid_.shape(1), byteLabeledCellgrid_.shape(2)); // TODO: need RAM only for slice. also needs allocator.
    CellVector above;
    CellVector below;
    std::queue<CellType> queue;
    if(byteLabeledCellgrid_.firstCell(order, d, v, cell)) {
        do { // trace connected component
            assert(cell.order() == order);
            assert(cell[d] == v);
            if((order == 3 || byteLabeledCellgrid_.isMarked(cell)) && !visited.isMarked(cell)) {
                {
                    const bool proceed = functor.preprocess(cell);
                    if(!proceed) {
                        return;
                    }
                }
                visited.mark(cell, true);
                queue.push(cell);
                while(!queue.empty()) {
                    assert(cell.order() == order);
                    assert(cell[d] == v);
                    {
                        const bool proceed = functor(queue.front());
                        if(!proceed) {
                            return;
                        }
                    }
                    byteLabeledCellgrid_.below(queue.front(), below);
                    queue.pop();
                    for(size_t j=0; j<below.size(); ++j) {
                        assert(below[j].order() == order - 1);
                        if(!byteLabeledCellgrid_.isMarked(below[j]) && below[j][d] == v) { // if not a boundary and in the same slice
                            byteLabeledCellgrid_.above(below[j], above);
                            for(size_t k=0; k<above.size(); ++k) {
                                assert(above[k].order() == order);
                                if((order == 3 || byteLabeledCellgrid_.isMarked(above[k])) && above[k][d] == v && !visited.isMarked(above[k])) {
                                    visited.mark(above[k], true);
                                    queue.push(above[k]);
                                }
                            }
                        }
                    }
                }
                {
                    const bool proceed = functor.postprocess();
                    if(!proceed) {
                        return;
                    }
                }
            }
        } while(byteLabeledCellgrid_.orderPreservingIncrement(d, cell));
    }
}

template<class T, class C>
template<class U, bool B>
void
CWX<T,C>::build(
    const View<U, B>& volumeLabeling
)
{
    // TODO: define anchors in every connected component in every slice

    if(volumeLabeling.dimension() != 3) {
        throw std::runtime_error("segmentation is not 3-dimensional.");
    }
    byteLabeledCellgrid_ = ByteLabeledCellgridType(
        volumeLabeling.shape(0),
        volumeLabeling.shape(1),
        volumeLabeling.shape(2));

    // mark cells
    {
        CellVector cells;
        CellType cell;
        // 2-cells
        if(byteLabeledCellgrid_.firstCell(2, cell)) {
            do {
                byteLabeledCellgrid_.above(cell, cells);
                assert(cells.size() == 2);
                if(volumeLabeling(cells[0][0]/2, cells[0][1]/2, cells[0][2]/2)
                != volumeLabeling(cells[1][0]/2, cells[1][1]/2, cells[1][2]/2)) {
                    byteLabeledCellgrid_.mark(cell, true);
                }
            } while(byteLabeledCellgrid_.orderPreservingIncrement(cell));
        }
        // 1-cells
        if(byteLabeledCellgrid_.firstCell(1, cell)) {
           do {
                unsigned char marked = 0;
                byteLabeledCellgrid_.above(cell, cells);
                for(size_t j=0; j<cells.size(); ++j) {
                    if(byteLabeledCellgrid_.isMarked(cells[j])) {
                        ++marked;
                        if(marked > 2) {
                            byteLabeledCellgrid_.mark(cell, true);
                            break;
                        }
                    }
                }
            } while(byteLabeledCellgrid_.orderPreservingIncrement(cell));
        }
        // 0-cells
        if(byteLabeledCellgrid_.firstCell(0, cell)) {
           do {
                byteLabeledCellgrid_.above(cell, cells);
                unsigned char marked = 0;
                for(size_t j=0; j<cells.size(); ++j) {
                    if(byteLabeledCellgrid_.isMarked(cells[j])) {
                        ++marked;
                        if(marked > 2) {
                            byteLabeledCellgrid_.mark(cell, true);
                            const Label label = cwcomplex_.push_back(0);
                            byteLabeledCellgrid_.anchor(cell, true);
                            const Label sameLabel = anchorage_.push_back(cell);
                            assert(label == sameLabel);
                            break;
                        }
                    }
                }
                // TODO: check if the following treatment is consistent
                // with the axioms of topology
                if(marked == 1) { // the weird case
                    byteLabeledCellgrid_.mark(cell, true);
                    const Label label = cwcomplex_.push_back(0);
                    byteLabeledCellgrid_.anchor(cell, true);
                    const Label sameLabel = anchorage_.push_back(cell);
                    assert(label == sameLabel);
                }
            } while(byteLabeledCellgrid_.orderPreservingIncrement(cell));
        }
    }

    // label connected components of 3-cells, 2-cells and 1-cells
    for(Order order = 3; order > 0; --order) {
        Labeler labeler(*this, order);
        process(order, labeler);
    }

    if(redundantAnchors_) {
        Anchorer anchorer(*this);
        for(Order d=0; d<3; ++d) { // dimension orthogonal to the slice
            for(Coordinate v=0; v<2*shape(d)-1; ++v) { // coordinate in that dimension
                for(Order order = 2; order < 4; ++order) { // increasing order results in less anchors
                    process(order, d, v, anchorer);
                }
            }
        }
    }

    // update cwcomplex_ for 0-cells
    CellType cell;
    for(Label label = 1; label <= numberOfCells(0); ++label) {
        anchorage_.anchor(0, label, cell);
        connect(cell, label);
    }

    // TODO: collect labels of connected components of *all orders* in *each* anchor

    testInvariant();
}

// inserts connections into cwcomplex_
template<class T, class C>
inline void
CWX<T,C>::connect(
    const CellType& cell,
    const Label& label
)
{
    CellVector above;
    byteLabeledCellgrid_.above(cell, above);
    std::set<Label> labelsAbove; // TODO: max size is 6, need not heap alloc
    for(size_t j=0; j<above.size(); ++j) {
        const Label labelAbove = atCell(above[j]);
        if(labelAbove != 0) {
            labelsAbove.insert(labelAbove);
        }
    }
    for(typename std::set<Label>::const_iterator it = labelsAbove.begin(); it != labelsAbove.end(); ++it) {
        cwcomplex_.connect(cell.order(), label, *it);
    }
}

template<class T, class C>
inline void
CWX<T,C>::testInvariant() const
{
#   ifndef NDEBUG
    for(Order order = 0; order < 4; ++order) {
        assert(cwcomplex_.numberOfCells(order) == anchorage_.numberOfCells(order));
    }

    CellType cell;
    CellVector above;

    for(cell[2] = 0; cell[2] < 2*shape(2)-1; ++cell[2]) {
        for(cell[1] = 0; cell[1] < 2*shape(1)-1; ++cell[1]) {
            for(cell[0] = 0; cell[0] < 2*shape(0)-1; ++cell[0]) {
                if(byteLabeledCellgrid_.isMarked(cell)) {
                    const Label label = atCell(cell);
                    assert(label != 0);

                    std::set<Label> labelsAbove;
                    byteLabeledCellgrid_.above(cell, above);
                    for(size_t j=0; j<above.size(); ++j) {
                        const Label labelAbove = atCell(above[j]);
                        if(labelAbove != 0) {
                            labelsAbove.insert(labelAbove);
                        }
                    }
                    assert(labelsAbove.size() == sizeAbove(cell.order(), label));
                    typename std::set<Label>::const_iterator it = labelsAbove.begin();
                    for(size_t j=0; j<labelsAbove.size(); ++j, ++it) {
                        assert((CWX<T, C>::above(cell.order(), label, j)) == *it);
                    }

                    const Label anchorLabel = anchorage_.anchor(cell);
                    if(anchorLabel == 0) {
                        assert(cell.order() != 0);
                        if(redundantAnchors_) {
                            assert(cell.order() != 1);
                        }
                    }
                    else {
                        assert(anchorLabel == label);
                    }
                }
                else {
                    if(cell.order() != 3) {
                        assert(anchorage_.anchor(cell) == 0);
                    }
                }
            }
        }
    }

    for(Order order = 0; order < 4; ++order) {
        for(Label label = 1; label <= numberOfCells(order); ++label) {
            anchorage_.anchor(order, label, cell);
            assert(atCell(cell) == label);
        }
    }

    if(redundantAnchors_) {
        // test if every connected component in each slice has at least 1 anchor
        AnchorTester anchorTester(*this);
        for(Order d=0; d<3; ++d) { // dimension orthogonal to the slice
            for(Coordinate v=0; v<2*shape(d)-1; ++v) { // coordinate in that dimension
                for(Order order = 2; order < 4; ++order) { // increasing order results in less anchors
                    process(order, d, v, anchorTester);
                }
            }
        }
    }
#   endif
}

template<class T, class C>
template<class U>
inline void
CWX<T,C>::labeledCellGrid(
    Marray<U>& out
) const
{
    const size_t arrayShape[] = {
        2 * shape(0) - 1, 
        2 * shape(1) - 1, 
        2 * shape(2) - 1
    };
    out.resize(arrayShape, arrayShape + 3);
    labeledCellGrid(static_cast<View<U>&>(out));
}

template<class T, class C>
template<class U>
void
CWX<T,C>::labeledCellGrid(
    View<U>& out
) const
{
    assert(out.dimension() == 3);
    assert(out.shape(0) == shape(0) * 2 - 1);
    assert(out.shape(1) == shape(0) * 2 - 1);
    assert(out.shape(2) == shape(0) * 2 - 1);
    detail::ExportLabeler<Label, Coordinate, U> exportLabeler(*this, out);
    for(Order order = 0; order <= 3; ++order) {
        for(Label label = 1; label <= numberOfCells(order); ++label) {
            exportLabeler.setLabel(label);
            process(order, label, exportLabeler);
        }
    }
}

template<class T, class C>
template<class U>
void
CWX<T,C>::labeledCellSlice(
    const Order d,
    const Coordinate v,
    Marray<U>& out
) const
{
    // TODO: implement
}

template<class T, class C>
template<class U>
void
CWX<T,C>::labeledCellSlice(
    const Order d,
    const Coordinate v,
    View<U>& out
) const
{
    // TODO: implement
}

template<class T, class C>
template<class U>
inline void
CWX<T,C>::labeledVoxelGrid(
    Marray<U>& out
) const
{
    const size_t arrayShape[] = {shape(0), shape(1), shape(2)};
    out.resize(arrayShape, arrayShape + 3);
    labeledVoxelGrid(static_cast<View<U>&>(out));
}

template<class T, class C>
template<class U>
void
CWX<T,C>::labeledVoxelGrid(
    View<U>& out
) const
{
    assert(out.dimension() == 3);
    assert(out.shape(0) == shape(0));
    assert(out.shape(1) == shape(0));
    assert(out.shape(2) == shape(0));
    detail::ExportVoxelLabeler<Label, Coordinate, U> exportVoxelLabeler(*this, out);
    for(Label label = 1; label <= numberOfCells(3); ++label) {
        exportVoxelLabeler.setLabel(label);
        process(3, label, exportVoxelLabeler);
    }
}

namespace detail {

template<class T, class C>
inline
Labeler<T, C>::Labeler(
    CWXType& cwx,
    const Order order
)
:   cwx_(cwx),
    order_(order)
{
    assert(order != 0); // this labeler is not suitable for 0-cells
}

template<class T, class C>
inline bool
Labeler<T, C>::preprocess(
    const CellType& cell
)
{
    assert(cell.order() == order_);
    label_ = cwx_.cwcomplex_.push_back(order_);
    cwx_.connect(cell, label_);
    cwx_.byteLabeledCellgrid_.anchor(cell, true);
    const Label sameLabel = cwx_.anchorage_.push_back(cell);
    assert(label_ == sameLabel);
    return true;
}

template<class T, class C>
inline bool
Labeler<T, C>::operator()(
    const CellType& cell
)
{
    assert(cell.order() == order_);
    if(cwx_.redundantAnchors_ && order_ == 1) {
        // every 1-cell of a connected component of 1-cells becomes an anchor
        cwx_.byteLabeledCellgrid_.anchor(cell, true);
        cwx_.anchorage_.anchor(cell, label_);
        // TODO: collect surrounding 2-cell labels at this anchor
    }
    return true;
}

template<class T, class C>
inline bool
Labeler<T, C>::postprocess()
{
    return true;
}

template<class T, class C>
inline
Anchorer<T, C>::Anchorer(
    CWXType& cwx
)
:   cwx_(cwx),
    anchor_(0,0,0),
    label_(0),
    anchorFound_(false)
{}

template<class T, class C>
inline bool
Anchorer<T, C>::preprocess(
    const CellType& cell
)
{
    anchorFound_ = false;
    label_ = 0;
    anchor_ = cell; // candidate if no other anchor is found (*)
    return true;
}

template<class T, class C>
inline bool
Anchorer<T, C>::operator()(
    const CellType& cell
)
{
    if(label_ == 0 && cwx_.byteLabeledCellgrid_.isAnchored(cell)) {
        if(!anchorFound_) {
            anchor_ = cell;
            anchorFound_ = true;
            label_ = cwx_.anchorage_.anchor(cell);
        }
        else {
            label_ = cwx_.anchorage_.anchor(cell);
            if(label_ != 0) {
                anchor_ = cell;
            }
        }
    }
    return true;
}

template<class T, class C>
inline bool
Anchorer<T, C>::postprocess()
{
    // add anchor if necessary
    if(label_ == 0) { // if no labeled anchor exists in this slice
        label_ = cwx_.atCell(anchor_); // look the label up by searching the grid
        assert(label_ != 0);
        if(anchorFound_) {
            /*
            std::cout << "labeling existing anchor of " << int(anchor_.order())
                      << "-component " << label_
                      << " at (" << anchor_[0]
                      << "," << anchor_[1]
                      << "," << anchor_[2] << ")" << std::endl;
            */
            cwx_.anchorage_.anchor(anchor_, label_);
        }
        else {
            /*
            std::cout << "introducing new anchor for " << int(anchor_.order())
                      << "-component " << label_
                      << " at (" << anchor_[0]
                      << "," << anchor_[1]
                      << "," << anchor_[2] << ")" << std::endl;
            */
            cwx_.byteLabeledCellgrid_.anchor(anchor_, true);
            cwx_.anchorage_.anchor(anchor_, label_);
        }
    }
    return true;
}

template<class T, class C>
inline
AnchorTester<T, C>::AnchorTester(
    const CWXType& cwx
)
:   cwx_(cwx),
    labeledAnchorFound_(false)
{}

template<class T, class C>
inline bool
AnchorTester<T, C>::preprocess(
    const CellType& cell
)
{
    labeledAnchorFound_ = false;
    return true;
}

template<class T, class C>
inline bool
AnchorTester<T, C>::operator()(
    const CellType& cell
)
{
    if(cwx_.byteLabeledCellgrid_.isAnchored(cell) && cwx_.anchorage_.anchor(cell) != 0) {
        labeledAnchorFound_ = true;
    }
    return true;
}

template<class T, class C>
inline bool
AnchorTester<T, C>::postprocess()
{
    assert(labeledAnchorFound_);
    return true;
}

template<class T, class C, class U>
inline 
ExportLabeler<T, C, U>::ExportLabeler(
    const CWXType& cwx, 
    ViewType& view
)
:   cwx_(cwx),
    view_(view),
    label_(ExportLabel())
{
    assert(view.dimension() == 3);
    assert(view.shape(0) == cwx.shape(0) * 2 - 1);
    assert(view.shape(1) == cwx.shape(1) * 2 - 1);
    assert(view.shape(2) == cwx.shape(2) * 2 - 1);
}

template<class T, class C, class U>
inline void
ExportLabeler<T, C, U>::setLabel(
    const ExportLabel label
) {
    label_ = label;
}

template<class T, class C, class U>
inline bool
ExportLabeler<T, C, U>::operator()(
    const CellType& cell
) {
    assert(cell[0] < view_.shape(0));
    assert(cell[1] < view_.shape(1));
    assert(cell[2] < view_.shape(2));
    view_(cell[0], cell[1], cell[2]) = label_;
    return true;
}

template<class T, class C, class U>
inline 
ExportVoxelLabeler<T, C, U>::ExportVoxelLabeler(
    const CWXType& cwx, 
    ViewType& view
)
:   cwx_(cwx),
    view_(view),
    label_(ExportLabel())
{
    assert(view.dimension() == 3);
    assert(view.shape(0) == cwx.shape(0));
    assert(view.shape(1) == cwx.shape(1));
    assert(view.shape(2) == cwx.shape(2));
}

template<class T, class C, class U>
inline void
ExportVoxelLabeler<T, C, U>::setLabel(
    const ExportLabel label
) {
    label_ = label;
}

template<class T, class C, class U>
inline bool
ExportVoxelLabeler<T, C, U>::operator()(
    const CellType& cell
) {
    assert(cell.order() == 3);
    assert(cell[0] / 2 < view_.shape(0));
    assert(cell[1] / 2 < view_.shape(1));
    assert(cell[2] / 2 < view_.shape(2));
    view_(cell[0] / 2, cell[1] / 2, cell[2] / 2) = label_;
    return true;
}

} // namespace detail

} // namespace andres
} // namespace cwx

#endif // #ifndef ANDRES_CWX_HXX
