set(DOCUMENTATION "This module provides interfaces to FFT
implementations. In particular it provides the direct and inverse
computations of Fast Fourier Transforms based on
<a href=\"http://vxl.sourceforge.net/\">VXL</a> and
<a href=\"http://www.fftw.org\">FFTW</a>. Note that when using the FFTW
implementation you must comply with the GPL license.")

itk_module(ITKFFT
  ENABLE_SHARED
  DEPENDS
    ITKCommon
  COMPILE_DEPENDS
    ITKImageGrid
  TEST_DEPENDS
    ITKTestKernel
    ITKImageCompare
  DESCRIPTION
    "${DOCUMENTATION}"
)
