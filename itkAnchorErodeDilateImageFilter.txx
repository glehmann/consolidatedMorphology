
#ifndef __itkAnchorErodeDilateImageFilter_txx
#define __itkAnchorErodeDilateImageFilter_txx

#include "itkAnchorErodeDilateImageFilter.h"

//#include "itkNeighborhoodAlgorithm.h"

#include "itkAnchorUtilities.h"

namespace itk {

template <class TImage, class TKernel, class TFunction1, class TFunction2>
AnchorErodeDilateImageFilter<TImage, TKernel, TFunction1, TFunction2>
::AnchorErodeDilateImageFilter()
{
  m_KernelSet = false;
}

template <class TImage, class TKernel, class TFunction1, class TFunction2>
void
AnchorErodeDilateImageFilter<TImage, TKernel, TFunction1, TFunction2>
::SetBoundary(const InputImagePixelType value)
{
  m_Boundary = value;
}

template <class TImage, class TKernel, class TFunction1, class TFunction2>
void
AnchorErodeDilateImageFilter<TImage, TKernel, TFunction1, TFunction2>
::GenerateData()
{

  // check that we are using a decomposable kernel
  if (!m_Kernel.GetDecomposable())
    {
    itkWarningMacro("Anchor morphology only works with decomposable structuring elements");
    return;
    }
  if (!m_KernelSet)
    {
    itkWarningMacro("No kernel set - quitting");
    return;
    }
  // TFunction1 will be < for erosions
  // TFunction2 will be <=


  // the initial version will adopt the methodology of loading a line
  // at a time into a buffer vector, carrying out the opening or
  // closing, and then copy the result to the output. Hopefully this
  // will improve cache performance when working along non raster
  // directions.

  // Allocate the output
  this->AllocateOutputs();
  InputImagePointer output = this->GetOutput();
  InputImageConstPointer input = this->GetInput();
  
  // get the region size
  InputImageRegionType OReg = output->GetRequestedRegion();
  // maximum buffer length is sum of dimensions
  std::cout << OReg << std::endl;
  unsigned int bufflength = 0;
  for (unsigned i = 0; i<TImage::ImageDimension; i++)
    {
    bufflength += OReg.GetSize()[i];
    }
  // compat
  bufflength += 2;

  InputImagePixelType * buffer = new InputImagePixelType[bufflength];
  InputImagePixelType * inbuffer = new InputImagePixelType[bufflength];
  // iterate over all the structuring elements
  typename KernelType::DecompType decomposition = m_Kernel.GetLines();
  BresType BresLine;
  ProgressReporter progress(this, 0, decomposition.size());

  for (unsigned i = 0; i < decomposition.size(); i++)
    {
    typename KernelType::LType ThisLine = decomposition[i];
    typename BresType::OffsetArray TheseOffsets = BresLine.buildLine(ThisLine, bufflength);
    unsigned int SELength = getLinePixels<typename KernelType::LType>(ThisLine);
    // want lines to be odd
    if (!(SELength%2))
      ++SELength;

    InputImageRegionType BigFace = mkEnlargedFace<InputImageType, typename KernelType::LType>(input, OReg, ThisLine);

    AnchorLine.SetSize(SELength);
    doFace<TImage, BresType, AnchorLineType, typename KernelType::LType>(input, output, m_Boundary, ThisLine, AnchorLine, 
									   TheseOffsets, inbuffer, buffer, OReg, BigFace);
    // after the first pass the input will be taken from the output
    input = this->GetOutput();
    progress.CompletedPixel();
    }


  delete [] buffer;
  delete [] inbuffer;
}


template<class TImage, class TKernel, class TFunction1, class TFunction2>
void
AnchorErodeDilateImageFilter<TImage, TKernel, TFunction1, TFunction2>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


template <class TImage, class TKernel, class TFunction1, class TFunction2>
void
AnchorErodeDilateImageFilter<TImage, TKernel, TFunction1, TFunction2>
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
