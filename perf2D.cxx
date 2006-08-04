#include "itkImageFileReader.h"

#include "itkGrayscaleDilateImageFilter.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkMorphologicalGradientImageFilter.h"
#include "itkGrayscaleMorphologicalOpeningImageFilter.h"
#include "itkGrayscaleMorphologicalClosingImageFilter.h"

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
  
  typedef itk::FlatStructuringElement< dim > SRType;
  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > DilateType;
  typedef itk::GrayscaleErodeImageFilter< IType, IType, SRType > ErodeType;
  typedef itk::MorphologicalGradientImageFilter< IType, IType, SRType > GradientType;
  typedef itk::GrayscaleMorphologicalOpeningImageFilter< IType, IType, SRType > OpenType;
  typedef itk::GrayscaleMorphologicalClosingImageFilter< IType, IType, SRType > CloseType;

  DilateType::Pointer bdilate = DilateType::New();
  bdilate->SetInput( reader->GetOutput() );
  
  DilateType::Pointer hdilate = DilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  
  DilateType::Pointer adilate = DilateType::New();
  adilate->SetInput( reader->GetOutput() );
  
  DilateType::Pointer vdilate = DilateType::New();
  vdilate->SetInput( reader->GetOutput() );
  
  reader->Update();
  
  std::vector< int > radiusList;
  for( int s=1; s<=10; s++)
    { radiusList.push_back( s ); }
  for( int s=15; s<=30; s+=5)
    { radiusList.push_back( s ); }
  radiusList.push_back( 40 );
  radiusList.push_back( 50 );
  radiusList.push_back( 100 );
  
  std::cout << "#radius" << "\t" 
            << "bd" << "\t" 
            << "hd" << "\t" 
            << "ad" << "\t" 
            << "vd" << "\t" 
            << std::endl;

  for( std::vector< int >::iterator it=radiusList.begin(); it !=radiusList.end() ; it++)
    {
    itk::TimeProbe bdtime;
    itk::TimeProbe hdtime;
    itk::TimeProbe adtime;
    itk::TimeProbe vdtime;

  
    SRType::RadiusType rad;
    rad.Fill( *it );
    SRType kernel = SRType::Box( rad );

    bdilate->SetKernel( kernel );
    hdilate->SetKernel( kernel );
    adilate->SetKernel( kernel );
    vdilate->SetKernel( kernel );
    bdilate->SetAlgorithm( DilateType::BASIC );
    hdilate->SetAlgorithm( DilateType::HISTO );
    adilate->SetAlgorithm( DilateType::ANCHOR );
    vdilate->SetAlgorithm( DilateType::VHGW );

    int nbOfRepeats;
    if( *it <= 10 )
      { nbOfRepeats = 20; }
    else if( *it <= 20 )
      { nbOfRepeats = 10; }
    else if( *it <= 50 )
      { nbOfRepeats = 5; }
    else
      { nbOfRepeats = 2; }
    //nbOfRepeats = 1;

    for( int i=0; i<nbOfRepeats; i++ )
      {
      bdtime.Start();
      bdilate->Update();
      bdtime.Stop();
      bdilate->Modified();

      hdtime.Start();
      hdilate->Update();
      hdtime.Stop();
      hdilate->Modified();

      adtime.Start();
      adilate->Update();
      adtime.Stop();
      adilate->Modified();

      vdtime.Start();
      vdilate->Update();
      vdtime.Stop();
      vdilate->Modified();

      }
      
    std::cout << std::setprecision(3) << *it << "\t" 
              << bdtime.GetMeanTime() << "\t" 
              << hdtime.GetMeanTime() << "\t" 
              << adtime.GetMeanTime() << "\t" 
              << vdtime.GetMeanTime() << "\t" 
//<< erode->GetNameOfBackendFilterClass()
              <<std::endl;
    }
  
  
  return 0;
}

