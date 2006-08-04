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
  : m_Kernel()
{
  m_BasicFilter = BasicFilterType::New();
  m_HistogramFilter = HistogramFilterType::New();
  m_AnchorFilter = AnchorFilterType::New();
  m_VHGWFilter = VHGWFilterType::New();
  m_Algorithm = HISTO;

  this->SetBoundary( itk::NumericTraits< PixelType >::max() );
}

template<class TInputImage, class TOutputImage, class TKernel>
void
GrayscaleErodeImageFilter<TInputImage, TOutputImage, TKernel>
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

    if( ( ImageDimension == 2 && m_Kernel.Size() < m_HistogramFilter->GetPixelsPerTranslation() * 5.4 )
        || ( ImageDimension == 3 && m_Kernel.Size() < m_HistogramFilter->GetPixelsPerTranslation() * 4.5 ) )
      {
      m_BasicFilter->SetKernel( kernel );
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
    { flatKernel = dynamic_cast< const FlatKernelType* >( & m_Kernel ); }
  catch( ... ) {}

  if( m_Algorithm != algo )
    {

    if( algo == BASIC )
      {
      m_BasicFilter->SetKernel( m_Kernel );
      }
    else if( algo == HISTO )
      {
      m_HistogramFilter->SetKernel( m_Kernel );
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
//     std::cout << "BasicErodeImageFilter" << std::endl;
    m_BasicFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_BasicFilter, 1.0f );
    
    m_BasicFilter->GraftOutput( this->GetOutput() );
    m_BasicFilter->Update();
    this->GraftOutput( m_BasicFilter->GetOutput() );
    }
  else if( m_Algorithm == HISTO )
    {
//     std::cout << "MovingHistogramErodeImageFilter" << std::endl;
    m_HistogramFilter->SetInput( this->GetInput() );
    progress->RegisterInternalFilter( m_HistogramFilter, 1.0f );
    
    m_HistogramFilter->GraftOutput( this->GetOutput() );
    m_HistogramFilter->Update();
    this->GraftOutput( m_HistogramFilter->GetOutput() );
    }
  else if( m_Algorithm == ANCHOR )
    {
//     std::cout << "AnchorErodeImageFilter" << std::endl;
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
//     std::cout << "vHGWErodeImageFilter" << std::endl;
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

  os << indent << "Kernel: " << m_Kernel << std::endl;
  os << indent << "Boundary: " <<  static_cast<typename NumericTraits<PixelType>::PrintType>( m_Boundary ) << std::endl;
  os << indent << "Algorithm: " << m_Algorithm << std::endl;
}

}// end namespace itk
#endif
