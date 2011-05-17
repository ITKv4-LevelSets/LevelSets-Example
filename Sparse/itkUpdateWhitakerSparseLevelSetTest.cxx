/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include <string>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryImageToWhitakerSparseLevelSetAdaptor.h"
#include "itkUpdateSparseLevelSet.h"
#include "itkImageRegionIterator.h"

int main( int argc, char* argv[] )
{
  const unsigned int Dimension = 2;

  typedef unsigned char InputPixelType;
  typedef double        OutputPixelType;

  typedef itk::Image< InputPixelType, Dimension >                  InputImageType;
  typedef itk::Image< OutputPixelType, Dimension >                 OutputImageType;

  typedef itk::ImageFileReader< InputImageType >                   InputReaderType;
  typedef itk::ImageFileWriter< OutputImageType >                  OutputWriterType;
  typedef itk::BinaryImageToWhitakerSparseLevelSetAdaptor< InputImageType,
      OutputPixelType > BinaryToSparseAdaptorType;

  typedef BinaryToSparseAdaptorType::LevelSetType                  SparseLevelSetType;
  typedef SparseLevelSetType::SparseImageType                      SparseImageType;
  typedef SparseLevelSetType::NodeAttributeType                    NodeAttributeType;


  typedef itk::ImageRegionIterator< SparseImageType > SparseIteratorType;
  typedef itk::ImageRegionIterator< OutputImageType > OutputIteratorType;

  InputReaderType::Pointer reader = InputReaderType::New();
  reader->SetFileName( argv[1] );
  try
    {
    reader->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    }
  InputImageType::Pointer input = reader->GetOutput();
  std::cout << "Input image read" << std::endl;

  BinaryToSparseAdaptorType::Pointer adaptor = BinaryToSparseAdaptorType::New();
  adaptor->SetInputImage( input );
  adaptor->Initialize();
  std::cout << "Finished converting to sparse format" << std::endl;

  SparseLevelSetType::Pointer sparseLevelSet = adaptor->GetSparseLevelSet();

  typedef itk::UpdateSparseLevelSet< Dimension, OutputPixelType > UpdateLevelSetType;
  UpdateLevelSetType::Pointer update_levelset = UpdateLevelSetType::New();
  update_levelset->SetSparseLevelSet( sparseLevelSet );

  UpdateLevelSetType::UpdateListType* update_list =
      new UpdateLevelSetType::UpdateListType;

  SparseLevelSetType::NodeListIterator list_it = sparseLevelSet->GetListNode( 0 )->begin();
  SparseLevelSetType::NodeListIterator list_end = sparseLevelSet->GetListNode( 0 )->end();

  size_t k = 0;

  while( list_it != list_end )
    {
    if( atoi( argv[2]) == 2 )
      {
      update_list->push_back( -1. );
      }
    else
      {
      if( atoi( argv[2] ) == 0 )
        {
        update_list->push_back( 1. );
        }
      else
        {
        if( ( k % 20 ) < 10 )
          {
          update_list->push_back( -1. );
          }
        else
          {
          update_list->push_back( 1. );
          }
        }
      }
    ++k;
    ++list_it;
    }

  update_levelset->SetUpdate( update_list );
  update_levelset->Update();

  delete update_list;

  NodeAttributeType p;
  SparseIteratorType sIt( sparseLevelSet->GetImage(),
                         sparseLevelSet->GetImage()->GetLargestPossibleRegion() );
  sIt.GoToBegin();

  OutputImageType::Pointer output = OutputImageType::New();
  output->SetRegions( sparseLevelSet->GetImage()->GetLargestPossibleRegion() );
  output->CopyInformation( sparseLevelSet->GetImage() );
  output->Allocate();
  output->FillBuffer( 0.0 );

  OutputIteratorType oIt( output,
                          output->GetLargestPossibleRegion() );
  oIt.GoToBegin();
  while( !oIt.IsAtEnd() )
    {
    p = sIt.Get();
    oIt.Set( p.m_Value );
    ++oIt;
    ++sIt;
    }

  OutputWriterType::Pointer writer = OutputWriterType::New();
  writer->SetFileName( argv[3] );
  writer->SetInput( output );

  try
    {
    writer->Update();
    }
  catch ( itk::ExceptionObject& err )
    {
    std::cout << err << std::endl;
    }

  return EXIT_SUCCESS;
}
