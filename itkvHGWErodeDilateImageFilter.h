#ifndef __itkvHGWErodeDilateImageFilter_h
#define __itkvHGWErodeDilateImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkProgressReporter.h"
#include "itkBresenhamLine.h"

namespace itk {

/** 
 * \class vHGWErodeDilateImageFilter
 * \brief class to implement erosions and dilations using anchor
 * methods. This is the base class that must be instantiated with
 * appropriate definitions of greater, less and so on.
 * The SetBoundary facility isn't necessary for operation of the
 * anchor method but is included for compatability with other
 * morphology classes in itk.

**/
template<class TImage, class TKernel, 
	 class TFunction1>
class ITK_EXPORT vHGWErodeDilateImageFilter :
    public ImageToImageFilter<TImage, TImage>
{
public:
  /** Standard class typedefs. */
  typedef vHGWErodeDilateImageFilter Self;
  typedef ImageToImageFilter<TImage, TImage>
  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Some convenient typedefs. */
  /** Kernel typedef. */
  typedef TKernel KernelType;

  typedef TImage InputImageType;
  typedef typename InputImageType::Pointer         InputImagePointer;
  typedef typename InputImageType::ConstPointer    InputImageConstPointer;
  typedef typename InputImageType::RegionType      InputImageRegionType;
  typedef typename InputImageType::PixelType       InputImagePixelType;
  typedef typename TImage::IndexType         IndexType;
  typedef typename TImage::SizeType          SizeType;

  /** ImageDimension constants */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TImage::ImageDimension);

  /** Standard New method. */
  itkNewMacro(Self);

  /** Runtime information support. */
  itkTypeMacro(vHGWErodeDilateImageFilter,
               ImageToImageFilter);

  void SetKernel( const KernelType& kernel )
  {
    m_Kernel=kernel;
    m_KernelSet = true;
  }

  /** Set/Get the boundary value. */
  void SetBoundary( const InputImagePixelType value );
  itkGetMacro(Boundary, InputImagePixelType);


protected:
  vHGWErodeDilateImageFilter();
  ~vHGWErodeDilateImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Multi-thread version GenerateData. */
  void  ThreadedGenerateData (const InputImageRegionType& outputRegionForThread,
                              int threadId) ;

  /** GrayscaleMorphologicalOpeningImageFilter need to make sure they request enough of an
   * input image to account for the structuring element size.  The input
   * requested region is expanded by the radius of the structuring element.
   * If the request extends past the LargestPossibleRegion for the input,
   * the request is cropped by the LargestPossibleRegion. */
  void GenerateInputRequestedRegion() ;

  // should be set by the meta filter
  InputImagePixelType m_Boundary;

private:
  vHGWErodeDilateImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  TKernel m_Kernel;
  bool m_KernelSet;
  typedef BresenhamLine<TImage::ImageDimension> BresType;


} ; // end of class


} // end namespace itk


#ifndef ITK_MANUAL_INSTANTIATION
#include "itkvHGWErodeDilateImageFilter.txx"
#endif

#endif

