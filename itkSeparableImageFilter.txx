#ifndef __itkSeparableImageFilter_txx
#define __itkSeparableImageFilter_txx

#include "itkSeparableImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk {

template <class TInputImage, class TOutputImage, class TFilter>
SeparableImageFilter<TInputImage, TOutputImage, TFilter>
::SeparableImageFilter()
{
  // create the pipeline
  for( unsigned i = 0; i < ImageDimension; i++ )
    {
    m_Filters[i] = FilterType::New();
    m_Filters[i]->ReleaseDataFlagOn();
    if( i > 0 ) 
      {
      m_Filters[i]->SetInput( m_Filters[i-1]->GetOutput() );
      }
    }
  
  m_Cast = CastType::New();
  m_Cast->SetInput( m_Filters[ImageDimension-1]->GetOutput() );
  m_Cast->SetInPlace( true );
}


template<class TInputImage, class TOutputImage, class TFilter>
void
SeparableImageFilter<TInputImage, TOutputImage, TFilter>
::Modified() const
{
  Superclass::Modified();
  for (unsigned i = 0; i < ImageDimension; i++)
    {
    m_Filters[i]->Modified();
    }
  m_Cast->Modified();
}


template<class TInputImage, class TOutputImage, class TFilter>
void
SeparableImageFilter<TInputImage, TOutputImage, TFilter>
::SetNumberOfThreads( int nb )
{
  Superclass::SetNumberOfThreads( nb );
  for (unsigned i = 0; i < ImageDimension; i++)
    {
    m_Filters[i]->SetNumberOfThreads( nb );
    }
  m_Cast->SetNumberOfThreads( nb );
}


template <class TInputImage, class TOutputImage, class TFilter>
void
SeparableImageFilter<TInputImage, TOutputImage, TFilter>
::SetRadius( const RadiusType & radius )
{
  Superclass::SetRadius( radius );
  
  // set up the kernels
  for (unsigned i = 0; i< ImageDimension; i++)
    {
    RadiusType rad;
    rad.Fill(0);
    rad[i] = radius[i];
    m_Filters[i]->SetRadius( rad );
    }
}


template <class TInputImage, class TOutputImage, class TFilter>
void
SeparableImageFilter<TInputImage, TOutputImage, TFilter>
::GenerateData()
{
  this->AllocateOutputs();
  
  // set up the pipeline
  m_Filters[0]->SetInput( this->GetInput() );

  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  for( unsigned i = 0; i< ImageDimension; i++ )
    {
    progress->RegisterInternalFilter( m_Filters[i], 1.0/ImageDimension );
    }

  m_Cast->GraftOutput( this->GetOutput() ); 
  m_Cast->Update();
  this->GraftOutput( m_Cast->GetOutput() );

}

}


#endif
