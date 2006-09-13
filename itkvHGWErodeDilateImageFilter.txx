
#ifndef __itkvHGWErodeDilateImageFilter_txx
#define __itkvHGWErodeDilateImageFilter_txx

#include "itkvHGWErodeDilateImageFilter.h"
#include "itkImageRegionIterator.h"
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
::ThreadedGenerateData (const InputImageRegionType& outputRegionForThread,
			int threadId)
//::GenerateData()
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
  //this->AllocateOutputs();
  //InputImagePointer output = this->GetOutput();
  InputImageConstPointer input = this->GetInput();

  InputImageRegionType IReg = outputRegionForThread;
  IReg.PadByRadius( m_Kernel.GetRadius() );
  IReg.Crop( this->GetInput()->GetRequestedRegion() );

  // experimental part
  // allocate an internal buffer
  typename InputImageType::Pointer internalbuffer = InputImageType::New();
  internalbuffer->SetRegions(IReg);
  internalbuffer->Allocate();
  InputImagePointer output = internalbuffer;

  // get the region size
  //InputImageRegionType OReg = output->GetRequestedRegion();
  InputImageRegionType OReg = outputRegionForThread;
//   std::cout << "vhgw oreg" << OReg << std::endl;
//   std::cout << "vhgw ireg" << IReg << std::endl;
//   std::cout << "vhgw internal" << internalbuffer->GetLargestPossibleRegion() << std::endl;
  // maximum buffer length is sum of dimensions
  unsigned int bufflength = 0;
  for (unsigned i = 0; i<TImage::ImageDimension; i++)
    {
    bufflength += IReg.GetSize()[i];
    }
  // compat
  bufflength += 2;

  InputImagePixelType * buffer = new InputImagePixelType[bufflength];
  InputImagePixelType * forward = new InputImagePixelType[bufflength];
  InputImagePixelType * reverse = new InputImagePixelType[bufflength];
  // iterate over all the structuring elements
  typename KernelType::DecompType decomposition = m_Kernel.GetLines();
  BresType BresLine;
  //ProgressReporter progress(this, 0, decomposition.size());
  ProgressReporter progress(this, threadId, decomposition.size());

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

    InputImageRegionType BigFace = mkEnlargedFace<InputImageType, typename KernelType::LType>(input, IReg, ThisLine);
    doFace<TImage, BresType, TFunction1, 
      typename KernelType::LType>(input, output, m_Boundary, ThisLine,  
				  TheseOffsets, SELength,
				  buffer, forward, 
				  reverse, IReg, BigFace);
    
    // after the first pass the input will be taken from the output
      // experimental part
    //input = this->GetOutput();
    input = internalbuffer;
    progress.CompletedPixel();
    }

  // experimental
  // copy internal buffer to output - is there any point using a crop filter??
  typedef typename itk::ImageRegionIterator<InputImageType> IterType;
  IterType oit(this->GetOutput(), OReg);
  IterType iit(internalbuffer, OReg);
  for (oit.GoToBegin(), iit.GoToBegin(); !oit.IsAtEnd(); ++oit, ++iit)
    {
    oit.Set(iit.Get());
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


template<class TImage, class TKernel, class TFunction1>
void
vHGWErodeDilateImageFilter<TImage, TKernel, TFunction1>
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
