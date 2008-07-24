/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGrayscaleErodeImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.15 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGrayscaleErodeImageFilter_h
#define __itkGrayscaleErodeImageFilter_h

#include "itkKernelImageFilter.h"
#include "itkMovingHistogramErodeImageFilter.h"
#include "itkBasicErodeImageFilter.h"
#include "itkAnchorErodeImageFilter.h"
#include "itkvHGWErodeImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkConstantBoundaryCondition.h"
#include "itkFlatStructuringElement.h"
#include "itkNeighborhood.h"

namespace itk {

/**
 * \class GrayscaleErodeImageFilter
 * \brief gray scale dilation of an image
 *
 * Erode an image using grayscale morphology. Dilation takes the
 * maximum of all the pixels identified by the structuring element.
 *
 * The structuring element is assumed to be composed of binary
 * values (zero or one). Only elements of the structuring element
 * having values > 0 are candidates for affecting the center pixel.
 * 
 * \sa MorphologyImageFilter, GrayscaleFunctionErodeImageFilter, BinaryErodeImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */

template<class TInputImage, class TOutputImage, class TKernel>
class ITK_EXPORT GrayscaleErodeImageFilter : 
    public KernelImageFilter<TInputImage, TOutputImage, TKernel>
{
public:
  /** Standard class typedefs. */
  typedef GrayscaleErodeImageFilter Self;
  typedef KernelImageFilter<TInputImage,TOutputImage, TKernel>  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(GrayscaleErodeImageFilter, 
               KernelImageFilter);
  
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

  typedef MovingHistogramErodeImageFilter< TInputImage, TOutputImage, TKernel > HistogramFilterType;
  typedef BasicErodeImageFilter< TInputImage, TOutputImage, TKernel > BasicFilterType;
  typedef FlatStructuringElement< ImageDimension > FlatKernelType;
  typedef AnchorErodeImageFilter< TInputImage, FlatKernelType > AnchorFilterType;
  typedef vHGWErodeImageFilter< TInputImage, FlatKernelType > VHGWFilterType;
  typedef CastImageFilter< TInputImage, TOutputImage > CastFilterType;
  
  /** Typedef for boundary conditions. */
  typedef ImageBoundaryCondition<InputImageType> *ImageBoundaryConditionPointerType;
  typedef ImageBoundaryCondition<InputImageType> const *ImageBoundaryConditionConstPointerType;
  typedef ConstantBoundaryCondition<InputImageType> DefaultBoundaryConditionType;


  /** Kernel typedef. */
  typedef TKernel KernelType;
//   typedef typename KernelType::Superclass KernelSuperClass;
//   typedef Neighborhood< typename KernelType::PixelType, ImageDimension > KernelSuperClass;
  
  /** Set kernel (structuring element). */
  void SetKernel( const KernelType& kernel );

  /** Set/Get the boundary value. */
  void SetBoundary( const PixelType value );
  itkGetMacro(Boundary, PixelType);
  
  /** Set/Get the backend filter class. */
  void SetAlgorithm(int algo );
  itkGetMacro(Algorithm, int);
  
  /** GrayscaleErodeImageFilter need to set its internal filters as modified */
  virtual void Modified() const;

  /** \deprecated
  * Allows a user to override the internal boundary condition. Care should be
  * be taken to ensure that the overriding boundary condition is a persistent
  * object during the time it is referenced. The overriding condition
  * can be of a different type than the default type as long as it is
  * a subclass of ImageBoundaryCondition. */
  void OverrideBoundaryCondition(const DefaultBoundaryConditionType* i)
    {
    itkLegacyBodyMacro(itk::GrayscaleErodeImageFilter::OverrideBoundaryCondition, 2.8);
    SetBoundary( i->GetConstant() );
    }

  /** \deprecated
   * Get the current boundary condition.
   */
  const DefaultBoundaryConditionType* GetBoundaryCondition()
    {
    itkLegacyBodyMacro(itk::GrayscaleErodeImageFilter::GetBoundaryCondition, 2.8);
    return &m_BoundaryCondition;
    }

  /** \deprecated
   * Rest the boundary condition to the default
   */
  void ResetBoundaryCondition()
    {
    itkLegacyBodyMacro(itk::GrayscaleErodeImageFilter::ResetBoundaryCondition, 2.8);
    SetBoundary( itk::NumericTraits< PixelType >::NonpositiveMin() );
    }

  /** define values used to determine which algorithm to use */
  static const int BASIC = 0;
  static const int HISTO = 1;
  static const int ANCHOR = 2;
  static const int VHGW = 3;

  void SetNumberOfThreads( int nb );


protected:
  GrayscaleErodeImageFilter();
  ~GrayscaleErodeImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  void GenerateData();

private:
  GrayscaleErodeImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  PixelType m_Boundary;

  // the filters used internally
  typename HistogramFilterType::Pointer m_HistogramFilter;
  typename BasicFilterType::Pointer m_BasicFilter;
  typename AnchorFilterType::Pointer m_AnchorFilter;
  typename VHGWFilterType::Pointer m_VHGWFilter;

  // and the name of the filter
  int m_Algorithm;

  // the boundary condition need to be stored here
  DefaultBoundaryConditionType m_BoundaryCondition;
  
} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkGrayscaleErodeImageFilter.txx"
#endif

#endif


