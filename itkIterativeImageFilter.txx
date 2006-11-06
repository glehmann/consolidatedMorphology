#ifndef __itkIterativeImageFilter_txx
#define __itkIterativeImageFilter_txx

#include "itkIterativeImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk {

template <class TInputImage, class TOutputImage>
IterativeImageFilter<TInputImage, TOutputImage>
::IterativeImageFilter()
{
  m_Cast = CastType::New();
  m_Cast->SetInPlace( true );
  m_Cast->ReleaseDataFlagOn();
  m_Filter = NULL;
}


template<class TInputImage, class TOutputImage>
void
IterativeImageFilter<TInputImage, TOutputImage>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  typename InputImageType::Pointer  inputPtr =
    const_cast< InputImageType * >( this->GetInput() );

  if ( !inputPtr )
    { return; }

  inputPtr->SetRequestedRegion(inputPtr->GetLargestPossibleRegion());
}


template<class TInputImage, class TOutputImage>
void
IterativeImageFilter<TInputImage, TOutputImage>
::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()->SetRequestedRegion( this->GetOutput()->GetLargestPossibleRegion() );
}


template <class TInputImage, class TOutputImage>
void
IterativeImageFilter<TInputImage, TOutputImage>
::GenerateData()
{
  if( !m_Filter )
    {
    itkExceptionMacro("Filter must be set.");
    }

  this->AllocateOutputs();
  // set up the pipeline
  m_Cast->SetInput( this->GetInput() );

  // Create a process accumulator for tracking the progress of this minipipeline
  ProgressAccumulator::Pointer progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);
  typename OutputImageType::Pointer img = m_Cast->GetOutput();
   for( int i=0; i<m_NumberOfIterations; i++ )
     {
     m_Filter->SetInput( img );
     progress->RegisterInternalFilter(m_Filter, 1.0/m_NumberOfIterations);
     m_Filter->Update();
     img = m_Filter->GetOutput();
     img->DisconnectPipeline();
     }

  this->GraftOutput( img );

}


template <class TInputImage, class TOutputImage>
void
IterativeImageFilter<TInputImage, TOutputImage>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "NumberOfIterations: " << m_NumberOfIterations << std::endl;
}

}


#endif
