/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkGrayscaleErodeImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/04/30 21:02:03 $
  Version:   $Revision: 1.14 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkGrayscaleErodeImageFilter_txx
#define __itkGrayscaleErodeImageFilter_txx

#include "itkGrayscaleErodeImageFilter.h"
#include "itkNumericTraits.h"
#include "itkProgressAccumulator.h"
#include <string>

namespace itk {


template<class TInputImage, class TOutputImage, class TKernel>
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
::GrayscaleErodeImageFilter()
{
  m_BasicFilter = BasicFilterType::New();
  m_HistogramFilter = HistogramFilterType::New();
  m_AnchorFilter = AnchorFilterType::New();
  m_VHGWFilter = VHGWFilterType::New();
  m_Algorithm = HISTO;

  this->SetBoundary( itk::NumericTraits< PixelType >::max() );
}

template< class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter< TInputImage, TOutputImage, TKernel>
::SetNumberOfThreads( int nb )
{
  Superclass::SetNumberOfThreads( nb );
  m_HistogramFilter->SetNumberOfThreads( nb );
  m_AnchorFilter->SetNumberOfThreads( nb );
  m_VHGWFilter->SetNumberOfThreads( nb );
  m_BasicFilter->SetNumberOfThreads( nb );
}


template< class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter< TInputImage, TOutputImage, TKernel>
::SetKernel( const KernelType& kernel )
{
  const FlatKernelType * flatKernel = NULL;
  try
    { flatKernel = dynamic_cast< const FlatKernelType* >( & kernel ); }
  catch( ... ) {}


  if( flatKernel != NULL && flatKernel->GetDecomposable() )
    {
    m_AnchorFilter->SetKernel( *flatKernel );
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

    if( ( ImageDimension == 2 && this->GetKernel().Size() < m_HistogramFilter->GetPixelsPerTranslation() * 5.4 )
        || ( ImageDimension == 3 && this->GetKernel().Size() < m_HistogramFilter->GetPixelsPerTranslation() * 4.5 ) )
      {
      m_BasicFilter->SetKernel( kernel );
      m_Algorithm = BASIC;
      }
    else
      {
      m_Algorithm = HISTO;
      }
    }

  Superclass::SetKernel( kernel );
}

template< class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter< TInputImage, TOutputImage, TKernel>
::SetBoundary( const PixelType value )
{
  m_Boundary = value;
  m_HistogramFilter->SetBoundary( value );
  m_AnchorFilter->SetBoundary(value);
  m_VHGWFilter->SetBoundary(value);
  m_BoundaryCondition.SetConstant( value );
  m_BasicFilter->OverrideBoundaryCondition( &m_BoundaryCondition );
}

template< class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter< TInputImage, TOutputImage, TKernel>
::SetAlgorithm( int algo )
{
  const FlatKernelType * flatKernel = NULL;
  try
    { flatKernel = dynamic_cast< const FlatKernelType* >( & this->GetKernel() ); }
  catch( ... ) {}

  if( m_Algorithm != algo )
    {

    if( algo == BASIC )
      {
      m_BasicFilter->SetKernel( this->GetKernel() );
      }
    else if( algo == HISTO )
      {
      m_HistogramFilter->SetKernel( this->GetKernel() );
      }
    else if( flatKernel != NULL && flatKernel->GetDecomposable() && algo == ANCHOR )
      {
      m_AnchorFilter->SetKernel( *flatKernel );
      }
    else if( flatKernel != NULL && flatKernel->GetDecomposable() && algo == VHGW )
      {
      m_VHGWFilter->SetKernel( *flatKernel );
      }
    else
      { itkExceptionMacro( << "Invalid algorithm" ); }

    m_Algorithm = algo;
    this->Modified();

    }
}

template<class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
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
    itkDebugMacro("Running BasicErodeImageFilter");
    m_BasicFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_BasicFilter, 1.0f );
    
    m_BasicFilter->GraftOutput( this->GetOutput() );
    m_BasicFilter->Update();
    this->GraftOutput( m_BasicFilter->GetOutput() );
    }
  else if( m_Algorithm == HISTO )
    {
    itkDebugMacro("Running MovingHistogramErodeImageFilter");
    m_HistogramFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_HistogramFilter, 1.0f );
    
    m_HistogramFilter->GraftOutput( this->GetOutput() );
    m_HistogramFilter->Update();
    this->GraftOutput( m_HistogramFilter->GetOutput() );
    }
  else if( m_Algorithm == ANCHOR )
    {
    itkDebugMacro("Running AnchorErodeImageFilter");
    m_AnchorFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_AnchorFilter, 0.9f );

    typename CastFilterType::Pointer cast = CastFilterType::New();
    cast->SetInput( m_AnchorFilter->GetOutput() );
    progress->RegisterInternalFilter( cast, 0.1f );
    
    cast->GraftOutput( this->GetOutput() );
    cast->Update();
    this->GraftOutput( cast->GetOutput() );
    }
  else if( m_Algorithm == VHGW )
    {
    itkDebugMacro("Running vHGWErodeImageFilter");
    m_VHGWFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_VHGWFilter, 0.9f );

    typename CastFilterType::Pointer cast = CastFilterType::New();
    cast->SetInput( m_VHGWFilter->GetOutput() );
    progress->RegisterInternalFilter( cast, 0.1f );
    
    cast->GraftOutput( this->GetOutput() );
    cast->Update();
    this->GraftOutput( cast->GetOutput() );
    }

}

template<class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
::Modified() const
{
  Superclass::Modified();
  m_BasicFilter->Modified();
  m_HistogramFilter->Modified();
  m_AnchorFilter->Modified();
  m_VHGWFilter->Modified();
}

template<class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Boundary: " <<  static_cast<typename NumericTraits<PixelType>::PrintType>( m_Boundary ) << std::endl;
  os << indent << "Algorithm: " << m_Algorithm << std::endl;
}

}// end namespace itk
#endif
