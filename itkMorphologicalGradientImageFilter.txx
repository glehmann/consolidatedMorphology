/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMorphologicalGradientImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkMorphologicalGradientImageFilter_txx
#define __itkMorphologicalGradientImageFilter_txx

#include "itkMorphologicalGradientImageFilter.h"
#include "itkNumericTraits.h"
#include "itkProgressAccumulator.h"
#include <string>

namespace itk {


template<class TInputImage, class TOutputImage, class TKernel>
MorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::MorphologicalGradientImageFilter()
  : m_Kernel()
{
  m_BasicFilter = BasicFilterType::New();
  m_HistogramFilter = HistogramFilterType::New();
  m_AnchorDilateFilter = AnchorDilateFilterType::New();
  m_AnchorErodeFilter = AnchorErodeFilterType::New();
  m_NameOfBackendFilterClass = m_HistogramFilter->GetNameOfClass();
}

template<class TInputImage, class TOutputImage, class TKernel>
void
MorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  typename Superclass::InputImagePointer  inputPtr = 
    const_cast< TInputImage * >( this->GetInput() );
  
  if ( !inputPtr )
    {
    return;
    }

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion;
  inputRequestedRegion = inputPtr->GetRequestedRegion();

  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius( m_Kernel.GetRadius() );

  // crop the input requested region at the input's largest possible region
  if ( inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()) )
    {
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    return;
    }
  else
    {
    // Couldn't crop the region (requested region is outside the largest
    // possible region).  Throw an exception.

    // store what we tried to request (prior to trying to crop)
    inputPtr->SetRequestedRegion( inputRequestedRegion );
    
    // build an exception
    InvalidRequestedRegionError e(__FILE__, __LINE__);
    OStringStream msg;
    msg << static_cast<const char *>(this->GetNameOfClass())
        << "::GenerateInputRequestedRegion()";
    e.SetLocation(msg.str().c_str());
    e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
    e.SetDataObject(inputPtr);
    throw e;
    }
}

template< class TInputImage, class TOutputImage, class TKernel>
void
MorphologicalGradientImageFilter< TInputImage, TOutputImage, TKernel>
::SetKernel( const KernelType& kernel )
{
  const FlatKernelType * flatKernel = NULL;
  try
    { flatKernel = dynamic_cast< const FlatKernelType* >( & kernel ); }
  catch( ... ) {}


  if( flatKernel != NULL && flatKernel->GetDecomposable() )
    {
    m_AnchorDilateFilter->SetKernel( *flatKernel );
    m_AnchorErodeFilter->SetKernel( *flatKernel );
    m_NameOfBackendFilterClass = m_AnchorDilateFilter->GetNameOfClass();
    }
  else if( m_HistogramFilter->GetUseVectorBasedAlgorithm() )
    {
    // histogram based filter is as least as good as the basic one, so always use it
    m_NameOfBackendFilterClass = m_HistogramFilter->GetNameOfClass();
    m_BasicFilter->SetKernel( kernel );
    }
  else 
    {
    // basic filter can be better than the histogram based one
    // apply a poor heuristic to find the best one. What is very important is to
    // select the histogram for large kernels

    // we need to set the kernel on the histogram filter to compare basic and histogram algorithm
    m_HistogramFilter->SetKernel( kernel );

    if( ( ImageDimension == 2 && m_Kernel.Size() < m_HistogramFilter->GetPixelsPerTranslation() * 5.4 )
        || ( ImageDimension == 3 && m_Kernel.Size() < m_HistogramFilter->GetPixelsPerTranslation() * 4.5 ) )
      {
      m_BasicFilter->SetKernel( kernel );
      m_NameOfBackendFilterClass = m_BasicFilter->GetNameOfClass();
      }
    else
      {
      m_NameOfBackendFilterClass = m_HistogramFilter->GetNameOfClass();
      }
    }

  m_Kernel = kernel;
}

template< class TInputImage, class TOutputImage, class TKernel>
void
MorphologicalGradientImageFilter< TInputImage, TOutputImage, TKernel>
::SetNameOfBackendFilterClass( const char * name )
{
  if( name == NULL )
    { itkExceptionMacro( << "Invalid name of class." ); }

  const FlatKernelType * flatKernel = NULL;
  try
    { flatKernel = dynamic_cast< const FlatKernelType* >( & m_Kernel ); }
  catch( ... ) {}

  std::cout << "flatKernel: " << flatKernel << std::endl;


  if( strcmp( m_NameOfBackendFilterClass, name ) )
    {
    if( !strcmp( name, m_BasicFilter->GetNameOfClass() ) )
      {
      m_BasicFilter->SetKernel( m_Kernel );
      m_NameOfBackendFilterClass = m_BasicFilter->GetNameOfClass();
      }
    else if( !strcmp( name, m_HistogramFilter->GetNameOfClass() ) )
      {
      m_HistogramFilter->SetKernel( m_Kernel );
      m_NameOfBackendFilterClass = m_HistogramFilter->GetNameOfClass();
      }
    else if( flatKernel != NULL && !strcmp( name, m_AnchorDilateFilter->GetNameOfClass() ) )
      {
      m_AnchorDilateFilter->SetKernel( *flatKernel );
      m_AnchorErodeFilter->SetKernel( *flatKernel );
      m_NameOfBackendFilterClass = m_AnchorDilateFilter->GetNameOfClass();
      }
    else
      { itkExceptionMacro( << "Invalid name of class." ); }
    this->Modified();
    }
}

template<class TInputImage, class TOutputImage, class TKernel>
void
MorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::GenerateData() 
{
  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // Allocate the output
  this->AllocateOutputs();

  // Delegate to a dilate filter.
  if( !strcmp( m_NameOfBackendFilterClass, m_BasicFilter->GetNameOfClass() ) )
    {
//     std::cout << "BasicDilateImageFilter" << std::endl;
    m_BasicFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_BasicFilter, 1.0f );
    
    m_BasicFilter->GraftOutput( this->GetOutput() );
    m_BasicFilter->Update();
    this->GraftOutput( m_BasicFilter->GetOutput() );
    }
  else if( !strcmp( m_NameOfBackendFilterClass, m_HistogramFilter->GetNameOfClass() ) )
    {
//     std::cout << "MovingHistogramDilateImageFilter" << std::endl;
    m_HistogramFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_HistogramFilter, 1.0f );
    
    m_HistogramFilter->GraftOutput( this->GetOutput() );
    m_HistogramFilter->Update();
    this->GraftOutput( m_HistogramFilter->GetOutput() );
    }
  else if( !strcmp( m_NameOfBackendFilterClass, m_AnchorDilateFilter->GetNameOfClass() ) )
    {
//     std::cout << "AnchorDilateImageFilter" << std::endl;
    m_AnchorDilateFilter->SetInput( this->GetInput() );
    m_AnchorErodeFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_AnchorDilateFilter, 0.45f );
    progress->RegisterInternalFilter( m_AnchorErodeFilter, 0.45f );

    typename SubtractFilterType::Pointer subtract = SubtractFilterType::New();
    subtract->SetInput1( m_AnchorDilateFilter->GetOutput() );
    subtract->SetInput2( m_AnchorErodeFilter->GetOutput() );
    progress->RegisterInternalFilter( subtract, 0.1f );
    
    subtract->GraftOutput( this->GetOutput() );
    subtract->Update();
    this->GraftOutput( subtract->GetOutput() );
    }

}

template<class TInputImage, class TOutputImage, class TKernel>
void
MorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::Modified() const
{
  Superclass::Modified();
  m_BasicFilter->Modified();
  m_HistogramFilter->Modified();
  m_AnchorDilateFilter->Modified();
  m_AnchorErodeFilter->Modified();
}

template<class TInputImage, class TOutputImage, class TKernel>
void
MorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Kernel: " << m_Kernel << std::endl;
  os << indent << "NameOfBackendFilterClass: " << m_NameOfBackendFilterClass << std::endl;
}

}// end namespace itk
#endif
