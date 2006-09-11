#ifndef __itkAnchorOpenCloseImageFilter_txx
#define __itkAnchorOpenCloseImageFilter_txx

#include "itkAnchorOpenCloseImageFilter.h"
#include "itkNeighborhoodAlgorithm.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkAnchorUtilities.h"
#include <itkImageRegionIterator.h>
namespace itk {

template <class TImage, class TKernel, class LessThan, class GreaterThan, class LessEqual, class GreaterEqual>
AnchorOpenCloseImageFilter<TImage, TKernel, LessThan, GreaterThan, LessEqual, GreaterEqual>
::AnchorOpenCloseImageFilter()
{
  m_KernelSet = false;
}

template <class TImage, class TKernel, class LessThan, class GreaterThan, class LessEqual, class GreaterEqual>
void
AnchorOpenCloseImageFilter<TImage, TKernel, LessThan, GreaterThan, LessEqual, GreaterEqual>
::GenerateData()
{

  // check that we are using a decomposable kernel
  if (!m_Kernel.GetDecomposable())
    {
    itkExceptionMacro("Anchor morphology only works with decomposable structuring elements");
    }
  if (!m_KernelSet)
    {
    itkExceptionMacro("No kernel set");
    }

  
  // Allocate the output
  this->AllocateOutputs();
  InputImagePointer output = this->GetOutput();
  InputImageConstPointer input = this->GetInput();

  
  // get the region size
  InputImageRegionType OReg = output->GetRequestedRegion();
#if 0
  // for testing purposes - set output to 0
  ImageRegionIterator<InputImageType> it(output, OReg);
  for(it.GoToBegin(); !it.IsAtEnd(); ++it)
    {
    it.Set(0);
    }
#endif

  // maximum buffer length is sum of dimensions
  unsigned int bufflength = 0;
  for (unsigned i = 0; i<TImage::ImageDimension; i++)
    {
    bufflength += OReg.GetSize()[i];
    }
  // compat
  bufflength += 2;
  //unsigned linecount = OReg.GetNumberOfPixels()/bufflength;

  InputImagePixelType * inbuffer = new InputImagePixelType[bufflength];
  InputImagePixelType * outbuffer = new InputImagePixelType[bufflength];

  // iterate over all the structuring elements
  typename KernelType::DecompType decomposition = m_Kernel.GetLines();
  BresType BresLine;
  ProgressReporter progress(this, 0, decomposition.size()*2);

  // first stage -- all of the erosions if we are doing an opening
  for (unsigned i = 0; i < decomposition.size() - 1; i++)
    {
    typename KernelType::LType ThisLine = decomposition[i];
    typename BresType::OffsetArray TheseOffsets = BresLine.buildLine(ThisLine, bufflength);
    unsigned int SELength = getLinePixels<typename KernelType::LType>(ThisLine);
    // want lines to be odd
    if (!(SELength%2))
      ++SELength;
    AnchorLineErode.SetSize(SELength);

    InputImageRegionType BigFace = mkEnlargedFace<InputImageType, typename KernelType::LType>(input, OReg, ThisLine);
    doFace<TImage, BresType, 
      AnchorLineErodeType, 
      typename KernelType::LType>(input, output, m_Boundary1, ThisLine, AnchorLineErode, 
				  TheseOffsets, inbuffer, outbuffer, OReg, BigFace);
    

    // after the first pass the input will be taken from the output
    input = this->GetOutput();
    progress.CompletedPixel();
    }
  // now do the opening in the middle of the chain
  {
  unsigned i = decomposition.size() - 1;
  typename KernelType::LType ThisLine = decomposition[i];
  typename BresType::OffsetArray TheseOffsets = BresLine.buildLine(ThisLine, bufflength);
  unsigned int SELength = getLinePixels<typename KernelType::LType>(ThisLine);
  // want lines to be odd
  if (!(SELength%2))
    ++SELength;

  AnchorLineOpen.SetSize(SELength);
  InputImageRegionType BigFace = mkEnlargedFace<InputImageType, typename KernelType::LType>(input, OReg, ThisLine);

  // Now figure out which faces of the image we should be starting
  // from with this line
  doFaceOpen(input, output, m_Boundary1, ThisLine,
	     TheseOffsets, outbuffer, 
	     OReg, BigFace);
  // equivalent to two passes
  progress.CompletedPixel();
  progress.CompletedPixel();  
  }

  // Now for the rest of the dilations -- note that i needs to be signed
  for (int i = decomposition.size() - 2; i >= 0; --i)
    {
    typename KernelType::LType ThisLine = decomposition[i];
    typename BresType::OffsetArray TheseOffsets = BresLine.buildLine(ThisLine, bufflength);
    unsigned int SELength = getLinePixels<typename KernelType::LType>(ThisLine);
    // want lines to be odd
    if (!(SELength%2))
      ++SELength;
  
    AnchorLineDilate.SetSize(SELength);

    InputImageRegionType BigFace = mkEnlargedFace<InputImageType, typename KernelType::LType>(input, OReg, ThisLine);
    doFace<TImage, BresType, 
      AnchorLineDilateType, 
      typename KernelType::LType>(input, output, m_Boundary2, ThisLine, AnchorLineDilate, 
				  TheseOffsets, inbuffer, outbuffer, OReg, BigFace);

    
    progress.CompletedPixel();
    }

  delete [] inbuffer;
  delete [] outbuffer;
}

template<class TImage, class TKernel, class LessThan, class GreaterThan, class LessEqual, class GreaterEqual>
void
AnchorOpenCloseImageFilter<TImage, TKernel, LessThan, GreaterThan, LessEqual, GreaterEqual>
::doFaceOpen(InputImageConstPointer input,
	     InputImagePointer output,
	     typename TImage::PixelType border,
	     typename KernelType::LType line,
	     typename BresType::OffsetArray LineOffsets,
	     InputImagePixelType * outbuffer,	      
	     const InputImageRegionType AllImage, 
	     const InputImageRegionType face)
{
  // iterate over the face
  typedef ImageRegionConstIteratorWithIndex<InputImageType> ItType;
  ItType it(input, face);
  it.GoToBegin();
  typename KernelType::LType NormLine = line;
  NormLine.Normalize();
  // set a generous tolerance
  float tol = 1.0/LineOffsets.size();
  while (!it.IsAtEnd()) 
    {
    typename TImage::IndexType Ind = it.GetIndex();
    unsigned start, end, len;
    if (fillLineBuffer<TImage, BresType, typename KernelType::LType>(input, Ind, NormLine, 
								     tol, LineOffsets, 
								     AllImage, outbuffer, 
								     start, end))
      {
      
      len = end - start + 1;
      // compat
      outbuffer[0]=border;
      outbuffer[len+1]=border;
      AnchorLineOpen.doLine(outbuffer,len+2);  // compat
      copyLineToImage<TImage, BresType>(output, Ind, LineOffsets, outbuffer, start, end);
      }
    ++it;
    }
}

template<class TImage, class TKernel, class LessThan, class GreaterThan, class LessEqual, class GreaterEqual>
void
AnchorOpenCloseImageFilter<TImage, TKernel, LessThan, GreaterThan, LessEqual, GreaterEqual>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


template<class TImage, class TKernel, class LessThan, class GreaterThan, class LessEqual, class GreaterEqual>
void
AnchorOpenCloseImageFilter<TImage, TKernel, LessThan, GreaterThan, LessEqual, GreaterEqual>
::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();
  
  // get pointers to the input and output
  typename Superclass::InputImagePointer  inputPtr = 
    const_cast< TImage * >( this->GetInput() );
  
  if ( !inputPtr )
    {
    return;
    }

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TImage::RegionType inputRequestedRegion;
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

} // end namespace itk

#endif
