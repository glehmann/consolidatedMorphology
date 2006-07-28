#Change PROJECT_NAME to the name of your project
PROJECT(consolidatedMorphology)

# set the name of the input image used to run the test
SET(INPUT_IMAGE ${CMAKE_SOURCE_DIR}/images/cthead1.png)

#include some macros from another file...
INCLUDE(IJMacros.txt)

#The following lines are required to use Dart
ENABLE_TESTING()
INCLUDE(Dart)
FIND_PROGRAM(IMAGE_COMPARE ImageCompare)

#Declare any external dependencies that your project may have here.
#examples include: ITK, VTK, JPEG, PNG, OpenGL, ZLIB, Perl, Java
#If you're not sure what name to use, look in the Modules directory of your
#cmake install and check that a file named Find(Package).cmake exists
SET(Required_Packages
  ITK
)

#this foreach loads all of the packages that you specified as required.
#It shouldn't need to be modified.
FOREACH(Package ${Required_Packages})
  LOADPACKAGE(${Package})
ENDFOREACH(Package)

#Set any libraries that your project depends on.
#examples: ITKCommon, VTKRendering, etc
SET(Libraries
  ITKCommon
  ITKBasicFilters
  ITKIO
)


# install devel files
OPTION(INSTALL_DEVEL_FILES "Install C++ headers" ON)
IF(INSTALL_DEVEL_FILES)
FILE(GLOB develFiles *.h *.txx) 
FOREACH(f ${develFiles})
  INSTALL_FILES(/include/InsightToolkit/BasicFilters FILES ${f})
ENDFOREACH(f)
ENDIF(INSTALL_DEVEL_FILES)



# option for wrapping
OPTION(BUILD_WRAPPERS "Wrap library" OFF)
IF(BUILD_WRAPPERS)
   SUBDIRS(Wrapping)
ENDIF(BUILD_WRAPPERS)
   
   

#the following block of code is an example of how to build an executable in
#cmake.  Unmodified, it will add an executable called "MyExe" to the project.
#MyExe will be built using the files MyClass.h and MyClass.cxx, and it will
#be linked to all the libraries you specified above. 
#You can build more than one executable per project

IF(BUILD_TESTING)

SET(CurrentExe "testSE")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "dilate2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "dilate2D_std_kernel")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})


SET(CurrentExe "erode2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "erode2D_std_kernel")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})


SET(CurrentExe "gradient2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "gradient2D_std_kernel")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})


SET(CurrentExe "open2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "open2D_std_kernel")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})


SET(CurrentExe "close2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "close2D_std_kernel")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})


SET(CurrentExe "whiteTopHat2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "whiteTopHat2D_std_kernel")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})


SET(CurrentExe "blackTopHat2D")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

SET(CurrentExe "blackTopHat2D_std_kernel")
ADD_EXECUTABLE(${CurrentExe} ${CurrentExe}.cxx)
TARGET_LINK_LIBRARIES(${CurrentExe} ${Libraries})

ENDIF(BUILD_TESTING)

#the following line is an example of how to add a test to your project.
#Testname is the title for this particular test.  ExecutableToRun is the
#program which will be running this test.  It can either be a part of this
#project or an external executable.  After that list any args that are needed
#for this test.  Include as many tests as you like.  If your project doesn't have
#any tests you can comment out or delete the following line.
# ADD_TEST(Testname ExecutableToRun arg1 arg2 arg3)

ADD_TEST(Dilate2D dilate2D ${INPUT_IMAGE} dilate2D-basic.png
dilate2D-histo.png dilate2D-anchor.png)
ADD_TEST(Dilate2DBasicCompare ${IMAGE_COMPARE} dilate2D-basic.png
${CMAKE_SOURCE_DIR}/images/dilate2D.png)
ADD_TEST(Dilate2DHistoCompare ${IMAGE_COMPARE} dilate2D-histo.png
${CMAKE_SOURCE_DIR}/images/dilate2D.png)
ADD_TEST(Dilate2DAnchorCompare ${IMAGE_COMPARE} dilate2D-anchor.png
${CMAKE_SOURCE_DIR}/images/dilate2D.png)

ADD_TEST(Dilate2Dstd dilate2D_std_kernel ${INPUT_IMAGE} dilate2D-std-basic.png
dilate2D-std-histo.png)
ADD_TEST(Dilate2DstdBasicCompare ${IMAGE_COMPARE} dilate2D-std-basic.png
${CMAKE_SOURCE_DIR}/images/dilate2D-std.png)
ADD_TEST(Dilate2DstdHistoCompare ${IMAGE_COMPARE} dilate2D-std-histo.png
${CMAKE_SOURCE_DIR}/images/dilate2D-std.png)


ADD_TEST(Erode2D erode2D ${INPUT_IMAGE} erode2D-basic.png
erode2D-histo.png erode2D-anchor.png)
ADD_TEST(Erode2DBasicCompare ${IMAGE_COMPARE} erode2D-basic.png
${CMAKE_SOURCE_DIR}/images/erode2D.png)
ADD_TEST(Erode2DHistoCompare ${IMAGE_COMPARE} erode2D-histo.png
${CMAKE_SOURCE_DIR}/images/erode2D.png)
ADD_TEST(Erode2DAnchorCompare ${IMAGE_COMPARE} erode2D-anchor.png
${CMAKE_SOURCE_DIR}/images/erode2D.png)

ADD_TEST(Erode2Dstd erode2D_std_kernel ${INPUT_IMAGE} erode2D-std-basic.png
erode2D-std-histo.png)
ADD_TEST(Erode2DstdBasicCompare ${IMAGE_COMPARE} erode2D-std-basic.png
${CMAKE_SOURCE_DIR}/images/erode2D-std.png)
ADD_TEST(Erode2DstdHistoCompare ${IMAGE_COMPARE} erode2D-std-histo.png
${CMAKE_SOURCE_DIR}/images/erode2D-std.png)


ADD_TEST(Gradient2D gradient2D ${INPUT_IMAGE} gradient2D-basic.png
gradient2D-histo.png gradient2D-anchor.png)
ADD_TEST(Gradient2DBasicCompare ${IMAGE_COMPARE} gradient2D-basic.png
${CMAKE_SOURCE_DIR}/images/gradient2D.png)
ADD_TEST(Gradient2DHistoCompare ${IMAGE_COMPARE} gradient2D-histo.png
${CMAKE_SOURCE_DIR}/images/gradient2D.png)
ADD_TEST(Gradient2DAnchorCompare ${IMAGE_COMPARE} gradient2D-anchor.png
${CMAKE_SOURCE_DIR}/images/gradient2D.png)

ADD_TEST(Gradient2Dstd gradient2D_std_kernel ${INPUT_IMAGE} gradient2D-std-basic.png
gradient2D-std-histo.png)
ADD_TEST(Gradient2DstdBasicCompare ${IMAGE_COMPARE} gradient2D-std-basic.png
${CMAKE_SOURCE_DIR}/images/gradient2D-std.png)
ADD_TEST(Gradient2DstdHistoCompare ${IMAGE_COMPARE} gradient2D-std-histo.png
${CMAKE_SOURCE_DIR}/images/gradient2D-std.png)



ADD_TEST(Open2D open2D ${INPUT_IMAGE} open2D-basic.png
open2D-histo.png open2D-anchor.png)
ADD_TEST(Open2DBasicCompare ${IMAGE_COMPARE} open2D-basic.png
${CMAKE_SOURCE_DIR}/images/open2D.png)
ADD_TEST(Open2DHistoCompare ${IMAGE_COMPARE} open2D-histo.png
${CMAKE_SOURCE_DIR}/images/open2D.png)
ADD_TEST(Open2DAnchorCompare ${IMAGE_COMPARE} open2D-anchor.png
${CMAKE_SOURCE_DIR}/images/open2D.png)

ADD_TEST(Open2Dstd open2D_std_kernel ${INPUT_IMAGE} open2D-std-basic.png
open2D-std-histo.png)
ADD_TEST(Open2DstdBasicCompare ${IMAGE_COMPARE} open2D-std-basic.png
${CMAKE_SOURCE_DIR}/images/open2D-std.png)
ADD_TEST(Open2DstdHistoCompare ${IMAGE_COMPARE} open2D-std-histo.png
${CMAKE_SOURCE_DIR}/images/open2D-std.png)



ADD_TEST(Close2D close2D ${INPUT_IMAGE} close2D-basic.png
close2D-histo.png close2D-anchor.png)
ADD_TEST(Close2DBasicCompare ${IMAGE_COMPARE} close2D-basic.png
${CMAKE_SOURCE_DIR}/images/close2D.png)
ADD_TEST(Close2DHistoCompare ${IMAGE_COMPARE} close2D-histo.png
${CMAKE_SOURCE_DIR}/images/close2D.png)
ADD_TEST(Close2DAnchorCompare ${IMAGE_COMPARE} close2D-anchor.png
${CMAKE_SOURCE_DIR}/images/close2D.png)

ADD_TEST(Close2Dstd close2D_std_kernel ${INPUT_IMAGE} close2D-std-basic.png
close2D-std-histo.png)
ADD_TEST(Close2DstdBasicCompare ${IMAGE_COMPARE} close2D-std-basic.png
${CMAKE_SOURCE_DIR}/images/close2D-std.png)
ADD_TEST(Close2DstdHistoCompare ${IMAGE_COMPARE} close2D-std-histo.png
${CMAKE_SOURCE_DIR}/images/close2D-std.png)



ADD_TEST(WhiteTopHat2D whiteTopHat2D ${INPUT_IMAGE} whiteTopHat2D-basic.png
whiteTopHat2D-histo.png whiteTopHat2D-anchor.png)
ADD_TEST(WhiteTopHat2DBasicCompare ${IMAGE_COMPARE} whiteTopHat2D-basic.png
${CMAKE_SOURCE_DIR}/images/whiteTopHat2D.png)
ADD_TEST(WhiteTopHat2DHistoCompare ${IMAGE_COMPARE} whiteTopHat2D-histo.png
${CMAKE_SOURCE_DIR}/images/whiteTopHat2D.png)
ADD_TEST(WhiteTopHat2DAnchorCompare ${IMAGE_COMPARE} whiteTopHat2D-anchor.png
${CMAKE_SOURCE_DIR}/images/whiteTopHat2D.png)

ADD_TEST(WhiteTopHat2Dstd whiteTopHat2D_std_kernel ${INPUT_IMAGE} whiteTopHat2D-std-basic.png
whiteTopHat2D-std-histo.png)
ADD_TEST(WhiteTopHat2DstdBasicCompare ${IMAGE_COMPARE} whiteTopHat2D-std-basic.png
${CMAKE_SOURCE_DIR}/images/whiteTopHat2D-std.png)
ADD_TEST(WhiteTopHat2DstdHistoCompare ${IMAGE_COMPARE} whiteTopHat2D-std-histo.png
${CMAKE_SOURCE_DIR}/images/whiteTopHat2D-std.png)



ADD_TEST(BlackTopHat2D blackTopHat2D ${INPUT_IMAGE} blackTopHat2D-basic.png
blackTopHat2D-histo.png blackTopHat2D-anchor.png)
ADD_TEST(BlackTopHat2DBasicCompare ${IMAGE_COMPARE} blackTopHat2D-basic.png
${CMAKE_SOURCE_DIR}/images/blackTopHat2D.png)
ADD_TEST(BlackTopHat2DHistoCompare ${IMAGE_COMPARE} blackTopHat2D-histo.png
${CMAKE_SOURCE_DIR}/images/blackTopHat2D.png)
ADD_TEST(BlackTopHat2DAnchorCompare ${IMAGE_COMPARE} blackTopHat2D-anchor.png
${CMAKE_SOURCE_DIR}/images/blackTopHat2D.png)

ADD_TEST(BlackTopHat2Dstd blackTopHat2D_std_kernel ${INPUT_IMAGE} blackTopHat2D-std-basic.png
blackTopHat2D-std-histo.png)
ADD_TEST(BlackTopHat2DstdBasicCompare ${IMAGE_COMPARE} blackTopHat2D-std-basic.png
${CMAKE_SOURCE_DIR}/images/blackTopHat2D-std.png)
ADD_TEST(BlackTopHat2DstdHistoCompare ${IMAGE_COMPARE} blackTopHat2D-std-histo.png
${CMAKE_SOURCE_DIR}/images/blackTopHat2D-std.png)

