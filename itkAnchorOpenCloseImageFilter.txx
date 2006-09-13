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
::ThreadedGenerateData (const InputImageRegionType& outputRegionForThread,
			int threadId)
{

  // check that we are using a decomposable kernel
  if (!m_Kernel.GetDecomposable())
    {
    itkExceptionMacro("Anchor morphology only works with decomposable structuring elements");
    return;
    }
  if (!m_KernelSet)
    {
    itkExceptionMacro("No kernel set - quitting");
    return;
    }
  // TFunction1 will be < for erosions
  // TFunction2 will be <=


  // the initial version will adopt the methodology of loading a line
  // at a time into a buffer vector, carrying out the opening or
  // closing, and then copy the result to the output. Hopefully this
  // will improve cache performance when working along non raster
  // directions.

  ProgressReporter progress(this, threadId, m_Kernel.GetLines().size()*2 + 1);

  InputImageConstPointer input = this->GetInput();

  InputImageRegionType IReg = outputRegionForThread;
  IReg.PadByRadius( m_Kernel.GetRadius() );
  IReg.Crop( this->GetInput()->GetRequestedRegion() );

   // allocate an internal buffer
  typename InputImageType::Pointer internalbuffer = InputImageType::New();
  internalbuffer->SetRegions(IReg);
  internalbuffer->Allocate();
  InputImagePointer output = internalbuffer;

  // get the region size
  InputImageRegionType OReg = outputRegionForThread;
  // maximum buffer length is sum of dimensions
  unsigned int bufflength = 0;
  for (unsigned i = 0; i<TImage::ImageDimension; i++)
    {
    bufflength += IReg.GetSize()[i];
    }

  // compat
  bufflength += 2;

  InputImagePixelType * buffer = new InputImagePixelType[bufflength];
  InputImagePixelType * inbuffer = new InputImagePixelType[bufflength];
  // iterate over all the structuring elements
  typename KernelType::DecompType decomposition = m_Kernel.GetLines();
  BresType BresLine;

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
				  TheseOffsets, inbuffer, buffer, OReg, BigFace);
    

    // after the first pass the input will be taken from the output
    input = internalbuffer;
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
	     TheseOffsets, buffer, 
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
				  TheseOffsets, inbuffer, buffer, OReg, BigFace);

    
    progress.CompletedPixel();
    }

  // copy internal buffer to output
  typedef typename itk::ImageRegionIterator<InputImageType> IterType;
  IterType oit(this->GetOutput(), OReg);
  IterType iit(internalbuffer, OReg);
  for (oit.GoToBegin(), iit.GoToBegin(); !oit.IsAtEnd(); ++oit, ++iit)
    {
    oit.Set(iit.Get());
    }
  progress.CompletedPixel();

  delete [] buffer;
  delete [] inbuffer;
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
