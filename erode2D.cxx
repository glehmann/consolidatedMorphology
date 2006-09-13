#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkFlatStructuringElement.h"
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
  
  typedef itk::FlatStructuringElement<dim> SRType;
  SRType::RadiusType radius;
  radius.Fill( 4 );
  SRType kernel = SRType::Box( radius );
  
  typedef itk::GrayscaleErodeImageFilter< IType, IType, SRType > ErodeType;
  ErodeType::Pointer erode = ErodeType::New();
  erode->SetInput( reader->GetOutput() );
  erode->SetKernel( kernel );
  
  itk::SimpleFilterWatcher watcher(erode, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( erode->GetOutput() );

  erode->SetAlgorithm( ErodeType::BASIC );
  writer->SetFileName( argv[2] );
  writer->Update();

  erode->SetAlgorithm( ErodeType::HISTO );
  writer->SetFileName( argv[3] );
  writer->Update();

  erode->SetAlgorithm( ErodeType::ANCHOR );
  writer->SetFileName( argv[4] );
  writer->Update();

  erode->SetAlgorithm( ErodeType::VHGW );
  writer->SetFileName( argv[5] );
  writer->Update();


/*  IType::Pointer img = kernel.GetImage<IType>();
  writer->SetInput( img );
  writer->SetFileName( "kernel.png" );
  writer->Update();*/
  
  
  return 0;
}

