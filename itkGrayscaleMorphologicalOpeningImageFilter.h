/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGrayscaleMorphologicalOpeningImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGrayscaleMorphologicalOpeningImageFilter_h
#define __itkGrayscaleMorphologicalOpeningImageFilter_h

#include "itkImageToImageFilter.h"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkMovingHistogramErodeImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkBasicErodeImageFilter.h"
#include "itkAnchorOpenImageFilter.h"
#include "itkvHGWErodeImageFilter.h"
#include "itkvHGWDilateImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkConstantBoundaryCondition.h"
#include "itkFlatStructuringElement.h"
#include "itkNeighborhood.h"

namespace itk {

/**
 * \class GrayscaleMorphologicalOpeningImageFilter
 * \brief gray scale dilation of an image
 *
 * Dilate an image using grayscale morphology. Dilation takes the
 * maximum of all the pixels identified by the structuring element.
 *
 * The structuring element is assumed to be composed of binary
 * values (zero or one). Only elements of the structuring element
 * having values > 0 are candidates for affecting the center pixel.
 * 
 * \sa MorphologyImageFilter, GrayscaleFunctionDilateImageFilter, BinaryDilateImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */

template<class TInputImage, class TOutputImage, class TKernel>
class ITK_EXPORT GrayscaleMorphologicalOpeningImageFilter : 
    public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef GrayscaleMorphologicalOpeningImageFilter Self;
  typedef ImageToImageFilter<TInputImage,TOutputImage>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(GrayscaleMorphologicalOpeningImageFilter, 
               ImageToImageFilter);
  
  /** Image related typedefs. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      TInputImage::ImageDimension);
                      
  /** Image related typedefs. */
  typedef TInputImage InputImageType;
  typedef TOutputImage OutputImageType;
  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;
  typedef typename TInputImage::OffsetType OffsetType ;
  typedef typename Superclass::OutputImageRegionType OutputImageRegionType;

  typedef FlatStructuringElement< ImageDimension > FlatKernelType;
  typedef MovingHistogramDilateImageFilter< TInputImage, TOutputImage, TKernel > HistogramDilateFilterType;
  typedef MovingHistogramErodeImageFilter< TInputImage, TOutputImage, TKernel > HistogramErodeFilterType;
  typedef BasicErodeImageFilter< TInputImage, TInputImage, TKernel > BasicErodeFilterType;
  typedef BasicDilateImageFilter< TInputImage, TOutputImage, TKernel > BasicDilateFilterType;
  typedef AnchorOpenImageFilter< TInputImage, FlatKernelType > AnchorFilterType;
  typedef vHGWErodeImageFilter< TInputImage, FlatKernelType > vHGWErodeFilterType;
  typedef vHGWDilateImageFilter< TInputImage, FlatKernelType > vHGWDilateFilterType;
  typedef CastImageFilter< TInputImage, TOutputImage > SubtractFilterType;
  
  /** Kernel typedef. */
  typedef TKernel KernelType;
//   typedef typename KernelType::Superclass KernelSuperClass;
//   typedef Neighborhood< typename KernelType::PixelType, ImageDimension > KernelSuperClass;
  
  /** Set kernel (structuring element). */
  void SetKernel( const KernelType& kernel );

  /** Get the kernel (structuring element). */
  itkGetConstReferenceMacro(Kernel, KernelType);
  
  /** Set/Get the backend filter class. */
  void SetAlgorithm(int algo );
  itkGetMacro(Algorithm, int);
  
  /** GrayscaleMorphologicalOpeningImageFilter need to make sure they request enough of an
   * input image to account for the structuring element size.  The input
   * requested region is expanded by the radius of the structuring element.
   * If the request extends past the LargestPossibleRegion for the input,
   * the request is cropped by the LargestPossibleRegion. */
  void GenerateInputRequestedRegion() ;

  /** GrayscaleMorphologicalOpeningImageFilter need to set its internal filters as modified */
  virtual void Modified() const;

  /** define values used to determine which algorithm to use */
  static const int BASIC = 0;
  static const int HISTO = 1;
  static const int ANCHOR = 2;
  static const int VHGW = 3;

  /** A safe border is added to input image to avoid borders effects
   * and remove it once the closing is done */
  itkSetMacro(SafeBorder, bool);
  itkGetConstReferenceMacro(SafeBorder, bool);
  itkBooleanMacro(SafeBorder);

protected:
  GrayscaleMorphologicalOpeningImageFilter();
  ~GrayscaleMorphologicalOpeningImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  void GenerateData();

private:
  GrayscaleMorphologicalOpeningImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** kernel or structuring element to use. */
  KernelType m_Kernel ;

  // the filters used internally
  typename HistogramDilateFilterType::Pointer m_HistogramDilateFilter;
  typename HistogramErodeFilterType::Pointer m_HistogramErodeFilter;
  typename BasicDilateFilterType::Pointer m_BasicDilateFilter;
  typename BasicErodeFilterType::Pointer m_BasicErodeFilter;
  typename vHGWDilateFilterType::Pointer m_vHGWDilateFilter;
  typename vHGWErodeFilterType::Pointer m_vHGWErodeFilter;
  typename AnchorFilterType::Pointer m_AnchorFilter;

  // and the name of the filter
  int m_Algorithm;

  bool m_SafeBorder;

} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGrayscaleMorphologicalOpeningImageFilter.txx"
#endif

#endif


