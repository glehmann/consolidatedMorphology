#ifndef __itkvHGWErodeImageFilter_h
#define __itkvHGWErodeImageFilter_h

#include "itkvHGWErodeDilateImageFilter.h"

namespace itk {
template <class pixtype>
class MinFunctor
{
public:
  MinFunctor(){}
  ~MinFunctor(){}
  inline pixtype operator()(const pixtype &A, const pixtype &B)
  {
    return std::min(A, B);
  }
};


template<class TImage, class TKernel>
class  ITK_EXPORT vHGWErodeImageFilter :
    public vHGWErodeDilateImageFilter<TImage, TKernel, MinFunctor<typename TImage::PixelType> >

{
public:
  typedef vHGWErodeImageFilter Self;
  typedef vHGWErodeDilateImageFilter<TImage, TKernel, MinFunctor<typename TImage::PixelType> > Superclass;

  /** Runtime information support. */
  itkTypeMacro(vHGWErodeImageFilter, 
               vHGWErodeDilateImageFilter);

  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  virtual ~vHGWErodeImageFilter() {}
protected:
  vHGWErodeImageFilter()
  {
    this->m_Boundary = itk::NumericTraits< typename TImage::PixelType >::max();
  }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    os << indent << "vHGW erosion: " << std::endl;
  }

private:
  
  vHGWErodeImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};


} // namespace itk

#endif
