#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"
#include "itkFlatStructuringElement.h"

int main(int argn, char * argv[])
{

  if( argn < 4 )
    {
    std::cerr << "usage: kernelShape fileName radius type [lines|img]" << std::endl;
    std::cerr << "  type: 0 -> Box" << std::endl;
    std::cerr << "        1 -> Ball" << std::endl;
    std::cerr << "        2 -> Poly" << std::endl;
    std::cerr << "        3 -> FromImage" << std::endl;
    return 0;
    }

  const int dim = 2;
  
  typedef unsigned char PType;
  typedef itk::Image< PType, dim > IType;

  typedef itk::FlatStructuringElement<dim> SEType;

  SEType::RadiusType Rad;
  Rad.Fill(atoi(argv[2]));
  int type = atoi(argv[3]);
  SEType K;
  if( type == 0 )
    K = SEType::Box(Rad);
  else if( type == 1 )
    K = SEType::Ball(Rad);
  else if( type == 2 )
    K = SEType::Poly(Rad, atoi(argv[4]));
  else if( type == 3 )
    {
    typedef itk::ImageFileReader< IType > ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( argv[4] );
    K = SEType::FromImage<IType>( reader->GetOutput() );
    }
  else if( type == 4 )
    K = SEType::Annulus(Rad, atoi(argv[4]), atoi(argv[5]));
  else if( type == 5 )
    K = SEType::Cross(Rad);
  else
    return 1;

  IType::Pointer kernelImage = K.GetImage<IType>();

  //kernelImage->Print( std::cout );

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( kernelImage );
  writer->SetFileName( argv[1] );
  writer->Update();

  return 0;
}

