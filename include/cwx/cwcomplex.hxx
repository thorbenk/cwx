#pragma once
#ifndef CWX_CWCOMPLEX_HXX
#define CWX_CWCOMPLEX_HXX

#include <cassert>
#include <vector>
#include <array>
#include <stdexcept>
#include <algorithm> // find
#include <string>
#include <sstream>

namespace cwx {

// up to order 3.
// all labels start at 1.
template<class T>
class CWComplex {
public:
    typedef T Label;
    typedef unsigned char Order;

    CWComplex();
    CWComplex(const Label, const Label, const Label, const Label);
    void reserve(const Order, const Label);

    // query
    Label numberOfCells(const Order) const;
    size_t sizeAbove(const Order, const Label) const;
    size_t sizeBelow(const Order, const Label) const;
    Label above(const Order, const Label, const size_t) const;
    Label below(const Order, const Label, const size_t) const;

    // manipulation
    Label push_back(const Order);
    void connect(const Order, const Label, const Label);    

private:
    template<class CONTAINER> void insertHelper(CONTAINER&, const Label) const;
    template<class CONTAINER> void insertHelper2(CONTAINER&, const Label) const;
    template<class CONTAINER> void testHelper(const CONTAINER&) const;
    template<class CONTAINER1, class CONTAINER2>
        void testHelper2(const CONTAINER1&, const CONTAINER2&) const;
    void testInvariant() const;

    // above0_[p][q] is the q-th 1-cell bounded by the 0-cell p
    std::vector<std::array<Label, 6> > above0_;
    std::vector<std::array<Label, 4> > above1_;
    std::vector<std::array<Label, 2> > above2_;

    // below1_[p][q] is the q-th 0-cell that bounds the 1-cell p
    // (the following members encode the inverse relation of the above)
    std::vector<std::array<Label, 2> > below1_;
    std::vector<std::vector<Label> > below2_;
    std::vector<std::vector<Label> > below3_;
};

template<class T>
inline
CWComplex<T>::CWComplex()
: above0_(1),
  above1_(1),
  above2_(1),
  below1_(1),
  below2_(1),
  below3_(1)
{
    above0_[0].fill(0);
    above1_[0].fill(0);
    above2_[0].fill(0);
    below1_[0].fill(0);
    testInvariant();
}

template<class T>
inline
CWComplex<T>::CWComplex(
    const typename CWComplex<T>::Label numberOfCells0,
    const typename CWComplex<T>::Label numberOfCells1,
    const typename CWComplex<T>::Label numberOfCells2,
    const typename CWComplex<T>::Label numberOfCells3
)
: above0_(numberOfCells0 + 1),
  above1_(numberOfCells1 + 1),
  above2_(numberOfCells2 + 1),
  below1_(numberOfCells1 + 1),
  below2_(numberOfCells2 + 1),
  below3_(numberOfCells3 + 1)
{
    above0_[0].fill(0);
    above1_[0].fill(0);
    above2_[0].fill(0);
    below1_[0].fill(0);
    testInvariant();
}

template<class T>
inline void
CWComplex<T>::reserve(
    const typename CWComplex<T>::Order order,
    const typename CWComplex<T>::Label numberOfCells
)
{
    switch(order) {
    case 0:
        above0_.reserve(numberOfCells + 1);
        break;
    case 1:
        above1_.reserve(numberOfCells + 1);
        below1_.reserve(numberOfCells + 1);
        break;
    case 2:
        above2_.reserve(numberOfCells + 1);
        below2_.reserve(numberOfCells + 1);
        break;
    case 3:
        below3_.reserve(numberOfCells + 1);
        break;
    default:
        throw std::runtime_error("invalid order");
    }
}

template<class T>
inline typename CWComplex<T>::Label
CWComplex<T>::numberOfCells(
    const typename CWComplex<T>::Order order
) const
{
    switch(order) {
    case 0:
        return static_cast<Label>(above0_.size() - 1);
    case 1:
        return static_cast<Label>(above1_.size() - 1);
    case 2:
        return static_cast<Label>(above2_.size() - 1);
    case 3:
        return static_cast<Label>(below3_.size() - 1);
    default:
        throw std::runtime_error("invalid order");
    }
}

template<class T>
inline size_t
CWComplex<T>::sizeAbove(
    const typename CWComplex<T>::Order order,
    const typename CWComplex<T>::Label label
) const
{
    assert(label > 0 && label <= numberOfCells(order));
    switch(order) {
    case 0:
        assert(label <= numberOfCells(0));
        for(size_t j=0; j<6; ++j) {
            if(above0_[label][j] == 0) {
                return j;
            }
        }
        return 6;
    case 1:
        assert(label <= numberOfCells(1));
        for(size_t j=0; j<4; ++j) {
            if(above1_[label][j] == 0) {
                return j;
            }
        }
        return 4;
    case 2:
        assert(label <= numberOfCells(2));
        return 2;
    case 3:
        assert(label <= numberOfCells(3));
        return 0;
    default:
        throw std::runtime_error("invalid order");
    }
}

template<class T>
inline size_t
CWComplex<T>::sizeBelow(
    const typename CWComplex<T>::Order order,
    const typename CWComplex<T>::Label label
) const
{
    assert(label > 0 && label <= numberOfCells(order));
    switch(order) {
    case 0:
        assert(label <= numberOfCells(0));
        return 0;
    case 1:
        assert(label <= numberOfCells(1));
        if(below1_[label][0] == 0) {
            return 0;
        }
        else if(below1_[label][1] == 0) {
            return 1;
        }
        else {
            return 2;
        }
    case 2:
        assert(label <= numberOfCells(2));
        return below2_[label].size();
    case 3:
        assert(label <= numberOfCells(3));
        return below3_[label].size();
    default:
        throw std::runtime_error("invalid order");
    }
}

// returns 0 if j >= sizeAbove(order, label)
// this behavior is
// - intented to allow for iterations over j until above(order, label, j) == 0
// - slightly different from that of below()
template<class T>
inline typename CWComplex<T>::Label
CWComplex<T>::above(
    const typename CWComplex<T>::Order order,
    const typename CWComplex<T>::Label label,
    const size_t j
) const
{
    assert(label > 0 && label <= numberOfCells(order));
    switch(order) {
    case 0:
        assert(label <= numberOfCells(0));
        assert(j < 6);
        return above0_[label][j];
    case 1:
        assert(label <= numberOfCells(1));
        assert(j < 4);
        return above1_[label][j];
    case 2:
        assert(label <= numberOfCells(2));
        assert(j < 2);
        return above2_[label][j];
    case 3:
        throw std::runtime_error("order 3 is not applicable here");
    default:
        throw std::runtime_error("invalid order");
    }
}

// assertion fails if j >= sizeBelow(order, label)
// this behavior is slightly different from that of above()
template<class T>
inline typename CWComplex<T>::Label
CWComplex<T>::below(
    const typename CWComplex<T>::Order order,
    const typename CWComplex<T>::Label label,
    const size_t j
) const
{
    assert(label > 0 && label <= numberOfCells(order));
    switch(order) {
    case 0:
        throw std::runtime_error("order 0 is not applicable here");
    case 1:
        assert(label <= numberOfCells(1));
        assert(j < sizeBelow(order, label));
        return below1_[label][j];
    case 2:
        assert(label <= numberOfCells(2));
        assert(j < sizeBelow(order, label));
        return below2_[label][j];
    case 3:
        assert(label <= numberOfCells(3));
        assert(j < sizeBelow(order, label));
        return below3_[label][j];
    default:
        throw std::runtime_error("invalid order");
    }
}

template<class T>
inline typename CWComplex<T>::Label
CWComplex<T>::push_back(
    const typename CWComplex<T>::Order order
)
{
    switch(order) {
    case 0:
        above0_.push_back(above0_[0]);
        break;
    case 1:
        above1_.push_back(above1_[0]);
        below1_.push_back(below1_[0]);
        break;
    case 2:
        above2_.push_back(above2_[0]);
        below2_.push_back(below2_[0]);
        break;
    case 3:
        below3_.push_back(below3_[0]);
        break;
    default:
        throw std::runtime_error("invalid order");
    }
    testInvariant();
    return numberOfCells(order);
}

template<class T>
inline void
CWComplex<T>::connect(
    const typename CWComplex<T>::Order order,
    const typename CWComplex<T>::Label label,
    const typename CWComplex<T>::Label labelAbove
)
{
    assert(order < 3);
    assert(label > 0 && label <= numberOfCells(order));
    assert(labelAbove > 0 && labelAbove <= numberOfCells(order + 1));
    switch(order) {
    case 0:
        assert(label <= numberOfCells(0));
        assert(labelAbove <= numberOfCells(1));
        insertHelper(above0_[label], labelAbove); // connect upwards
        insertHelper(below1_[labelAbove], label); // connect downwards
        break;
    case 1:
        assert(label <= numberOfCells(1));
        assert(labelAbove <= numberOfCells(2));
        insertHelper(above1_[label], labelAbove); // connect upwards
        insertHelper2(below2_[labelAbove], label); // connect downwards
        break;
    case 2:
        assert(label <= numberOfCells(2));
        assert(labelAbove <= numberOfCells(3));
        insertHelper(above2_[label], labelAbove); // connect upwards
        insertHelper2(below3_[labelAbove], label); // connect downwards
        break;
    case 3:
        throw std::runtime_error("order 3 is not applicable here");
    default:
        throw std::runtime_error("invalid order");
    }
    testInvariant();
}

// insert into fixed-size container whose entries are and are supposed to remain
// unique and in ascending order, except for, possibly, a terminal sequence of
// zeros indicating free spots
template<class T>
template<class CONTAINER>
inline void
CWComplex<T>::insertHelper(
    CONTAINER& container,
    const typename CWComplex<T>::Label label
) const
{
    for(size_t j=0; j<container.size(); ++j) {
        if(container[j] == 0) {
            container[j] = label;
            return;
        }
        else if(container[j] > label) {
            assert(container[container.size()-1] == 0);
            // shift everything beyong j to the right:
            for(size_t k=container.size()-1; k>j; --k) {
                container[k] = container[k-1];
            }
            // insert labelAbove at position j:
            container[j] = label;
            return;
        }
        else if(container[j] == label) { // label is already in container
            return;
        }
    }
    throw std::runtime_error("insert failed. container is full.");
}

// insert into container with const_iterator and member function insert and
// whose entries are and are supposed to remain unique and in ascending order
template<class T>
template<class CONTAINER>
inline void
CWComplex<T>::insertHelper2(
    CONTAINER& container,
    const typename CWComplex<T>::Label label
) const
{
    typename CONTAINER::iterator it = container.begin();
    while(it != container.end() && *it != 0 && *it < label) {
        ++it;
    }
    if(it == container.end() || *it != label) {
        container.insert(it, label);
    }
}

// tests for a container with member functions size and operator[] if the
// entries are in ascending order, except for, possibly, a terminal sequence of
// zeros
template<class T>
template<class CONTAINER>
inline void
CWComplex<T>::testHelper(
    const CONTAINER& container
) const
{
    if(container.size() == 0) {
        return;
    }
    else {
        bool expectZeros = (container[0] == 0);
        for(size_t k=1; k<container.size(); ++k) {
            if(expectZeros) {
                assert(container[k] == 0);
            }
            else {
                if(container[k] == 0) {
                    expectZeros = true;
                }
                else {
                    assert(container[k] > container[k-1]);
                }
            }
        }
    }
}

// checks for all valid labels and all labelAbove == container1[label][k]
// if there exists an m such that container2[labelAbove][m] == label
template<class T>
template<class CONTAINER1, class CONTAINER2>
inline void
CWComplex<T>::testHelper2(
    const CONTAINER1& container1,
    const CONTAINER2& container2
) const
{
    for(size_t label=0; label<container1.size(); ++label) {
        for(size_t k=0; k<container1[label].size(); ++k) {
            const size_t labelAbove = container1[label][k];
            if(labelAbove == 0) {
                break;
            }
            else {
                assert(std::find(container2[labelAbove].begin(),
                    container2[labelAbove].end(), label)
                    != container2[labelAbove].end());
            }
        }
    }
}

template<class T>
inline void
CWComplex<T>::testInvariant() const
{
#ifndef NDEBUG
    // test for existence of the special reserved label 0
    assert(above0_.size() > 0);
    assert(above1_.size() > 0);
    assert(above2_.size() > 0);
    assert(below1_.size() > 0);
    assert(below2_.size() > 0);
    assert(below3_.size() > 0);
    for(size_t j=0; j<6; ++j) {
        assert(above0_[0][j] == 0);
    }
    for(size_t j=0; j<4; ++j) {
        assert(above1_[0][j] == 0);
    }
    for(size_t j=0; j<2; ++j) {
        assert(above2_[0][j] == 0);
    }
    assert(below1_[0][0] == 0);
    assert(below1_[0][1] == 0);
    assert(below2_[0].size() == 0);
    assert(below3_[0].size() == 0);

    // test bounding relations
    // 1. test if entries are unique and in ascending order, except for,
    //    possibly, a terminal sequence of zeros indicating free spots
    for(size_t j=1; j<above0_.size(); ++j)
        testHelper(above0_[j]);
    for(size_t j=1; j<above1_.size(); ++j)
        testHelper(above1_[j]);
    for(size_t j=1; j<above2_.size(); ++j)
        testHelper(above2_[j]);
    for(size_t j=1; j<below1_.size(); ++j)
        testHelper(below1_[j]);
    for(size_t j=1; j<below2_.size(); ++j)
        testHelper(below2_[j]);
    for(size_t j=1; j<below3_.size(); ++j)
        testHelper(below3_[j]);
    // 2. test if bounding relations are consistent with their inverses
    assert(below1_.size() == above1_.size());
    assert(below2_.size() == above2_.size());
    testHelper2(above0_, below1_);
    testHelper2(below1_, above0_);
    testHelper2(above1_, below2_);
    testHelper2(below2_, above1_);
    testHelper2(above2_, below3_);
    testHelper2(below3_, above2_);
#endif
}

} // namespace cwx

#endif // #ifndef CWX_CWCOMPLEX_HXX
