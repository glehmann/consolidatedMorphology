
#include "itkIndent.h"
#include "itkImage.h"
#include "itkFlatStructuringElement.h"

int main(int, char * argv[])
{
  const int dim = 2;

  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::FlatStructuringElement<dim> SEType;
  SEType::RadiusType radius;
  radius.Fill(10);
  SEType SE1 = SEType::Box(radius);
  SE1.PrintSelf(std::cout, itk::Indent(0));

  SEType SE2 = SEType::Ball(radius);
  SE2.PrintSelf(std::cout, itk::Indent(0));
  return 0;
}
