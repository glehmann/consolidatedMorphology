#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkFlatStructuringElement.h"
#include "itkSimpleFilterWatcher.h"
#include "itkMultiThreader.h"

// Dilation by octagons - results should be identical for different algorithms

int main(int, char * argv[])
{
  const int dim = 2;
  typedef unsigned char PType;
  //typedef float PType;
  typedef itk::Image< PType, dim >    IType;
  itk::MultiThreader::SetGlobalDefaultNumberOfThreads(2);
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::FlatStructuringElement<dim> SRType;
  SRType::RadiusType radius;
  radius.Fill( 7 );
  SRType kernel = SRType::Poly( radius, 4);
  
  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );
  dilate->SetKernel( kernel );
  
  itk::SimpleFilterWatcher watcher(dilate, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( dilate->GetOutput() );

  dilate->SetAlgorithm( DilateType::BASIC );
  writer->SetFileName( argv[2] );
  writer->Update();

  dilate->SetAlgorithm( DilateType::HISTO );
  writer->SetFileName( argv[3] );
  writer->Update();

  dilate->SetAlgorithm( DilateType::ANCHOR );
  writer->SetFileName( argv[4] );
  writer->Update();
  dilate->SetAlgorithm( DilateType::VHGW );
  writer->SetFileName( argv[5] );
  writer->Update();

  IType::Pointer img = kernel.GetImage<IType>();
  writer->SetInput( img );
  writer->SetFileName( "kernel.png" );
  writer->Update();
  
  
  return 0;
}

