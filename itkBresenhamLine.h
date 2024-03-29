#ifndef __itkBresenhamLine_h
#define __itkBresenhamLine_h

#include "itkVector.h"
#include "itkOffset.h"
#include "itkIndex.h"
#include <vector>

namespace itk {

/* a simple class that will return an array of indexes that are
* offsets along the line. The line will be described by a vector and a
* length */

template <unsigned int VDimension>
class ITK_EXPORT BresenhamLine
{
public:
  typedef BresenhamLine Self;
  // This defines the line direction
  typedef Vector<float, VDimension> LType;
  typedef Offset<VDimension> OffsetType;
  typedef Index<VDimension> IndexType;
  typedef std::vector<OffsetType> OffsetArray;

  typedef typename IndexType::IndexValueType IndexValueType;

  // constructurs
  BresenhamLine(){}
  ~BresenhamLine(){}

  OffsetArray buildLine(LType Direction, unsigned int length);

};


}


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBresenhamLine.txx"
#endif


#endif
