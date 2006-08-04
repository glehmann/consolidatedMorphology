#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkCropImageFilter.h"
#include "itkFlatStructuringElement.h"
#include "itkTimeProbe.h"
#include <vector>
#include "itkMultiThreader.h"
#include <iomanip>

int main(int, char * argv[])
{
  itk::MultiThreader::SetGlobalMaximumNumberOfThreads(1);

  const int dim = 2;
  typedef unsigned char PType;
  typedef itk::Image< PType, dim >    IType;
  
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::CropImageFilter< IType, IType > CropType;
  CropType::Pointer crop = CropType::New();
  crop->SetInput( reader->GetOutput() );
  
  typedef itk::FlatStructuringElement<dim> SRType;
  SRType::RadiusType rad;
  rad.Fill( 3 );
  SRType kernel = SRType::Box( rad );

  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer bdilate = DilateType::New();
  bdilate->SetInput( crop->GetOutput() );
  bdilate->SetKernel( kernel );
  bdilate->SetAlgorithm( DilateType::BASIC );
  
  DilateType::Pointer hdilate = DilateType::New();
  hdilate->SetInput( crop->GetOutput() );
  hdilate->SetKernel( kernel );
  hdilate->SetAlgorithm( DilateType::HISTO );
  
  DilateType::Pointer adilate = DilateType::New();
  adilate->SetInput( crop->GetOutput() );
  adilate->SetKernel( kernel );
  adilate->SetAlgorithm( DilateType::ANCHOR );
  
  DilateType::Pointer vdilate = DilateType::New();
  vdilate->SetInput( crop->GetOutput() );
  vdilate->SetKernel( kernel );
  vdilate->SetAlgorithm( DilateType::VHGW );
  
  reader->Update();
  
  std::cout << "#size" << "\t" 
            << "bd" << "\t" 
            << "hd" << "\t" 
            << "ad" << "\t" 
            << "vd" << "\t" 
            << "bd/p" << "\t" 
            << "hd/p" << "\t" 
            << "ad/p" << "\t" 
            << "vd/p" << "\t" 
            << std::endl;

  IType::SizeType imSize = reader->GetOutput()->GetLargestPossibleRegion().GetSize();
  IType::SizeType size = imSize;

  for( int i=0; i<=25; i++)
    {
    itk::TimeProbe bdtime;
    itk::TimeProbe hdtime;
    itk::TimeProbe adtime;
    itk::TimeProbe vdtime;

    IType::SizeType cropSize = imSize - size;
    for(int j=0; j<dim; j++)
      { cropSize[j] = cropSize[j] / 2; }
    crop->SetUpperBoundaryCropSize( cropSize );
    crop->SetLowerBoundaryCropSize( cropSize );

    int nbOfRepeats = 5;

    for( int i=0; i<nbOfRepeats; i++ )
      {
      bdtime.Start();
      bdilate->UpdateLargestPossibleRegion();
      bdtime.Stop();
      bdilate->Modified();

      hdtime.Start();
      hdilate->UpdateLargestPossibleRegion();
      hdtime.Stop();
      hdilate->Modified();

      adtime.Start();
      adilate->UpdateLargestPossibleRegion();
      adtime.Stop();
      adilate->Modified();

      vdtime.Start();
      vdilate->UpdateLargestPossibleRegion();
      vdtime.Stop();
      vdilate->Modified();

      }
      
    unsigned long imageSize = 1;
    for( int j=0; j<dim; j++)
      { imageSize *= crop->GetOutput()->GetLargestPossibleRegion().GetSize()[j]; }

    std::cout << std::setprecision(3)
              << imageSize << "\t" 
              << bdtime.GetMeanTime() << "\t" 
              << hdtime.GetMeanTime() << "\t" 
              << adtime.GetMeanTime() << "\t" 
              << vdtime.GetMeanTime() << "\t" 
              << bdtime.GetMeanTime() / (double) imageSize * 1000000000 << "\t" 
              << hdtime.GetMeanTime() / (double) imageSize * 1000000000 << "\t" 
              << adtime.GetMeanTime() / (double) imageSize * 1000000000 << "\t" 
              << vdtime.GetMeanTime() / (double) imageSize * 1000000000 << "\t" 
              << std::endl;

    for(int j=0; j<dim; j++)
      { size[j] = static_cast<unsigned long>(size[j] * 0.85f); }
    }
  
    
  
  return 0;
}

