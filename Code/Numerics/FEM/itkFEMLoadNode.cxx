/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkFEMLoadNode.cxx
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

// disable debug warnings in MS compiler
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

#include "itkFEMLoadNode.h"
#include "itkFEMUtility.h"

namespace itk {
namespace fem {




/**
 * Read the LoadNode object from input stream
 */
void LoadNode::Read( std::istream& f, void* info )
{
  int n;
  /**
   * Convert the info pointer to a usable objects
   */
  Node::ArrayType::ConstPointer nodes=static_cast<ReadInfoType*>(info)->m_node;


  /** first call the parent's read function */
  Superclass::Read(f,info);

  /** read and set pointer to node that we're applying the load to */
  SkipWhiteSpace(f); f>>n; if(!f) goto out;
  try
  {
    node=dynamic_cast<const Node*>( &*nodes->Find(n));
  }
  catch ( FEMExceptionObjectNotFound e )
  {
    throw FEMExceptionObjectNotFound(__FILE__,__LINE__,"LoadNode::Read()",e.m_baseClassName,e.m_GN);
  }
  
  /** read and set the number of elements inside a force vector */
  SkipWhiteSpace(f); f>>n; if(!f) goto out;
  F.resize(n);  

  /** read the force vector itself */
  SkipWhiteSpace(f); f>>F; if(!f) goto out;

out:

  if( !f )
  {
    throw FEMExceptionIO(__FILE__,__LINE__,"LoadNode::Read()","Error reading FEM load!");
  }

}




/**
 * Write the LoadNode to the output stream
 */
void LoadNode::Write( std::ostream& f, int ofid ) const {

  /** if not set already, se set the ofid */
  if (ofid<0) ofid=OFID;

  /** first call the parent's write function */
  Superclass::Write(f,ofid);

  /** write the actual Load data */
  f<<"\t"<<node->GN<<"\t% GN of node on which the load acts"<<"\n";
  f<<"\t"<<F.size()<<" "<<F<<"\t% Force vector (first number is the size of a vector)\n";

  /** check for errors */
  if (!f)
  {
    throw FEMExceptionIO(__FILE__,__LINE__,"LoadNode::Write()","Error writing FEM load!");
  }

}

FEM_CLASS_REGISTER(LoadNode)




}} // end namespace itk::fem
