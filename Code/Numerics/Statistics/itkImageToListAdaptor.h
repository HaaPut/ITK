/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkImageToListAdaptor.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToListAdaptor_h
#define __itkImageToListAdaptor_h

#include <typeinfo>

#include "itkImage.h"
#include "itkPixelTraits.h"
#include "itkListSampleBase.h"
#include "itkSmartPointer.h"
#include "itkImageRegionIterator.h"
#include "itkFixedArray.h"
#include "itkMacro.h"

namespace itk{ 
namespace Statistics{

/** \class ImageToListAdaptor
 *  \brief This class provides ListSampleBase interfaces to ITK Image
 *
 * After calling SetImage( const Image * ) method to plug in the image object,
 * users can use Sample interfaces to access Image data.
 * However, the resulting data are a list of measurement vectors. The type of
 * data is measurement vector. For example, if the pixel type of Image object 
 * is STL vector< float > and each pixel has two different types of 
 * measurements, intensity and gradient magnitude, this adaptor has
 * measurement vector of type ITK Point< float, 2>, and one element of the Point
 * is intensity and the other is gradient magnitude.
 *
 * There are two concepts of dimensions for this container. One is for Image 
 * object, and the other is for measurement vector dimension.
 * Only when using ITK Index to access data, the former concept is applicable
 * Otherwise, dimensions means dimensions of measurement vectors. 
 *
 * From the above example, there were two elements in a pixel and each pixel
 * provides [] operator for accessing its elements. However, in many cases,
 * The pixel might be a scalar value such as int or float. In this case,
 * The pixel doesn't support [] operator. To deal with this problem,
 * This class has two companion classes, ScalarAccessor and VectorAccessor.
 * If the pixel type is a scalar type, then you don't have change the third
 * template argument. If you have pixel type is vector one and supports
 * [] operator, then replace third argument with VectorAccessor
 *
 * \sa Sample, ListSampleBase
 */

template < class TImage,
           class TMeasurementVector = 
           ITK_TYPENAME TImage::PixelType >
class ITK_EXPORT ImageToListAdaptor : 
    public ListSampleBase< TMeasurementVector >
{
public:
  /** Standard class typedefs */
  typedef ImageToListAdaptor Self;
  typedef ListSampleBase< TMeasurementVector > Superclass;
  typedef SmartPointer< Self > Pointer;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToListAdaptor, ListSampleBase) ;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self) ;
  
  /** Image typedefs */
  typedef TImage ImageType;
  typedef typename ImageType::Pointer ImagePointer ;
  typedef typename ImageType::ConstPointer ImageConstPointer ;
  typedef typename ImageType::IndexType IndexType ;
  typedef typename ImageType::PixelType PixelType ;
  typedef typename ImageType::PixelContainerConstPointer PixelContainerConstPointer ;
  typedef typename ImageType::PixelContainer::ElementIdentifier 
  InstanceIdentifier;
  
  /** Image Iterator typedef support */
  typedef ImageRegionIterator< ImageType > IteratorType ; 
  typedef PixelTraits< typename TImage::PixelType > PixelTraitsType ;

  /** the number of components in a measurement vector */
  itkStaticConstMacro(MeasurementVectorSize, unsigned int,
                      PixelTraitsType::Dimension);

  /** Superclass typedefs for Measurement vector, measurement, 
   * Instance Identifier, frequency, size, size element value */
  typedef typename PixelTraitsType::ValueType MeasurementType ;
  typedef typename Superclass::FrequencyType FrequencyType ;

  typedef TMeasurementVector MeasurementVectorType ;
  typedef MeasurementVectorType ValueType ;

  /** Method to set the image */
  void SetImage(const TImage* image) ;

  /** Method to get the image */
  const TImage* GetImage() const;

  /** returns the number of measurement vectors in this container*/
  unsigned int Size() const ;

  inline virtual const MeasurementVectorType & GetMeasurementVector(const InstanceIdentifier &id) const;

  inline FrequencyType GetFrequency(const InstanceIdentifier &id) const ;

  FrequencyType GetTotalFrequency() const ;

  class Iterator;
  friend class Iterator;
  
  Iterator Begin()
  { 
    Iterator iter(0, this);
    return iter; 
  }
  
  Iterator End()        
  {
    Iterator iter(this->Size(), this); 
    return iter; 
  }
  
  class Iterator
  {
  public:
    
    Iterator(){}
    
    Iterator(InstanceIdentifier id, Pointer pContainer)
      :m_Id(id),m_Container(pContainer)
    {}
    
    FrequencyType GetFrequency() const
    { return 1 ;}

    const MeasurementVectorType & GetMeasurementVector() const
    { return m_Container->GetMeasurementVector(m_Id) ;} 

    InstanceIdentifier GetInstanceIdentifier() const
    { return m_Id ;}

    Iterator& operator++()
    { ++m_Id ; return *this ;}
    
    Iterator& operator+()
    { m_Id += n; return *this ;}

    Iterator& operator+(int n)
    { m_Id += n; return *this ;}
    
    Iterator& operator-(int n)
    { m_Id -= n; return *this ;}

    bool operator!=(const Iterator &it)
    {
      if (m_Id != it.m_Id)
        {return true ;}

      if (m_Container != it.m_Container)
        { return true ;}

      return false ;
    }
    
    bool operator==(const Iterator &it)
    { return !(*this != it);}
    
    Iterator& operator = (const Iterator &iter)
    { 
      m_Id = iter.m_Id; 
      m_Container = iter.m_Container ; 
      return *this ;
    }

    Iterator(const Iterator &iter)
    { 
      m_Id = iter.m_Id; 
      m_Container = iter.m_Container ; 
    }
    
  private:
    InstanceIdentifier m_Id;  // Current id 
    Pointer m_Container ;
  } ;
  
protected:
  ImageToListAdaptor() ;
  virtual ~ImageToListAdaptor() {}
  void PrintSelf(std::ostream& os, Indent indent) const;  

  PixelContainerConstPointer m_PixelContainer ;
  bool m_UseBuffer ;
  typename TImage::IndexType m_ImageBeginIndex ;
  typename TImage::IndexType m_ImageEndIndex ;

private:
  ImageToListAdaptor(const Self&) ; //purposely not implemented
  void operator=(const Self&) ; //purposely not implemented

  ImageConstPointer m_Image ;
} ; // end of class ImageToListAdaptor

} // end of namespace Statistics
} // end of namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageToListAdaptor.txx"
#endif

#endif
