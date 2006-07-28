#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGrayscaleMorphologicalOpeningImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkCommand.h"
#include "itkSimpleFilterWatcher.h"


int main(int, char * argv[])
{
  const int dim = 2;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::BinaryBallStructuringElement<bool, dim> SRType;
  SRType kernel;
  SRType::RadiusType radius;
  radius.Fill( 4 );
  kernel.SetRadius( radius );
  kernel.CreateStructuringElement();
  
  typedef itk::GrayscaleMorphologicalOpeningImageFilter< IType, IType, SRType > MorphologicalOpeningType;
  MorphologicalOpeningType::Pointer open = MorphologicalOpeningType::New();
  open->SetInput( reader->GetOutput() );
  open->SetKernel( kernel );
  
  itk::SimpleFilterWatcher watcher(open, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( open->GetOutput() );

  open->SetAlgorithm( MorphologicalOpeningType::BASIC );
  writer->SetFileName( argv[2] );
  writer->Update();

  open->SetAlgorithm( MorphologicalOpeningType::HISTO );
  writer->SetFileName( argv[3] );
  writer->Update();

  try
    {
    open->SetAlgorithm( MorphologicalOpeningType::ANCHOR );
    return EXIT_FAILURE;
    }
  catch( ... )
    { std::cout << "exception succesfully catched" << std::endl; }

/*  IType::Pointer img = kernel.GetImage<IType>();
  writer->SetInput( img );
  writer->SetFileName( "kernel.png" );
  writer->Update();*/
  
  
  return 0;
}

