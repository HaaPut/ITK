/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkKdTree.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkKdTree_txx
#define __itkKdTree_txx


namespace itk{ 
namespace Statistics{

template< class TSample >
KdTreeNonterminalNode< TSample >
::KdTreeNonterminalNode(unsigned int partitionDimension,
                        MeasurementType partitionValue,
                        Superclass* left,
                        Superclass* right)
{
  m_PartitionDimension = partitionDimension ;
  m_PartitionValue = partitionValue ;
  m_Left = left ;
  m_Right = right ;
}

template< class TSample >
void
KdTreeNonterminalNode< TSample >
::GetParameters(unsigned int &partitionDimension, 
                MeasurementType &partitionValue)
{
  partitionDimension = m_PartitionDimension ;
  partitionValue = m_PartitionValue ;
}

template< class TSample >
KdTreeWeightedCentroidNonterminalNode< TSample >
::KdTreeWeightedCentroidNonterminalNode(unsigned int partitionDimension,
                                         MeasurementType partitionValue,
                                         Superclass* left,
                                         Superclass* right,
                                         CentroidType &centroid,
                                         unsigned int size)
{
  m_PartitionDimension = partitionDimension ;
  m_PartitionValue = partitionValue ;
  m_Left = left ;
  m_Right = right ;
  m_WeightedCentroid = centroid ;

  for (unsigned int i = 0 ; i < TSample::MeasurementVectorSize ; i++)
    {
    m_Centroid[i] = m_WeightedCentroid[i] / double(size) ;
    }

  m_Size = size ;
}

template< class TSample >
void
KdTreeWeightedCentroidNonterminalNode< TSample >
::GetParameters(unsigned int &partitionDimension, 
                MeasurementType &partitionValue)
{
  partitionDimension = m_PartitionDimension ;
  partitionValue = m_PartitionValue ;
}

template< class TSample >
KdTree< TSample >
::KdTree()
{
  m_EmptyTerminalNode = 
    new KdTreeTerminalNode< TSample >() ;

  m_DistanceMetric = DistanceMetricType::New() ;
  m_Sample = 0 ;
  m_Root = 0 ;
  m_BucketSize = 16 ;
}

template< class TSample >
KdTree< TSample >
::~KdTree()
{
  if ( m_Root != 0 )
    {
    DeleteNode(m_Root) ;
    }
  
  delete m_EmptyTerminalNode ;
}

template< class TSample >
void
KdTree< TSample >
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  
  os << indent << "Input Sample: " ;
  if ( m_Sample != 0 )
    {
    os << m_Sample << std::endl ;
    }
  else
    {
    os << "not set." << std::endl ;
    }

  os << indent << "Bucket Size: " << m_BucketSize << std::endl ;
  os << indent << "Root Node: " ;
  if ( m_Root != 0 )
    {
    os << m_Root << std::endl ;
    }
  else
    {
    os << "not set." << std::endl ;
    }
}

template< class TSample >
void
KdTree< TSample >
::DeleteNode(KdTreeNodeType *node)
{
  if ( node->IsTerminal() )
    {
    // terminal node
    if (node == m_EmptyTerminalNode)
      {
      // empty node
      return ;
      }
    delete node ;
    return ;
    }

  // non-terminal node
  if ( node->Left() != 0 )
    {
    DeleteNode(node->Left()) ;
    }

  if ( node->Right() != 0 )
    {
    DeleteNode(node->Right()) ;
    }

  delete node ;
}

template< class TSample >
void
KdTree< TSample >
::SetSample(TSample* sample)
{
  m_Sample = sample ;
}

template< class TSample >
void
KdTree< TSample >
::SetBucketSize(unsigned int size)
{
  m_BucketSize = size ;
}


template< class TSample >
void
KdTree< TSample >
::Search(MeasurementVectorType &query, unsigned int k,
         InstanceIdentifierVectorType& result)
{
  if (k > this->Size())
    {
    itkExceptionMacro(<<"The k value for the nearest neighbor search should be less than or equal to the number of the measurement vectors.");
    }

  m_NearestNeighbors.resize(k) ;

  MeasurementVectorType lowerBound ;
  MeasurementVectorType upperBound ;

  for (unsigned int d = 0 ; d < MeasurementVectorSize ; d++)
    {
    lowerBound[d] = NumericTraits< MeasurementType >::NonpositiveMin() ;
    upperBound[d] = NumericTraits< MeasurementType >::max() ;
    }

  m_NumberOfVisits = 0 ;
  m_StopSearch = false ;

  this->NearestNeighborSearchLoop(m_Root, query, lowerBound, upperBound) ;

  m_Neighbors = m_NearestNeighbors.GetNeighbors() ;
  result = m_Neighbors ;
}

template< class TSample >
inline int
KdTree< TSample >
::NearestNeighborSearchLoop(KdTreeNodeType* node, 
                            MeasurementVectorType &query, 
                            MeasurementVectorType &lowerBound,
                            MeasurementVectorType &upperBound)
{
  unsigned int i ;
  InstanceIdentifier tempId ;
  double tempDistance ;

  if ( node->IsTerminal() )
    {
    // terminal node
    if (node == m_EmptyTerminalNode)
      {
      // empty node
      return 0 ;
      }

    for (i = 0 ; i < node->Size() ; i++)
      {
      tempId = node->GetInstanceIdentifier(i) ;
      tempDistance = 
        m_DistanceMetric->
        Evaluate(query, m_Sample->GetMeasurementVector(tempId)) ;
      m_NumberOfVisits++ ;
      if (tempDistance < m_NearestNeighbors.GetLargestDistance() )
        {
          m_NearestNeighbors.ReplaceFarthestNeighbor(tempId, tempDistance) ;
        }
      }

    if ( BallWithinBounds(query, lowerBound, upperBound, 
                          m_NearestNeighbors.GetLargestDistance()) )
      {
      return 1 ;
      }

    return 0;
    }


  unsigned int partitionDimension ; 
  MeasurementType partitionValue ;
  MeasurementType tempValue ;
  node->GetParameters(partitionDimension, partitionValue) ;

  if (query[partitionDimension] <= partitionValue)
    {
    // search the closer child node
    tempValue = upperBound[partitionDimension] ;
    upperBound[partitionDimension] = partitionValue ;
    if (NearestNeighborSearchLoop(node->Left(), query, lowerBound, upperBound))
      {
      return 1 ;
      }
    upperBound[partitionDimension] = tempValue ;

    // search the other node, if necessary
    tempValue = lowerBound[partitionDimension] ;
    lowerBound[partitionDimension] = partitionValue ;
    if ( BoundsOverlapBall(query, lowerBound, upperBound, 
                           m_NearestNeighbors.GetLargestDistance()) )
      {
      NearestNeighborSearchLoop(node->Right(), query, lowerBound, upperBound) ;
      }
    lowerBound[partitionDimension] = tempValue ;
    }
  else
    {
    // search the closer child node
    tempValue = lowerBound[partitionDimension] ;
    lowerBound[partitionDimension] = partitionValue ;
    if (NearestNeighborSearchLoop(node->Right(), query, lowerBound, upperBound))
      {
      return 1 ;
      }
    lowerBound[partitionDimension] = tempValue ;

    // search the other node, if necessary
    tempValue = upperBound[partitionDimension] ;
    upperBound[partitionDimension] = partitionValue ;
    if ( BoundsOverlapBall(query, lowerBound, upperBound, 
                           m_NearestNeighbors.GetLargestDistance()) )
      {
      NearestNeighborSearchLoop(node->Left(), query, lowerBound, upperBound) ;
      }
    upperBound[partitionDimension] = tempValue ;
    }

  // stop or continue search
  if ( BallWithinBounds(query, lowerBound, upperBound, 
                        m_NearestNeighbors.GetLargestDistance()) )
    {
    return 1 ;
    }  

  return 0 ;
}

template< class TSample >
void
KdTree< TSample >
::Search(MeasurementVectorType &query, double radius,
         InstanceIdentifierVectorType& result)
{
  MeasurementVectorType lowerBound ;
  MeasurementVectorType upperBound ;

  for (unsigned int d = 0 ; d < MeasurementVectorSize ; d++)
    {
    lowerBound[d] = NumericTraits< MeasurementType >::NonpositiveMin() ;
    upperBound[d] = NumericTraits< MeasurementType >::max() ;
    }

  m_NumberOfVisits = 0 ;
  m_StopSearch = false ;

  m_Neighbors.clear() ;
  m_SearchRadius = radius ;
  this->SearchLoop(m_Root, query, lowerBound, upperBound) ;
  result = m_Neighbors ;
}

template< class TSample >
inline int
KdTree< TSample >
::SearchLoop(KdTreeNodeType* node, 
             MeasurementVectorType &query, 
             MeasurementVectorType &lowerBound,
             MeasurementVectorType &upperBound)
{
  unsigned int i ;
  InstanceIdentifier tempId ;
  double tempDistance ;

  if ( node->IsTerminal() )
    {
    // terminal node
    if (node == m_EmptyTerminalNode)
      {
      // empty node
      return 0 ;
      }

    for (i = 0 ; i < node->Size() ; i++)
      {
      tempId = node->GetInstanceIdentifier(i) ;
      tempDistance = 
        m_DistanceMetric->
        Evaluate(query, m_Sample->GetMeasurementVector(tempId)) ;
      m_NumberOfVisits++ ;
      if (tempDistance < m_SearchRadius )
        {
          m_Neighbors.push_back(tempId) ;
        }
      }

    if ( BallWithinBounds(query, lowerBound, upperBound, 
                          m_SearchRadius) )
      {
      return 1 ;
      }

    return 0;
    }


  unsigned int partitionDimension ; 
  MeasurementType partitionValue ;
  MeasurementType tempValue ;
  node->GetParameters(partitionDimension, partitionValue) ;

  if (query[partitionDimension] <= partitionValue)
    {
    // search the closer child node
    tempValue = upperBound[partitionDimension] ;
    upperBound[partitionDimension] = partitionValue ;
    if (SearchLoop(node->Left(), query, lowerBound, upperBound))
      {
      return 1 ;
      }
    upperBound[partitionDimension] = tempValue ;

    // search the other node, if necessary
    tempValue = lowerBound[partitionDimension] ;
    lowerBound[partitionDimension] = partitionValue ;
    if ( BoundsOverlapBall(query, lowerBound, upperBound, 
                           m_SearchRadius) )
      {
      SearchLoop(node->Right(), query, lowerBound, upperBound) ;
      }
    lowerBound[partitionDimension] = tempValue ;
    }
  else
    {
    // search the closer child node
    tempValue = lowerBound[partitionDimension] ;
    lowerBound[partitionDimension] = partitionValue ;
    if (SearchLoop(node->Right(), query, lowerBound, upperBound))
      {
      return 1 ;
      }
    lowerBound[partitionDimension] = tempValue ;

    // search the other node, if necessary
    tempValue = upperBound[partitionDimension] ;
    upperBound[partitionDimension] = partitionValue ;
    if ( BoundsOverlapBall(query, lowerBound, upperBound, 
                           m_SearchRadius) )
      {
      SearchLoop(node->Left(), query, lowerBound, upperBound) ;
      }
    upperBound[partitionDimension] = tempValue ;
    }

  // stop or continue search
  if ( BallWithinBounds(query, lowerBound, upperBound, 
                        m_SearchRadius) )
    {
    return 1 ;
    }  

  return 0 ;
}

template< class TSample >
inline bool
KdTree< TSample >
::BallWithinBounds(MeasurementVectorType &query, 
                   MeasurementVectorType &lowerBound,
                   MeasurementVectorType &upperBound,
                   double radius)
{
  unsigned int dimension ;
  for (dimension = 0 ; dimension < MeasurementVectorSize ; dimension++)
    {
    if ((m_DistanceMetric->Evaluate(query[dimension] ,
                                    lowerBound[dimension]) <= 
         radius) ||
        (m_DistanceMetric->Evaluate(query[dimension] ,
                                    upperBound[dimension]) <= 
         radius))
      {
      return false ;
      }
    }
  return true ;
}

template< class TSample >
inline bool
KdTree< TSample >
::BoundsOverlapBall(MeasurementVectorType &query, 
                    MeasurementVectorType &lowerBound,
                    MeasurementVectorType &upperBound,
                    double radius)
{
  double sum = NumericTraits< double >::Zero ;
  double temp ;
  unsigned int dimension ;
  double squaredSearchRadius = radius * radius ;
  for (dimension = 0  ; dimension < MeasurementVectorSize ; dimension++)
    {

    if (query[dimension] <= lowerBound[dimension])
      {
      temp = m_DistanceMetric->Evaluate(query[dimension], 
                                        lowerBound[dimension]) ;
      sum += temp * temp ;
      if (sum < squaredSearchRadius)
        {
        return true ;
        }
      }
    else if (query[dimension] >= upperBound[dimension])
      {
      temp = m_DistanceMetric->Evaluate(query[dimension], 
                                        upperBound[dimension]) ;
      sum += temp * temp ;
      if (sum < squaredSearchRadius)
        {
        return true ;
        }
      }
    }
  return false ;
}



template< class TSample >
void
KdTree< TSample >
::PrintTree(KdTreeNodeType *node, int level, unsigned int activeDimension)
{
  level++ ;
  if ( node->IsTerminal() )
    {
    // terminal node
    if (node == m_EmptyTerminalNode)
      {
      // empty node
      std::cout << "Empty node: level = " << level << std::endl ;
      return ;
      }

    std::cout << "Terminal: level = " << level 
              << " dim = " << activeDimension<< std::endl ;
    std::cout << "          " ;
    for (unsigned int i = 0 ; i < node->Size() ; i++)
      {
      std::cout << "[" << node->GetInstanceIdentifier(i) << "] "
                << m_Sample->GetMeasurementVector(node->GetInstanceIdentifier(i))[activeDimension] << ", " ;
      }
    std::cout << std::endl ;
    return ;
    }
  
  unsigned int partitionDimension ;
  MeasurementType partitionValue ;

  node->GetParameters(partitionDimension, partitionValue) ;
  typename KdTreeNodeType::CentroidType centroid ;
  node->GetWeightedCentroid(centroid) ;
  std::cout << "Nonterminal: level = " << level << std::endl ;
  std::cout << "             dim = " << partitionDimension << std::endl ;
  std::cout << "             value = " << partitionValue << std::endl ;
  std::cout << "             weighted centroid = " 
            << centroid ;
  std::cout << "             size = " << node->Size()<< std::endl ;
 
  PrintTree(node->Left(), level, partitionDimension) ;
  PrintTree(node->Right(), level, partitionDimension) ;
}


} // end of namespace Statistics 
} // end of namespace itk

#endif










