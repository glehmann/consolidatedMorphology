#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGrayscaleDilateImageFilter.h"
#include "itkFlatStructuringElement.h"
#include "itkSimpleFilterWatcher.h"
#include "itkMultiThreader.h"
#include <iomanip>

// Dilation by octagons - results should be identical for different algorithms

int main(int, char * argv[])
{
  const int dim = 2;
  typedef unsigned char PType;
  //typedef float PType;
  typedef itk::Image< PType, dim >    IType;
  // read the input image
  typedef itk::ImageFileReader< IType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );
  
  typedef itk::FlatStructuringElement<dim> SRType;
  
  typedef itk::GrayscaleDilateImageFilter< IType, IType, SRType > DilateType;
  DilateType::Pointer dilateH = DilateType::New();
  DilateType::Pointer dilateV = DilateType::New();
  DilateType::Pointer dilateA = DilateType::New();
  dilateH->SetInput( reader->GetOutput() );
  dilateV->SetInput( reader->GetOutput() );
  dilateA->SetInput( reader->GetOutput() );
  
  std::vector< int > radiusList;
  for( int s=1; s<=10; s++)
    { radiusList.push_back( s ); }
  for( int s=15; s<=30; s+=5)
    { radiusList.push_back( s ); }
  radiusList.push_back( 40 );
  radiusList.push_back( 50 );
  radiusList.push_back( 100 );
  std::cout << "#radius" << "\t" 
	    << "histogram ball" << "\t" 
	    << "vHGW polygon" << "\t" 
	    << "anchor polygon" << std::endl;

// itkSimpleFilterWatcher watcher(dilate, "filter");

//   typedef itk::ImageFileWriter< IType > WriterType;
//   WriterType::Pointer writer = WriterType::New();
//   writer->SetInput( dilate->GetOutput() );
  for( std::vector< int >::iterator it=radiusList.begin(); it !=radiusList.end() ; it++)
    {
    int nbOfRepeats;
    if( *it <= 10 )
      { nbOfRepeats = 20; }
    else if( *it <= 20 )
      { nbOfRepeats = 10; }
    else if( *it <= 50 )
      { nbOfRepeats = 5; }
    else
      { nbOfRepeats = 2; }

      SRType::RadiusType radius;
      radius.Fill( *it );
      SRType kernelH = SRType::Ball(radius);
      // use a default number of lines
      SRType kernelD = SRType::Poly(radius, 0);
    
      dilateH->SetKernel(kernelH);
      dilateH->SetAlgorithm( DilateType::HISTO );
      dilateH->Modified();

      dilateV->SetKernel(kernelD);
      dilateV->SetAlgorithm( DilateType::VHGW );
      dilateV->Modified();

      dilateA->SetKernel(kernelD);
      dilateA->SetAlgorithm( DilateType::ANCHOR );
      dilateA->Modified();

      itk::TimeProbe htime, vtime, atime;

      for( int i=0; i<nbOfRepeats; i++ )
	{

	htime.Start();
	dilateH->Update();
	htime.Stop();
	dilateH->Modified();

	vtime.Start();
	dilateV->Update();
	vtime.Stop();
	dilateV->Modified();

	atime.Start();
	dilateA->Update();
	atime.Stop();
	dilateA->Modified();

	}
      std::cout << std::setprecision(3) << *it << "\t" 
		<< htime.GetMeanTime() << "\t"
		<< vtime.GetMeanTime() << "\t"
		<< atime.GetMeanTime() << std::endl;
    }  
  
  return 0;
}

