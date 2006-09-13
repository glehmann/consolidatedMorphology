#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkvHGWDilateImageFilter.h"
#include "itkAnchorDilateImageFilter.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkFlatStructuringElement.h"
#include <iomanip>

int main(int, char * argv[])
{
  const int dim = 2;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::FlatStructuringElement< dim > SRType;
  SRType::RadiusType rad;
  rad.Fill( 4 );
  SRType kernel = SRType::Box( rad );
  
  typedef itk::MovingHistogramDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer hdilate = HDilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  hdilate->SetKernel( kernel );
  
  typedef itk::BasicDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );
  dilate->SetKernel( kernel );
  
  typedef itk::vHGWDilateImageFilter< IType, SRType > VHDilateType;
  VHDilateType::Pointer vhdilate = VHDilateType::New();
  vhdilate->SetInput( reader->GetOutput() );
  vhdilate->SetKernel( kernel );
  
  typedef itk::AnchorDilateImageFilter< IType, SRType > ADilateType;
  ADilateType::Pointer adilate = ADilateType::New();
  adilate->SetInput( reader->GetOutput() );
  adilate->SetKernel( kernel );
  
/*  // write 
  typedef itk::ImageFileWriter< IType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( dilate->GetOutput() );
  writer->SetFileName( argv[2] );*/
  
  reader->Update();
  
  std::cout << "#nb" << "\t" 
            << "d" << "\t" 
            << "hd" << "\t"
            << "ad" << "\t"
            << "vhd" << std::endl;

  for( int t=1; t<=10; t++ )
    {
    itk::TimeProbe time;
    itk::TimeProbe htime;
    itk::TimeProbe vhtime;
    itk::TimeProbe atime;
  
    dilate->SetNumberOfThreads( t );
    hdilate->SetNumberOfThreads( t );
    vhdilate->SetNumberOfThreads( t );
    adilate->SetNumberOfThreads( t );
    
    for( int i=0; i<50; i++ )
      {
      time.Start();
      dilate->Update();
      time.Stop();
      
      htime.Start();
      hdilate->Update();
      htime.Stop();
      
      vhtime.Start();
      vhdilate->Update();
      vhtime.Stop();
      
      atime.Start();
      adilate->Update();
      atime.Stop();
      
      dilate->Modified();
      hdilate->Modified();
      vhdilate->Modified();
      adilate->Modified();
      }
      
    std::cout << std::setprecision(3) << t << "\t" 
              << time.GetMeanTime() << "\t"
              << htime.GetMeanTime() << "\t"
              << atime.GetMeanTime() << "\t"
              << vhtime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}

