#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGrayscaleMorphologicalOpeningImageFilter.h"
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
//  radius[1]=0;
  SRType kernel = SRType::Box( radius );
  
  typedef itk::GrayscaleMorphologicalOpeningImageFilter< IType, IType, SRType > MorphologicalOpeningType;
  MorphologicalOpeningType::Pointer open = MorphologicalOpeningType::New();
  open->SetInput( reader->GetOutput() );
  open->SetKernel( kernel );
  
  //itk::SimpleFilterWatcher watcher(open, "filter");

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( open->GetOutput() );

  open->SetAlgorithm( MorphologicalOpeningType::BASIC );
  writer->SetFileName( argv[2] );
  writer->Update();

  open->SetAlgorithm( MorphologicalOpeningType::HISTO );
  writer->SetFileName( argv[3] );
  writer->Update();

  open->SetAlgorithm( MorphologicalOpeningType::ANCHOR );
  writer->SetFileName( argv[4] );
  writer->Update();

  open->SetAlgorithm( MorphologicalOpeningType::VHGW );
  writer->SetFileName( argv[5] );
  writer->Update();


/*  IType::Pointer img = kernel.GetImage<IType>();
  writer->SetInput( img );
  writer->SetFileName( "kernel.png" );
  writer->Update();*/
  
  
  return 0;
}

