#ifndef __itkFlatStructuringElement_h
#define __itkFlatStructuringElement_h

#include "itkNeighborhood.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkSize.h"
#include "itkOffset.h"
#include "itkOffsetLexicographicCompare.h"
#include <set>
namespace itk {

/** \class FlatStructuringElement
* \brief A class to support a variety of flat structuring elements, 
* including versions created by decomposition of lines.
**/

template<unsigned int VDimension>
class ITK_EXPORT FlatStructuringElement
{
public:
  /** Standard class typedefs. */
  typedef FlatStructuringElement Self;
  //typedef Neighborhood<bool, VDimension> Superclass;

  /** External support for dimensionality. */
  itkStaticConstMacro(NeighborhoodDimension, unsigned int, VDimension);

  /** Radius typedef support. */
  typedef Size<VDimension> RadiusType;

  /** Default destructor. */
  virtual ~FlatStructuringElement() {}

  /** Default consructor. */
  FlatStructuringElement() {}

  /** Various constructors */

  static FlatStructuringElement Box(RadiusType radius)
  {
    FlatStructuringElement res = FlatStructuringElement();
   
    res.m_seType = seBox;
    for (unsigned i = 0; i < VDimension; ++i)
      {
      res.m_Box[i] = 2*radius[i] + 1;
      }
    return (res);
  }
  
  static FlatStructuringElement Ball(RadiusType radius)
  {
    FlatStructuringElement res = FlatStructuringElement();
    
    res.m_seType = seBall;
    res.m_Ball.SetRadius(radius);
    res.m_Ball.CreateStructuringElement();
    return res;
  }

  void PrintSelf(std::ostream &os, Indent indent) const;

private:
  typedef enum {seBall, seBox, seAny, sePoly} seType;
  // flag to define the structuring element type
  seType m_seType; 

  // structures to represent the different types of SE
  typedef BinaryBallStructuringElement<bool, VDimension> BallSEType;
  
  // A box structuring element - will get decomposed into VDimension
  // orthogonal lines
  typedef Size<VDimension> BoxSEType;
  
  // polyhedra support - radial decomposition of circles/spheres 
  // into lines.
  // Use offsets to represent lines at the moment - basically consider
  // it as a vector describing line orientation
  typedef Offset<VDimension> LineType;
  typedef std::set<LineType, Functor::OffsetLexicographicCompare<VDimension> > PolySEType;

  // instances of the possible SEs
  BallSEType m_Ball;
  BoxSEType m_Box;
  PolySEType m_Poly;
};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkFlatStructuringElement.txx"
#endif



#endif
