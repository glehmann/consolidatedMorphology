#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGrayscaleMorphologicalClosingImageFilter.h"
#include "itkFlatStructuringElement.h"
#include "itkCropImageFilter.h"
#include "itkConstantPadImageFilter.h"
#include "itkvHGWErodeImageFilter.h"
#include "itkvHGWDilateImageFilter.h"
#include "itkNumericTraits.h"

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
  radius.Fill( 15 );
  SRType kernel = SRType::Box( radius );
  
  typedef itk::GrayscaleMorphologicalClosingImageFilter< IType, IType, SRType > MorphologicalClosingType;
  MorphologicalClosingType::Pointer close = MorphologicalClosingType::New();
  close->SetInput( reader->GetOutput() );
  close->SetKernel( kernel );
  

  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( close->GetOutput() );

  close->SetAlgorithm( MorphologicalClosingType::BASIC );
  writer->SetFileName( argv[2] );
  writer->Update();

  std::cout << "Done basic, starting vHGW" << std::endl;
  // now for an explicit pipeline
  typedef itk::ConstantPadImageFilter<IType, IType> PadType;
  typedef itk::CropImageFilter<IType, IType> CropType;
  typedef itk::vHGWErodeImageFilter< IType, SRType > vHGWErodeFilterType;
  typedef itk::vHGWDilateImageFilter< IType, SRType > vHGWDilateFilterType;

  PadType::Pointer pad = PadType::New();
  pad->SetInput(reader->GetOutput());
  pad->SetPadLowerBound( kernel.GetRadius().m_Size );
  pad->SetPadUpperBound( kernel.GetRadius().m_Size );
  pad->SetConstant( itk::NumericTraits< IType::PixelType>::NonpositiveMin() );
  pad->Update();
  vHGWDilateFilterType::Pointer dilate = vHGWDilateFilterType::New();
  vHGWErodeFilterType::Pointer erode = vHGWErodeFilterType::New();

  dilate->SetInput(pad->GetOutput());
  dilate->SetKernel(kernel);
  dilate->Update();
  erode->SetInput(dilate->GetOutput());
  erode->SetKernel(kernel);
  erode->Update();

  CropType::Pointer crop = CropType::New();
  crop->SetInput( erode->GetOutput() );
  crop->SetUpperBoundaryCropSize( kernel.GetRadius() );
  crop->SetLowerBoundaryCropSize( kernel.GetRadius() );
  
  writer->SetInput(crop->GetOutput());
  writer->SetFileName( argv[3] );
  writer->Update();
  std::cout << "done " << std::endl;

/*  IType::Pointer img = kernel.GetImage<IType>();
  writer->SetInput( img );
  writer->SetFileName( "kernel.png" );
  writer->Update();*/
  
  
  return EXIT_SUCCESS;
}

