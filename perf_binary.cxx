#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkMovingHistogramDilateImageFilter.h"
#include "itkBasicDilateImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkTimeProbe.h"
#include "itkMultiThreader.h"
#include <vector>

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
  
  typedef itk::BinaryBallStructuringElement< PType, dim > SRType;
  
  typedef itk::MovingHistogramDilateImageFilter< IType, IType, SRType > HDilateType;
  HDilateType::Pointer hdilate = HDilateType::New();
  hdilate->SetInput( reader->GetOutput() );
  
  typedef itk::BasicDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilate = DilateType::New();
  dilate->SetInput( reader->GetOutput() );
  
  typedef itk::BinaryDilateImageFilter< IType, IType, SRType > BDilateType;
  BDilateType::Pointer bdilate = BDilateType::New();
  bdilate->SetInput( reader->GetOutput() );
  
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
            << "nb" << "\t" 
            << "d" << "\t" 
            << "hd" << "\t" 
            << "bd" << std::endl;

  for( std::vector< int >::iterator it=radiusList.begin(); it !=radiusList.end() ; it++)
    {
    itk::TimeProbe dtime;
    itk::TimeProbe hdtime;
    itk::TimeProbe bdtime;

    SRType kernel;
    kernel.SetRadius( *it );
    kernel.CreateStructuringElement();

    // compute the number of activated neighbors in the structuring element
    unsigned long nbOfNeighbors = 0;
    for( SRType::Iterator nit=kernel.Begin(); nit!=kernel.End(); nit++ )
      {
      if( *nit > 0 )
        { nbOfNeighbors++; }
      }

  
    dilate->SetKernel( kernel );
    bdilate->SetKernel( kernel );
    hdilate->SetKernel( kernel );
    
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
      dtime.Start();
      dilate->Update();
      dtime.Stop();
      dilate->Modified();

      hdtime.Start();
      hdilate->Update();
      hdtime.Stop();
      hdilate->Modified();
      
      bdtime.Start();
      bdilate->Update();
      bdtime.Stop();
      bdilate->Modified();
      
      }
      
    std::cout << *it << "\t"
              << nbOfNeighbors << "\t" 
              << dtime.GetMeanTime() << "\t" 
              << hdtime.GetMeanTime() << "\t" 
              << bdtime.GetMeanTime() << std::endl;
    }
  
  
  return 0;
}

