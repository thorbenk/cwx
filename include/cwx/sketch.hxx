#pragma once
#ifndef CWX_SKETCH_HXX
#define CWX_SKETCH_HXX

#include <iostream>
#include <algorithm> // std::sort

#include "cwx/cell.hxx"

namespace cwx {

// this functor can be used with CWX::process
// TODO: provide ostream as optional parameter to constructor
template<class C>
class Sketch {
public:
    typedef C Coordinate;
    typedef Cell<Coordinate> CellType;
    typedef typename CellType::Order Order;
    enum ThreeCellDrawMode {Small, Full};

    Sketch(std::ostream& = std::cout);
    ~Sketch();

    bool operator()(const CellType&);
    void scale(const float);
    void color(const Order, const float, const float, const float, const float = 1.0f);
    void threeCellDrawMode(const ThreeCellDrawMode);
    void maxCoordinate(const Coordinate, const Coordinate, const Coordinate);
    void grid(const bool);
    void axes(const bool);

private:
    std::ostream& stream_;
    typename CellType::Corners corners_; // memory to play with
    size_t colors_[4]; // of 0-cells, 1-cells and 2-cells
    float scale_;
    ThreeCellDrawMode threeCellDrawMode_;
    bool withAxes_;
    bool withGrid_;
    Coordinate maxCoordinate_[3];
};

template<class C>
inline
Sketch<C>::Sketch(
    std::ostream& stream
)
:   stream_(stream),
    corners_(),
    scale_(1.0f),
    threeCellDrawMode_(Small),
    withAxes_(true),
    withGrid_(true)
{
    colors_[0] = 0;
    colors_[1] = 0;
    colors_[2] = 0;
    colors_[3] = 0;
    maxCoordinate_[0] = 0;
    maxCoordinate_[1] = 0;
    maxCoordinate_[2] = 0;
    // header
    stream_ << "def cwx {" << std::endl
            << "def s0 0.1" << std::endl; // size of 0-cells
    color(0, 0.0f, 0.0f, 0.0f);
    color(1, 0.0f, 0.0f, 1.0f);
    color(2, 0.0f, 1.0f, 0.0f);
    color(3, 1.0f, 0.0f, 0.0f);
}

template<class C>
inline
Sketch<C>::~Sketch()
{
    // footer
    stream_ << "}" << std::endl
            << "def eye (" << maxCoordinate_[0]
            << ", " << 0.5f * maxCoordinate_[1]
            << ", " << 1.5f * maxCoordinate_[2]
            << ")" << std::endl;

    if(withGrid_ || withAxes_) {
        stream_ << "def axes {" << std::endl;
        if(withGrid_) {
            for(unsigned char d = 0; d < 3; ++d) {
                Coordinate from[] = {0, 0, 0};
                for(Coordinate k = 1; k < 3; ++k) {
                    Coordinate to[] = {maxCoordinate_[0], maxCoordinate_[1], maxCoordinate_[2]};
                    for(Coordinate j = 0; j <= maxCoordinate_[d]; ++j) {
                        from[d] = j;
                        to[d] = j;
                        to[(d + k) % 3] = 0;
                        stream_ << "line[style=thin,color=black](" << from[0]
                                << "," << from[1]
                                << "," << from[2]
                                << ")(" << to[0]
                                << "," << to[1]
                                << "," << to[2]
                                << ")" << std::endl;
                    }
                }
            }
        }
        if(withAxes_) {
            stream_ << "line[draw=black,line width=1pt,arrows=-latex](0,0,0)(" << maxCoordinate_[0] + 1 << ",0,0)" << std::endl
                    << "line[draw=black,line width=1pt,arrows=-latex](0,0,0)(0," << maxCoordinate_[1] + 1<< ",0)" << std::endl
                    << "line[draw=black,line width=1pt,arrows=-latex](0,0,0)(0,0," << maxCoordinate_[2] + 1 << ")" << std::endl;
                    /*
                    << "special|\\node at #1 {$x$};|[lay=under](" << maxCoordinate_[0] + 1.5f << ",0,0)" << std::endl
                    << "special|\\node at #1 {$y$};|[lay=under](0," << maxCoordinate_[1] + 1.5f << ",0)" << std::endl
                    << "special|\\node at #1 {$z$};|[lay=under](0,0," << maxCoordinate_[2] + 1.5f << ")" << std::endl;
                    */
        }
        stream_ << "}" << std::endl
                << "put {view((eye), (0,0,0)) then scale(" << scale_
                << ")}{axes}" << std::endl;
    }
    stream_ << "put {view((eye), (0,0,0)) then scale(" << scale_
            << ")} {cwx}" << std::endl
            << "global { language tikz }" << std::endl;
}

template<class C>
inline void
Sketch<C>::color(
    const Order order,
    const float r,
    const float g,
    const float b,
    const float opacity
)
{
    assert(r >= 0.0 && r <= 1.0);
    assert(g >= 0.0 && g <= 1.0);
    assert(b >= 0.0 && b <= 1.0);
    assert(opacity >= 0.0 && opacity <= 1.0);
    ++colors_[order];
    switch(order) {
    case 0:
        stream_ << "special|\\definecolor{color0c" << colors_[0]
                << "}{rgb}{" << r << "," << g << "," << b << "}|[lay=under]" << std::endl
                << "special|\\tikzstyle{line0c" << colors_[0]
                << "} = [draw=color0c" << colors_[0]
                << "!80!black]|[lay=under]" << std::endl
                << "special|\\tikzstyle{fill0c" << colors_[0]
                << "} = [fill=color0c" << colors_[0]
                << "]|[lay=under]" << std::endl;
        break;
    case 1:
        stream_ << "special|\\definecolor{color1c" << colors_[1]
                << "}{rgb}{" << r << "," << g << "," << b << "}|[lay=under]" << std::endl
                << "special|\\tikzstyle{line1c" << colors_[1]
                << "}=[draw=color1c" << colors_[1]
                << ",line width=3pt]|[lay=under]" << std::endl;
        break;
    case 2:
        stream_ << "special|\\definecolor{color2c" << colors_[2]
                << "}{rgb}{" << r << "," << g << "," << b << "}|[lay=under]" << std::endl
                << "special|\\tikzstyle{line2c" << colors_[2]
                << "}=[draw=color2c" << colors_[2]
                << "!20!black]|[lay=under]" << std::endl
                << "special|\\tikzstyle{fill2c" << colors_[2]
                << "}=[fill=color2c" << colors_[2]
                << ",opacity=" << opacity << "]|[lay=under]" << std::endl;
        break;
    case 3:
        stream_ << "special|\\definecolor{color3c" << colors_[3]
                << "}{rgb}{" << r << "," << g << "," << b << "}|[lay=under]" << std::endl
                << "special|\\tikzstyle{line3c" << colors_[3]
                << "} = [draw=color3c" << colors_[3]
                << "!80!black]|[lay=under]" << std::endl
                << "special|\\tikzstyle{fill3c" << colors_[3]
                << "} = [fill=color3c" << colors_[3]
                << ",opacity=" << opacity << "]|[lay=under]" << std::endl;
        break;
    }
}

template<class C>
bool
Sketch<C>::operator()(
    const CellType& cell
)
{
    cell.corners(corners_);
    for(size_t j=0; j<corners_.size(); ++j) {
        for(size_t k=0; k<3; ++k) {
            if(maxCoordinate_[k] < corners_[j][k]) {
                maxCoordinate_[k] = corners_[j][k];
            }
        }
    }
    switch(cell.order()) {
    case 0:
        assert(corners_.size() == 1);        
        // cheap on the latex side:
        /*
        stream_ << "dots[fill=color0c" << colors_[0]
                << ",dotsize=5pt](" << corners_[0][0]
                << "," << corners_[0][1]
                << "," << corners_[0][2]
                << ")" << std::endl;
        */
        // expensive on the latex side:
        stream_ << "put { translate([" << corners_[0][0]
                << "," << corners_[0][1]
                << "," << corners_[0][2]
                << "]) } {" << std::endl
                << "polygon[line style=line0c" << colors_[0]
                << ",fill style=fill0c" << colors_[0]
                << "](-s0,-s0,-s0)(s0,-s0,-s0)(s0,s0,-s0)(-s0,s0,-s0)" << std::endl
                << "polygon[line style=line0c" << colors_[0]
                << ",fill style=fill0c" << colors_[0]
                << "](-s0,-s0,-s0)(-s0,s0,-s0)(-s0,s0,s0)(-s0,-s0,s0)" << std::endl
                << "polygon[line style=line0c" << colors_[0]
                << ",fill style=fill0c" << colors_[0]
                << "](-s0,-s0,-s0)(-s0,-s0,s0)(s0,-s0,s0)(s0,-s0,-s0)" << std::endl
                << "polygon[line style=line0c" << colors_[0]
                << ",fill style=fill0c" << colors_[0]
                << "](s0,s0,s0)(-s0,s0,s0)(-s0,-s0,s0)(s0,-s0,s0)" << std::endl
                << "polygon[line style=line0c" << colors_[0]
                << ",fill style=fill0c" << colors_[0]
                << "](s0,s0,s0)(s0,-s0,s0)(s0,-s0,-s0)(s0,s0,-s0)" << std::endl
                << "polygon[line style=line0c" << colors_[0]
                << ",fill style=fill0c" << colors_[0]
                << "](s0,s0,s0)(s0,s0,-s0)(-s0,s0,-s0)(-s0,s0,s0)" << std::endl
                << "}" << std::endl;
        break;
    case 1:
        assert(corners_.size() == 2);
        stream_ << "line[line style=line1c" << colors_[1] << "]("
                << corners_[0][0] << ","
                << corners_[0][1] << ","
                << corners_[0][2] << ")("
                << corners_[1][0] << ","
                << corners_[1][1] << ","
                << corners_[1][2] << ")" << std::endl;
        break;
    case 2:
        // the correctness of this part depends on the order in which the
        // vertices are output by the function Cell<Coordinate>::corners
        assert(corners_.size() == 4);
        stream_ << "polygon[line style=line2c" << colors_[2]
                << ",fill style=fill2c" << colors_[2] << "]("
                << corners_[0][0] << ","
                << corners_[0][1] << ","
                << corners_[0][2] << ")("
                << corners_[1][0] << ","
                << corners_[1][1] << ","
                << corners_[1][2] << ")("
                << corners_[2][0] << ","
                << corners_[2][1] << ","
                << corners_[2][2] << ")("
                << corners_[3][0] << ","
                << corners_[3][1] << ","
                << corners_[3][2] << ")" << std::endl;
        break;
    case 3:
        if(threeCellDrawMode_ == Small) {
            stream_ << "put { translate([" << 0.5f + corners_[0][0]
                    << "," << 0.5f + corners_[0][1]
                    << "," << 0.5f + corners_[0][2]
                    << "]) } {" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](-s0,-s0,-s0)(s0,-s0,-s0)(s0,s0,-s0)(-s0,s0,-s0)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](-s0,-s0,-s0)(-s0,s0,-s0)(-s0,s0,s0)(-s0,-s0,s0)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](-s0,-s0,-s0)(-s0,-s0,s0)(s0,-s0,s0)(s0,-s0,-s0)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](s0,s0,s0)(-s0,s0,s0)(-s0,-s0,s0)(s0,-s0,s0)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](s0,s0,s0)(s0,-s0,s0)(s0,-s0,-s0)(s0,s0,-s0)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](s0,s0,s0)(s0,s0,-s0)(-s0,s0,-s0)(-s0,s0,s0)" << std::endl
                    << "}" << std::endl;
        }
        else if(threeCellDrawMode_ == Full) {
            stream_ << "put { translate([" << 0.5f + corners_[0][0]
                    << "," << 0.5f + corners_[0][1]
                    << "," << 0.5f + corners_[0][2]
                    << "]) } {" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](-0.5,-0.5,-0.5)(0.5,-0.5,-0.5)(0.5,0.5,-0.5)(-0.5,0.5,-0.5)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](-0.5,-0.5,-0.5)(-0.5,0.5,-0.5)(-0.5,0.5,0.5)(-0.5,-0.5,0.5)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](-0.5,-0.5,-0.5)(-0.5,-0.5,0.5)(0.5,-0.5,0.5)(0.5,-0.5,-0.5)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](0.5,0.5,0.5)(-0.5,0.5,0.5)(-0.5,-0.5,0.5)(0.5,-0.5,0.5)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](0.5,0.5,0.5)(0.5,-0.5,0.5)(0.5,-0.5,-0.5)(0.5,0.5,-0.5)" << std::endl
                    << "polygon[line style=line3c" << colors_[3]
                    << ",fill style=fill3c" << colors_[3]
                    << "](0.5,0.5,0.5)(0.5,0.5,-0.5)(-0.5,0.5,-0.5)(-0.5,0.5,0.5)" << std::endl
                    << "}" << std::endl;
        }
        break;
    }
    return true;
}

template<class C>
inline void
Sketch<C>::scale(
    const float scale
)
{
    scale_ = scale;
}

template<class C>
inline void
Sketch<C>::threeCellDrawMode(
    const ThreeCellDrawMode mode
)
{
    threeCellDrawMode_ = mode;
}

template<class C>
inline void
Sketch<C>::maxCoordinate(
    const Coordinate x,
    const Coordinate y,
    const Coordinate z
)
{
    maxCoordinate_[0] = x;
    maxCoordinate_[1] = x;
    maxCoordinate_[2] = x;
    // will be over-written if processed cells have greater coordinates
}

template<class C>
inline void
Sketch<C>::grid(
    const bool enable
)
{
    withGrid_ = enable;
}

template<class C>
inline void
Sketch<C>::axes(
    const bool enable
)
{
    withAxes_ = enable;
}

} // namespace cwx

#endif // CWX_SKETCH_HXX
