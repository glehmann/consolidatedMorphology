/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkBasicErodeImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.17 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkBasicErodeImageFilter_h
#define __itkBasicErodeImageFilter_h

#include "itkMorphologyImageFilter.h"

namespace itk {

/** \class BasicErodeImageFilter
 * \brief gray scale erosion of an image
 *
 * Erode an image using grayscale morphology. Erosion takes the
 * minimum of all the pixels identified by the structuring element.
 *
 * The structuring element is assumed to be composed of binary
 * values (zero or one). Only elements of the structuring element
 * having values > 0 are candidates for affecting the center pixel.
 * 
 * For the each input image pixel, 
 *   - NeighborhoodIterator gives neighbors of the pixel. 
 *   - Evaluate() member function returns the minimum value among 
 *     the image neighbors where the kernel has elements > 0
 *   - Replace the original value with the min value
 *
 * \sa MorphologyImageFilter, GrayscaleFunctionErodeImageFilter, BinaryErodeImageFilter
 * \ingroup ImageEnhancement  MathematicalMorphologyImageFilters
 */
template<class TInputImage, class TOutputImage, class TKernel>
class ITK_EXPORT BasicErodeImageFilter : 
    public MorphologyImageFilter<TInputImage, TOutputImage, TKernel>
{
public:
  /** Standard class typedefs. */
  typedef BasicErodeImageFilter Self;
  typedef MorphologyImageFilter<TInputImage, TOutputImage, TKernel>
  Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  
  /** Standard New method. */
  itkNewMacro(Self);  

  /** Runtime information support. */
  itkTypeMacro(BasicErodeImageFilter, 
               MorphologyImageFilter);
  
  /** Declaration of pixel type. */
  typedef typename Superclass::PixelType PixelType;

  /** Kernel (structuring element) iterator. */
  typedef typename Superclass::KernelIteratorType  KernelIteratorType;

  /** Neighborhood iterator type. */
  typedef typename Superclass::NeighborhoodIteratorType NeighborhoodIteratorType ;

  /** Kernel typedef. */
  typedef typename Superclass::KernelType KernelType;

  /** Default boundary condition type */
  typedef typename Superclass::DefaultBoundaryConditionType DefaultBoundaryConditionType;
  
protected:
  BasicErodeImageFilter();
  ~BasicErodeImageFilter() {};

  /** Evaluate image neighborhood with kernel to find the new value 
   * for the center pixel value.
   *
   * It will return the minimum value of the image pixels whose corresponding
   * element in the structuring element is positive. This version of
   * Evaluate is used for non-boundary pixels. */
  virtual PixelType Evaluate(const NeighborhoodIteratorType &nit,
                             const KernelIteratorType kernelBegin,
                             const KernelIteratorType kernelEnd);

private:
  BasicErodeImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  // Default boundary condition for erosion filter, defaults to
  // NumericTraits<PixelType>::max()
  DefaultBoundaryConditionType m_ErodeBoundaryCondition;

} ; // end of class

} // end namespace itk
  
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkBasicErodeImageFilter.txx"
#endif

#endif


