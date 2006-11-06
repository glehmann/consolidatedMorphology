#ifndef __itkIterativeImageFilter_h
#define __itkIterativeImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkCastImageFilter.h"


namespace itk {

/**
 * \class IterativeImageFilter
 * \brief A iterative filter to apply N times a transform to an image
 *
 * \author Gaetan Lehmann
 */

template<class TInputImage, class TOutputImage>
class ITK_EXPORT IterativeImageFilter : 
public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef IterativeImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(IterativeImageFilter,
               ImageToImageFilter);
 
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  
  typedef TOutputImage OutputImageType;
  typedef typename TOutputImage::PixelType OutputPixelType ;

  typedef ImageToImageFilter<TOutputImage, TOutputImage> FilterType ;
  typedef CastImageFilter< InputImageType, OutputImageType > CastType ;
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  /** n-dimensional Kernel radius. */
  typedef typename TInputImage::SizeType RadiusType ;

  itkSetMacro(NumberOfIterations, unsigned int);
  itkGetMacro(NumberOfIterations, unsigned int);

  itkSetObjectMacro(Filter, FilterType);
  itkGetObjectMacro(Filter, FilterType);

  void GenerateInputRequestedRegion() ;
  void EnlargeOutputRequestedRegion(DataObject *itkNotUsed(output));

protected:
  IterativeImageFilter();
  ~IterativeImageFilter() {};

  void GenerateData();

  void PrintSelf(std::ostream& os, Indent indent) const;

  typename FilterType::Pointer m_Filter;
  
  typename CastType::Pointer m_Cast;

  unsigned int m_NumberOfIterations;

private:
  IterativeImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
};

}

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkIterativeImageFilter.txx"
#endif

#endif
