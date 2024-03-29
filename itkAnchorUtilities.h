#ifndef __itkAnchorUtilities_h
#define __itkAnchorUtilities_h

#include <list>

#include "itkSharedMorphUtilities.h"

namespace itk {

/**
 * \class AnchorUtilities 
 * \brief functionality in common for anchor openings/closings and
 * erosions/dilation
 *
**/

#if 0
// can be moved to SharedMorphUtilities if user control of border is permitted
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
#endif

template <class TImage, class TBres, class TLine>
int computeStartEnd(const typename TImage::IndexType StartIndex,
		    const TLine line,
		    const float tol,
		    const typename TBres::OffsetArray LineOffsets,
		    const typename TImage::RegionType AllImage, 
		    unsigned &start,
		    unsigned &end);
template <class TImage, class TBres, class TAnchor, class TLine>
void doFace(typename TImage::ConstPointer input,
	    typename TImage::Pointer output,
	    typename TImage::PixelType border,
	    TLine line,
	    TAnchor &AnchorLine,
	    const typename TBres::OffsetArray LineOffsets,
	    typename TImage::PixelType * inbuffer,
	    typename TImage::PixelType * outbuffer,	      
	    const typename TImage::RegionType AllImage, 
	    const typename TImage::RegionType face);

// This creates a list of non overlapping faces that need to be
// processed for this particular line orientation. We are doing this
// instead of using the Face Calculator to avoid repeated operations
// starting from corners.

//template <class TRegion, class TLine>
//std::list<TRegion> mkFaceList(const TRegion AllImage,
//			      const TLine line);


} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkAnchorUtilities.txx"
#endif

#endif
