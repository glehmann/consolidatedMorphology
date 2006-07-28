#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkWhiteTopHatImageFilter.h"
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
  
  typedef itk::WhiteTopHatImageFilter< IType, IType, SRType > TopHatType;
  TopHatType::Pointer topHat = TopHatType::New();
  topHat->SetInput( reader->GetOutput() );
  topHat->SetKernel( kernel );
  
  itk::SimpleFilterWatcher watcher(topHat, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( topHat->GetOutput() );

  topHat->SetAlgorithm( TopHatType::BASIC );
  writer->SetFileName( argv[2] );
  writer->Update();

  topHat->SetAlgorithm( TopHatType::HISTO );
  writer->SetFileName( argv[3] );
  writer->Update();

  try
    { topHat->SetAlgorithm( TopHatType::ANCHOR ); }
  catch( ... )
    { std::cout << "exception succesfully catched" << std::endl; }

/*  IType::Pointer img = kernel.GetImage<IType>();
  writer->SetInput( img );
  writer->SetFileName( "kernel.png" );
  writer->Update();*/
  
  
  return 0;
}

