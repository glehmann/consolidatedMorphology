#ifndef __itkvHGWDilateImageFilter_h
#define __itkvHGWDilateImageFilter_h

#include "itkvHGWErodeDilateImageFilter.h"

namespace itk {
template <class pixtype>
class MaxFunctor
{
public:
  MaxFunctor(){}
  ~MaxFunctor(){}
  inline pixtype operator()(const pixtype &A, const pixtype &B)
  {
    return std::max(A, B);
  }
};


template<class TImage, class TKernel>
class  ITK_EXPORT vHGWDilateImageFilter :
    public vHGWErodeDilateImageFilter<TImage, TKernel, MaxFunctor<typename TImage::PixelType> >

{
public:
  typedef vHGWDilateImageFilter Self;
  typedef vHGWErodeDilateImageFilter<TImage, TKernel, MaxFunctor<typename TImage::PixelType> > Superclass;

  /** Runtime information support. */
  itkTypeMacro(vHGWDilateImageFilter, 
               vHGWErodeDilateImageFilter);

  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  virtual ~vHGWDilateImageFilter() {}
protected:
  vHGWDilateImageFilter()
  {
    this->m_Boundary = itk::NumericTraits< typename TImage::PixelType >::NonpositiveMin();
  }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    os << indent << "vHGW erosion: " << std::endl;
  }

private:
  
  vHGWDilateImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};


} // namespace itk

#endif
