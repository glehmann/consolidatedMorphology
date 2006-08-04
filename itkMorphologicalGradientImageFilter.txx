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
  m_BasicDilateFilter = BasicDilateFilterType::New();
  m_BasicErodeFilter = BasicErodeFilterType::New();
  m_HistogramFilter = HistogramFilterType::New();
  m_AnchorDilateFilter = AnchorDilateFilterType::New();
  m_AnchorErodeFilter = AnchorErodeFilterType::New();
  m_vHGWDilateFilter = VHGWDilateFilterType::New();
  m_vHGWErodeFilter = VHGWErodeFilterType::New();
  m_Algorithm = HISTO;
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
    m_Algorithm = ANCHOR;
    }
  else if( m_HistogramFilter->GetUseVectorBasedAlgorithm() )
    {
    // histogram based filter is as least as good as the basic one, so always use it
    m_Algorithm = HISTO;
    m_HistogramFilter->SetKernel( kernel );
    }
  else 
    {
    // basic filter can be better than the histogram based one
    // apply a poor heuristic to find the best one. What is very important is to
    // select the histogram for large kernels

    // we need to set the kernel on the histogram filter to compare basic and histogram algorithm
    m_HistogramFilter->SetKernel( kernel );

    if( m_Kernel.Size() < m_HistogramFilter->GetPixelsPerTranslation() * 4.0 )
      {
      m_BasicDilateFilter->SetKernel( kernel );
      m_BasicErodeFilter->SetKernel( kernel );
      m_Algorithm = BASIC;
      }
    else
      {
      m_Algorithm = HISTO;
      }
    }

  m_Kernel = kernel;
}


template< class TInputImage, class TOutputImage, class TKernel>
void
MorphologicalGradientImageFilter< TInputImage, TOutputImage, TKernel>
::SetAlgorithm( int algo )
{
  const FlatKernelType * flatKernel = NULL;
  try
    { flatKernel = dynamic_cast< const FlatKernelType* >( & m_Kernel ); }
  catch( ... ) {}

  if( m_Algorithm != algo )
    {

    if( algo == BASIC )
      {
      m_BasicDilateFilter->SetKernel( m_Kernel );
      m_BasicErodeFilter->SetKernel( m_Kernel );
      }
    else if( algo == HISTO )
      {
      m_HistogramFilter->SetKernel( m_Kernel );
      }
    else if( flatKernel != NULL && flatKernel->GetDecomposable() && algo == ANCHOR )
      {
      m_AnchorDilateFilter->SetKernel( *flatKernel );
      m_AnchorErodeFilter->SetKernel( *flatKernel );
      }
    else if( flatKernel != NULL && flatKernel->GetDecomposable() && algo == VHGW )
      {
      m_vHGWDilateFilter->SetKernel( *flatKernel );
      m_vHGWErodeFilter->SetKernel( *flatKernel );
      }
    else
      { itkExceptionMacro( << "Invalid algorithm" ); }

    m_Algorithm = algo;
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
  if( m_Algorithm == BASIC )
    {
//     std::cout << "BasicDilateImageFilter" << std::endl;
    m_BasicDilateFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_BasicDilateFilter, 0.4f );

    m_BasicErodeFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_BasicErodeFilter, 0.4f );

    typename SubtractFilterType::Pointer sub = SubtractFilterType::New();
    sub->SetInput1( m_BasicDilateFilter->GetOutput() );
    sub->SetInput2( m_BasicErodeFilter->GetOutput() );
    progress->RegisterInternalFilter( sub, 0.1f );
    
    sub->GraftOutput( this->GetOutput() );
    sub->Update();
    this->GraftOutput( sub->GetOutput() );
    }
  else if( m_Algorithm == HISTO )
    {
//     std::cout << "MovingHistogramDilateImageFilter" << std::endl;
    m_HistogramFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_HistogramFilter, 1.0f );
    
    m_HistogramFilter->GraftOutput( this->GetOutput() );
    m_HistogramFilter->Update();
    this->GraftOutput( m_HistogramFilter->GetOutput() );
    }
  else if( m_Algorithm == ANCHOR )
    {
//     std::cout << "AnchorDilateImageFilter" << std::endl;
    m_AnchorDilateFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_AnchorDilateFilter, 0.4f );

    m_AnchorErodeFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_AnchorErodeFilter, 0.4f );

    typename SubtractFilterType::Pointer sub = SubtractFilterType::New();
    sub->SetInput1( m_AnchorDilateFilter->GetOutput() );
    sub->SetInput2( m_AnchorErodeFilter->GetOutput() );
    progress->RegisterInternalFilter( sub, 0.1f );
    
    sub->GraftOutput( this->GetOutput() );
    sub->Update();
    this->GraftOutput( sub->GetOutput() );
    }
  else if( m_Algorithm == VHGW )
    {
//     std::cout << "vHGWDilateImageFilter" << std::endl;
    m_vHGWDilateFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_vHGWDilateFilter, 0.4f );

    m_vHGWErodeFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_vHGWErodeFilter, 0.4f );

    typename SubtractFilterType::Pointer sub = SubtractFilterType::New();
    sub->SetInput1( m_vHGWDilateFilter->GetOutput() );
    sub->SetInput2( m_vHGWErodeFilter->GetOutput() );
    progress->RegisterInternalFilter( sub, 0.1f );
    
    sub->GraftOutput( this->GetOutput() );
    sub->Update();
    this->GraftOutput( sub->GetOutput() );
    }

}

template<class TInputImage, class TOutputImage, class TKernel>
void
MorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::Modified() const
{
  Superclass::Modified();
  m_BasicDilateFilter->Modified();
  m_BasicErodeFilter->Modified();
  m_HistogramFilter->Modified();
  m_AnchorDilateFilter->Modified();
  m_AnchorErodeFilter->Modified();
  m_vHGWDilateFilter->Modified();
  m_vHGWErodeFilter->Modified();
}

template<class TInputImage, class TOutputImage, class TKernel>
void
MorphologicalGradientImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Kernel: " << m_Kernel << std::endl;
  os << indent << "Algorithm: " << m_Algorithm << std::endl;
}

}// end namespace itk
#endif
