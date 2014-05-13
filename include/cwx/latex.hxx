#pragma once
#ifndef ANDRES_CWX_LATEX_HXX
#define ANDRES_CWX_LATEX_HXX

#include <vector>
#include <array>
#include <iostream>
#include <random>
#include <string>

#include "cwx/cwcomplex.hxx"
#include "cwx/cwx.hxx"

namespace andres {
namespace cwx {

template<class T>
class CWComplexLatex {
public:
    typedef T Label;
    typedef CWComplex<Label> CWComplexType;
    typedef typename CWComplexType::Order Order;
    typedef std::array<float, 3> Color;

    template<class C> CWComplexLatex(const CWX<Label, C>&, std::ostream& = std::cout);
    CWComplexLatex(const CWComplexType&, std::ostream& = std::cout);
    void color(const Order, const Label, const float, const float, const float);
    void text(const Order, const std::string&);
    void write() const;

private:
    void randomizeColors();
    void standardText();

    const CWComplexType& cwcomplex_;
    std::ostream& stream_;
    std::array<std::vector<Color>, 4> colors_;
    std::array<std::string, 4> text_;
};

template<class T>
inline
CWComplexLatex<T>::CWComplexLatex(
    const CWComplexType& cwcomplex,
    std::ostream& stream
)
:   cwcomplex_(cwcomplex),
    stream_(stream),
    colors_(),
    text_()
{    
    randomizeColors();
    standardText();
}

template<class T>
template<class C>
inline
CWComplexLatex<T>::CWComplexLatex(
    const CWX<T, C>& cwx,
    std::ostream& stream
)
:   cwcomplex_(cwx.cwcomplex_),
    stream_(stream),
    colors_(),
    text_()
{
    randomizeColors();
    standardText();
}

template<class T>
inline void
CWComplexLatex<T>::color(
    const Order order,
    const Label label,
    const float r,
    const float g,
    const float b
)
{
    colors_[order][label][0] = r;
    colors_[order][label][1] = g;
    colors_[order][label][2] = b;
}

template<class T>
inline void
CWComplexLatex<T>::text(
    const Order order,
    const std::string& txt
)
{
    text_[order] = txt;
}

template<class T>
void
CWComplexLatex<T>::write() const
{
    stream_ << "% make sure to \\usepackage{tikz} " << std::endl
            << "\\begin{tikzpicture}" << std::endl
            << "\\tikzstyle{edge}=[draw,thick,-]" << std::endl;
    for(Order order = 0; order < 4; ++order) {
        const unsigned int orderPrint = order;
        for(size_t label=1; label <= cwcomplex_.numberOfCells(order); ++label) {
            stream_ << "\\definecolor{color" << orderPrint
                    << "-" << label
                    << "}{rgb}{" << colors_[order][label][0]
                    << "," << colors_[order][label][1]
                    << "," << colors_[order][label][2]
                    << "}" << std::endl
                    << "\\tikzstyle{node" << orderPrint
                    << "-" << label
                    << "}=[circle,scale=0.5,fill=color" << orderPrint
                    << "-" << label
                    << "]" << std::endl;
        }
    }

    // nodes
    stream_ << "\\matrix[column sep=2ex,row sep=0.7ex]{" << std::endl;
    for(unsigned char j = 0; j < 4; ++j) {
        const unsigned char order = 3 - j;
        const unsigned int orderPrint = order;
        stream_ << "\\node[left] {" << text_[order] << "}; &" << std::endl;
        for(size_t label = 1; label <= cwcomplex_.numberOfCells(order); ++label) {
            std::stringstream previous;
            if(label == 1) {
                previous << "]";
            }
            else {
                previous << ",right of=o" << orderPrint
                         << "c" << label - 1
                         << "]";
            }
            stream_ << "\\node [node" << orderPrint
              << "-" << label
              << previous.str()
              << " (o" << orderPrint
              << "c" << label
              << ") {};" << std::endl;
        }
        stream_ << "\\\\" << std::endl;
    }
    stream_ << "};" << std::endl << std::endl;

    // edges
    for(unsigned char order = 0; order < 3; ++order) {
        const unsigned int orderPrint = order;
        for(Label label = 1; label <= cwcomplex_.numberOfCells(order); ++label) {
            for(size_t k = 0; k < cwcomplex_.sizeAbove(order, label); ++k) {
                stream_ << "\\path [edge] (o" << orderPrint << "c" << label
                  << ") -- (o" << orderPrint + 1 << "c" << cwcomplex_.above(order, label, k)
                  << ");" << std::endl;
            }
        }
        if(cwcomplex_.numberOfCells(order) != 0) {
            stream_ << std::endl;
        }
    }

    stream_ << "\\end{tikzpicture}" << std::endl;
}

template<class T>
inline void
CWComplexLatex<T>::randomizeColors()
{
    for(Order order = 0; order < 4; ++order) {
        colors_[order].resize(cwcomplex_.numberOfCells(order) + 1);
        for(size_t label=1; label <= cwcomplex_.numberOfCells(order); ++label) {
            for(size_t k=0; k<3; ++k) {
                colors_[order][label][k] = static_cast<float>(std::rand()) / RAND_MAX;
            }
        }
    }
}

template<class T>
inline void
CWComplexLatex<T>::standardText()
{
    text_[0] = "0-cells";
    text_[1] = "1-cells";
    text_[2] = "2-cells";
    text_[3] = "3-cells";
}

} // namespace cwx
} // namespace andres

#endif // ANDRES_CWX_LATEX_HXX
