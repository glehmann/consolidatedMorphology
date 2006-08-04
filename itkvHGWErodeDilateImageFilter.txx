
#ifndef __itkvHGWErodeDilateImageFilter_txx
#define __itkvHGWErodeDilateImageFilter_txx

#include "itkvHGWErodeDilateImageFilter.h"

//#include "itkNeighborhoodAlgorithm.h"

#include "itkvHGWUtilities.h"

namespace itk {

template <class TImage, class TKernel, class TFunction1>
vHGWErodeDilateImageFilter<TImage, TKernel, TFunction1>
::vHGWErodeDilateImageFilter()
{
  m_KernelSet = false;
}

template <class TImage, class TKernel, class TFunction1>
void
vHGWErodeDilateImageFilter<TImage, TKernel, TFunction1>
::SetBoundary(const InputImagePixelType value)
{
  m_Boundary = value;
}

template <class TImage, class TKernel, class TFunction1>
void
vHGWErodeDilateImageFilter<TImage, TKernel, TFunction1>
::GenerateData()
{

  // check that we are using a decomposable kernel
  if (!m_Kernel.GetDecomposable())
    {
    itkWarningMacro("vHGW morphology only works with decomposable structuring elements");
    return;
    }
  if (!m_KernelSet)
    {
    itkWarningMacro("No kernel set - quitting");
    return;
    }
  // TFunction1 will be < for erosions

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
  unsigned int bufflength = 0;
  for (unsigned i = 0; i<TImage::ImageDimension; i++)
    {
    bufflength += OReg.GetSize()[i];
    }
  // compat
  bufflength += 2;

  InputImagePixelType * buffer = new InputImagePixelType[bufflength];
  InputImagePixelType * forward = new InputImagePixelType[bufflength];
  InputImagePixelType * reverse = new InputImagePixelType[bufflength];
  // iterate over all the structuring elements
  typename KernelType::DecompType decomposition = m_Kernel.GetLines();
  BresType BresLine;
  ProgressReporter progress(this, 0, decomposition.size());

//   std::cout << decomposition.size() << " lines will be used" << std::endl;

  for (unsigned i = 0; i < decomposition.size(); i++)
    {
    typename KernelType::LType ThisLine = decomposition[i];
    typename BresType::OffsetArray TheseOffsets = BresLine.buildLine(ThisLine, bufflength);
    unsigned int SELength = getLinePixels<typename KernelType::LType>(ThisLine);
    // want lines to be odd
    if (!(SELength%2))
      ++SELength;

//     std::cout << "line: "<< ThisLine << SELength << std::endl;

    InputImageRegionType BigFace = mkEnlargedFace<InputImageType, typename KernelType::LType>(input, OReg, ThisLine);
    doFace<TImage, BresType, TFunction1, 
      typename KernelType::LType>(input, output, m_Boundary, ThisLine,  
				  TheseOffsets, SELength,
				  buffer, forward, 
				  reverse, OReg, BigFace);
    
    // after the first pass the input will be taken from the output
    input = this->GetOutput();
    progress.CompletedPixel();
    }


  delete [] buffer;
  delete [] forward;
  delete [] reverse;
}


template<class TImage, class TKernel, class TFunction1>
void
vHGWErodeDilateImageFilter<TImage, TKernel, TFunction1>
::PrintSelf(std::ostream &os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}


} // end namespace itk

#endif
