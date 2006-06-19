
#ifndef __itkFlatStructuringElement_txx
#define __itkFlatStructuringElement_txx

#include "itkFlatStructuringElement.h"

namespace itk
{
template<unsigned int VDimension>
void FlatStructuringElement< VDimension >
::PrintSelf(std::ostream &os, Indent indent) const
{
  //Superclass::PrintSelf(os, indent);
  os << indent << "SE type = " << m_seType << std::endl;
}

}

#endif
