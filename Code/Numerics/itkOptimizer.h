/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkOptimizer.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

Copyright (c) 2001 Insight Consortium
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * The name of the Insight Consortium, nor the names of any consortium members,
   nor of any contributors, may be used to endorse or promote products derived
   from this software without specific prior written permission.

  * Modified source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#ifndef __itkOptimizer_h
#define __itkOptimizer_h

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "itkExceptionObject.h"
#include "itkScaleTransform.h"


namespace itk
{
  
/** \class Optimizer
 * \brief Generic representation for an optimization method 
 *
 * \ingroup Numerics
 */
template <class TCostFunction>
class ITK_EXPORT Optimizer : public Object 
{
public:
  /**
   * Standard "Self" typedef.
   */
  typedef Optimizer  Self;

  /**
   * Standard "Superclass" typedef.
   */
  typedef   Object  Superclass;

  /** 
   * Smart pointer typedef support 
   */
  typedef SmartPointer<Self>   Pointer;
  typedef SmartPointer<const Self>  ConstPointer;


 /** 
   * Run-time type information (and related methods).
   */
  itkTypeMacro(Optimizer, Object);


  /**
   * Method for creation through the object factory.
   */
  itkNewMacro(Self);


  /**
   *  Parameters type.
   *  it defines a position in the optimization search space
   */
  typedef typename TCostFunction::ParametersType ParametersType;


  /**
   * Dimension of the Search Space
   */
  enum { SpaceDimension = TCostFunction::SpaceDimension };
 
  /**
   *  Transform type.
   *  it defines a transform to be applied to points before 
   *  being evaluated in the cost function. This allows to 
   *  map to a more convenient space. In particular this is
   *  used to normalize parameter spaces in which some parameters
   *  have a different dynamic range.  
   */
  typedef     ScaleTransform<double,SpaceDimension>      TransformType;


  /**
   *  Measure type.
   *  it defines a type used to return the cost function value 
   */
  typedef typename TCostFunction::MeasureType MeasureType;


  /**
   *  Derivative type.
   *  it defines a type used to return the cost function derivative 
   */
  typedef typename TCostFunction::DerivativeType DerivativeType;


  /**
   *   Set the position to initialize the optimization  
   */
  itkSetMacro(InitialPosition, ParametersType);


  /**
   *   Get the position to initialize the optimization  
   */
  itkGetConstMacro(InitialPosition, ParametersType);


  /**
   *   Get current position of the optimization  
   */
  itkGetConstMacro( CurrentPosition, ParametersType );


  /**
   *   Set current transform
   */
  itkSetObjectMacro( Transform, TransformType );


  /**
   *   Get current transform
   */
  itkGetObjectMacro( Transform, TransformType );


protected:

  Optimizer() {
     m_Transform = TransformType::New();
     };
  virtual ~Optimizer() {};
  Optimizer(const Self&) {}
  void operator=(const Self&) {}

  /**
   *   Set the current position 
   */
  itkSetMacro( CurrentPosition, ParametersType );


private:
  
  ParametersType          m_InitialPosition;
  ParametersType          m_CurrentPosition;

  typename TransformType::Pointer  m_Transform; 

};

} // end namespace itk

#endif



