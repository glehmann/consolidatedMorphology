#include "itkImageFileReader.h"

#include "itkGrayscaleDilateImageFilter.h"
#include "itkGrayscaleErodeImageFilter.h"
#include "itkMorphologicalGradientImageFilter.h"
#include "itkGrayscaleMorphologicalOpeningImageFilter.h"
#include "itkGrayscaleMorphologicalClosingImageFilter.h"

#include "itkSeparableImageFilter.h"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkMovingHistogramErodeImageFilter.h"

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

  typedef itk::MovingHistogramDilateImageFilter< IType, IType, SRType > MHDilateType;
  typedef itk::SeparableImageFilter< IType, IType, MHDilateType, SRType > SMHDilateType;

  typedef itk::MovingHistogramErodeImageFilter< IType, IType, SRType > MHErodeType;
  typedef itk::SeparableImageFilter< IType, IType, MHErodeType, SRType > SMHErodeType;

  DilateType::Pointer bdilate = DilateType::New();
  bdilate->SetInput( reader->GetOutput() );
  
  DilateType::Pointer hdilate = DilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  
  DilateType::Pointer adilate = DilateType::New();
  adilate->SetInput( reader->GetOutput() );
  
  DilateType::Pointer vdilate = DilateType::New();
  vdilate->SetInput( reader->GetOutput() );
  
  SMHDilateType::Pointer smhdilate = SMHDilateType::New();
  smhdilate->SetInput( reader->GetOutput() );
  

  ErodeType::Pointer berode = ErodeType::New();
  berode->SetInput( reader->GetOutput() );
  
  ErodeType::Pointer herode = ErodeType::New();
  herode->SetInput( reader->GetOutput() );
  
  ErodeType::Pointer aerode = ErodeType::New();
  aerode->SetInput( reader->GetOutput() );
  
  ErodeType::Pointer verode = ErodeType::New();
  verode->SetInput( reader->GetOutput() );
  
  SMHErodeType::Pointer smherode = SMHErodeType::New();
  smherode->SetInput( reader->GetOutput() );
  

  GradientType::Pointer bgradient = GradientType::New();
  bgradient->SetInput( reader->GetOutput() );
  
  GradientType::Pointer hgradient = GradientType::New();
  hgradient->SetInput( reader->GetOutput() );
  
  GradientType::Pointer agradient = GradientType::New();
  agradient->SetInput( reader->GetOutput() );
  
  GradientType::Pointer vgradient = GradientType::New();
  vgradient->SetInput( reader->GetOutput() );
  

  OpenType::Pointer bopen = OpenType::New();
  bopen->SetInput( reader->GetOutput() );
  
  OpenType::Pointer hopen = OpenType::New();
  hopen->SetInput( reader->GetOutput() );
  
  OpenType::Pointer aopen = OpenType::New();
  aopen->SetInput( reader->GetOutput() );
  
  OpenType::Pointer vopen = OpenType::New();
  vopen->SetInput( reader->GetOutput() );
  

  CloseType::Pointer bclose = CloseType::New();
  bclose->SetInput( reader->GetOutput() );
  
  CloseType::Pointer hclose = CloseType::New();
  hclose->SetInput( reader->GetOutput() );
  
  CloseType::Pointer aclose = CloseType::New();
  aclose->SetInput( reader->GetOutput() );
  
  CloseType::Pointer vclose = CloseType::New();
  vclose->SetInput( reader->GetOutput() );

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
            << "smhd" << "\t" 
            << "ad" << "\t" 
            << "vd" << "\t" 
            << "be" << "\t" 
            << "he" << "\t" 
            << "smhe" << "\t" 
            << "ae" << "\t" 
            << "ve" << "\t" 
            << "bg" << "\t" 
            << "hg" << "\t" 
            << "ag" << "\t" 
            << "vg" << "\t" 
            << "bo" << "\t" 
            << "ho" << "\t" 
            << "ao" << "\t" 
            << "vo" << "\t" 
            << "bc" << "\t" 
            << "hc" << "\t" 
            << "ac" << "\t" 
            << "vc" << "\t" 
            << std::endl;

  for( std::vector< int >::iterator it=radiusList.begin(); it !=radiusList.end() ; it++)
    {
    itk::TimeProbe bdtime;
    itk::TimeProbe hdtime;
    itk::TimeProbe adtime;
    itk::TimeProbe vdtime;
    itk::TimeProbe smhdtime;

    itk::TimeProbe betime;
    itk::TimeProbe hetime;
    itk::TimeProbe aetime;
    itk::TimeProbe vetime;
    itk::TimeProbe smhetime;

    itk::TimeProbe bgtime;
    itk::TimeProbe hgtime;
    itk::TimeProbe agtime;
    itk::TimeProbe vgtime;

    itk::TimeProbe botime;
    itk::TimeProbe hotime;
    itk::TimeProbe aotime;
    itk::TimeProbe votime;
  
    itk::TimeProbe bctime;
    itk::TimeProbe hctime;
    itk::TimeProbe actime;
    itk::TimeProbe vctime;
  
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
    smhdilate->SetRadius( rad );

    berode->SetKernel( kernel );
    herode->SetKernel( kernel );
    aerode->SetKernel( kernel );
    verode->SetKernel( kernel );
    berode->SetAlgorithm( ErodeType::BASIC );
    herode->SetAlgorithm( ErodeType::HISTO );
    aerode->SetAlgorithm( ErodeType::ANCHOR );
    verode->SetAlgorithm( ErodeType::VHGW );
    smherode->SetRadius( rad );

    bgradient->SetKernel( kernel );
    hgradient->SetKernel( kernel );
    agradient->SetKernel( kernel );
    vgradient->SetKernel( kernel );
    bgradient->SetAlgorithm( GradientType::BASIC );
    hgradient->SetAlgorithm( GradientType::HISTO );
    agradient->SetAlgorithm( GradientType::ANCHOR );
    vgradient->SetAlgorithm( GradientType::VHGW );

    bopen->SetKernel( kernel );
    hopen->SetKernel( kernel );
    aopen->SetKernel( kernel );
    vopen->SetKernel( kernel );
    bopen->SetAlgorithm( OpenType::BASIC );
    hopen->SetAlgorithm( OpenType::HISTO );
    aopen->SetAlgorithm( OpenType::ANCHOR );
    vopen->SetAlgorithm( OpenType::VHGW );

    bclose->SetKernel( kernel );
    hclose->SetKernel( kernel );
    aclose->SetKernel( kernel );
    vclose->SetKernel( kernel );
    bclose->SetAlgorithm( CloseType::BASIC );
    hclose->SetAlgorithm( CloseType::HISTO );
    aclose->SetAlgorithm( CloseType::ANCHOR );
    vclose->SetAlgorithm( CloseType::VHGW );

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

      smhdtime.Start();
      smhdilate->Update();
      smhdtime.Stop();
      smhdilate->Modified();

      adtime.Start();
      adilate->Update();
      adtime.Stop();
      adilate->Modified();

      vdtime.Start();
      vdilate->Update();
      vdtime.Stop();
      vdilate->Modified();


      betime.Start();
      berode->Update();
      betime.Stop();
      berode->Modified();

      hetime.Start();
      herode->Update();
      hetime.Stop();
      herode->Modified();

      smhetime.Start();
      smherode->Update();
      smhetime.Stop();
      smherode->Modified();

      aetime.Start();
      aerode->Update();
      aetime.Stop();
      aerode->Modified();

      vetime.Start();
      verode->Update();
      vetime.Stop();
      verode->Modified();


      bgtime.Start();
      bgradient->Update();
      bgtime.Stop();
      bgradient->Modified();

      hgtime.Start();
      hgradient->Update();
      hgtime.Stop();
      hgradient->Modified();

      agtime.Start();
      agradient->Update();
      agtime.Stop();
      agradient->Modified();

      vgtime.Start();
      vgradient->Update();
      vgtime.Stop();
      vgradient->Modified();


      botime.Start();
      bopen->Update();
      botime.Stop();
      bopen->Modified();

      hotime.Start();
      hopen->Update();
      hotime.Stop();
      hopen->Modified();

      aotime.Start();
      aopen->Update();
      aotime.Stop();
      aopen->Modified();

      votime.Start();
      vopen->Update();
      votime.Stop();
      vopen->Modified();


      bctime.Start();
      bclose->Update();
      bctime.Stop();
      bclose->Modified();

      hctime.Start();
      hclose->Update();
      hctime.Stop();
      hclose->Modified();

      actime.Start();
      aclose->Update();
      actime.Stop();
      aclose->Modified();

      vctime.Start();
      vclose->Update();
      vctime.Stop();
      vclose->Modified();

      }
      
    std::cout << std::setprecision(3) << *it << "\t" 
              << bdtime.GetMeanTime() << "\t" 
              << hdtime.GetMeanTime() << "\t" 
              << smhdtime.GetMeanTime() << "\t" 
              << adtime.GetMeanTime() << "\t" 
              << vdtime.GetMeanTime() << "\t" 
              << betime.GetMeanTime() << "\t" 
              << hetime.GetMeanTime() << "\t" 
              << smhetime.GetMeanTime() << "\t" 
              << aetime.GetMeanTime() << "\t" 
              << vetime.GetMeanTime() << "\t" 
              << bgtime.GetMeanTime() << "\t" 
              << hgtime.GetMeanTime() << "\t" 
              << agtime.GetMeanTime() << "\t" 
              << vgtime.GetMeanTime() << "\t" 
              << botime.GetMeanTime() << "\t" 
              << hotime.GetMeanTime() << "\t" 
              << aotime.GetMeanTime() << "\t" 
              << votime.GetMeanTime() << "\t" 
              << bctime.GetMeanTime() << "\t" 
              << hctime.GetMeanTime() << "\t" 
              << actime.GetMeanTime() << "\t" 
              << vctime.GetMeanTime() << "\t" 
//<< erode->GetNameOfBackendFilterClass()
              <<std::endl;
    }
  
  
  return 0;
}

