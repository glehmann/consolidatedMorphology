#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMorphologicalGradientImageFilter.h"
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
  
  typedef itk::MorphologicalGradientImageFilter< IType, IType, SRType > MorphologicalGradientType;
  MorphologicalGradientType::Pointer gradient = MorphologicalGradientType::New();
  gradient->SetInput( reader->GetOutput() );
  gradient->SetKernel( kernel );
  
  itk::SimpleFilterWatcher watcher(gradient, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( gradient->GetOutput() );

  gradient->SetAlgorithm( MorphologicalGradientType::BASIC );
  writer->SetFileName( argv[2] );
  writer->Update();

  gradient->SetAlgorithm( MorphologicalGradientType::HISTO );
  writer->SetFileName( argv[3] );
  writer->Update();

  gradient->SetAlgorithm( MorphologicalGradientType::ANCHOR );
  writer->SetFileName( argv[4] );
  writer->Update();

  gradient->SetAlgorithm( MorphologicalGradientType::ANCHOR );
  writer->SetFileName( argv[5] );
  writer->Update();


/*  IType::Pointer img = kernel.GetImage<IType>();
  writer->SetInput( img );
  writer->SetFileName( "kernel.png" );
  writer->Update();*/
  
  
  return 0;
}

