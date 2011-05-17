project( LevelSets-Example )

cmake_minimum_required( VERSION 2.8 )

find_package( ITK REQUIRED )

if( ITK_FOUND )
  include( ${ITK_USE_FILE} )
endif()

include_directories( BEFORE ${ITK_INCLUDE_DIRS} )

list( FIND ITK_MODULES_ENABLED ITK-LevelSetsv4 ITK-LevelSetsv4_FOUND )
if( ITK-LevelSetsv4_FOUND LESS 0 )
  message( SEND_ERROR "These examples require ITK-LevelSetsv4 to be turned ON" )
endif()

#add_subdirectory( Dense )
add_subdirectory( Sparse )
