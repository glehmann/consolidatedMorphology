#ifndef __itkAnchorDilateImageFilter_h
#define __itkAnchorDilateImageFilter_h

#include "itkAnchorErodeDilateImageFilter.h"

namespace itk {

template<class TImage, class TKernel>
class  ITK_EXPORT AnchorDilateImageFilter :
    public AnchorErodeDilateImageFilter<TImage, TKernel, std::greater<typename TImage::PixelType>, std::greater_equal<typename TImage::PixelType> >

{
public:
  typedef AnchorDilateImageFilter Self;
  typedef AnchorErodeDilateImageFilter<TImage, TKernel, std::less<typename TImage::PixelType>, std::less_equal<typename TImage::PixelType> > Superclass;

  /** Runtime information support. */
  itkTypeMacro(AnchorDilateImageFilter, 
               AnchorErodeDilateImageFilter);

  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  virtual ~AnchorDilateImageFilter() {}
protected:
  AnchorDilateImageFilter(){}
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    os << indent << "Anchor dilation: " << std::endl;
  }

private:
  
  AnchorDilateImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

};


} // namespace itk

#endif
