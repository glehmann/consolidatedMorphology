#ifndef __itk_SharedMorphUtilities_h
#define __itk_SharedMorphUtilities_h

#include <list>


namespace itk {

template <class TRegion, class TLine>
bool needToDoFace(const TRegion AllImage,
		  const TRegion face,
		  const TLine line);

template <class TImage, class TBres, class TLine>
int computeStartEnd(const typename TImage::IndexType StartIndex,
		    const TLine line,
		    const float tol,
		    const typename TBres::OffsetArray LineOffsets,
		    const typename TImage::RegionType AllImage, 
		    unsigned &start,
		    unsigned &end);

template <class TImage, class TBres, class TLine>
int fillLineBuffer(typename TImage::ConstPointer input,
		   const typename TImage::IndexType StartIndex,
		   const TLine line,
		   const float tol,
		   const typename TBres::OffsetArray LineOffsets,
		   const typename TImage::RegionType AllImage, 
		   typename TImage::PixelType * inbuffer,
		   unsigned &start,
		   unsigned &end);

template <class TImage, class TBres>
void copyLineToImage(const typename TImage::Pointer output,
		     const typename TImage::IndexType StartIndex,
		     const typename TBres::OffsetArray LineOffsets,
		     const typename TImage::PixelType * outbuffer,
		     const unsigned start,
		     const unsigned end);

// This returns a face with a normal between +/- 45 degrees of the
// line. The face is enlarged so that AllImage is entirely filled by
// lines starting from every pixel in the face. This means that some
// of the region will not touch the image. This approach is necessary
// because we want to be able to sweep the lines in a fashion that
// does not have overlap between them.
template <class TInputImage, class TLine>
typename TInputImage::RegionType
mkEnlargedFace(const typename TInputImage::ConstPointer input,
	       const typename TInputImage::RegionType AllImage,
	       const TLine line);

// figure out the correction factor for length->pixel count based on
// line angle
template <class TLine>
unsigned int getLinePixels(const TLine line);

} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkSharedMorphUtilities.txx"
#endif

#endif
