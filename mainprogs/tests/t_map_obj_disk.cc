#include "chroma.h"
#include <cstdlib>
#include <iostream>

#include "util/ferm/map_obj.h"
#include "util/ferm/map_obj/map_obj_disk.h"
#include "util/ferm/key_prop_colorvec.h"
#include "util/ferm/map_obj/map_obj_factory_w.h"
#include "util/ferm/map_obj/map_obj_aggregate_w.h"


using namespace QDP;
using namespace Chroma;



void fail()
{
  QDPIO::cout << "FAIL" << endl;
  Chroma::finalize();
  exit(1);
}

void testMapObjInsertions(MapObjectDisk<char, float>& the_map);
void testMapKeyPropColorVecInsterions(MapObject<KeyPropColorVec_t, LatticeFermion>& pc_map, const multi1d<LatticeFermion>& lf_array);

void testMapObjLookups(MapObjectDisk<char, float>& the_map);
void testMapKeyPropColorVecLookups(MapObject<KeyPropColorVec_t, LatticeFermion>& pc_map, const multi1d<LatticeFermion>& lf_array);

static std::string xml_in_str="<MapObject><MapObjType>MAP_OBJ_DISK</MapObjType><FileName>./propColorVecMapObjTest</FileName></MapObject>";

int main(int argc, char *argv[])
{


  // Register
  Chroma::MapObjectWilson4DEnv::registerKeyPropColorVecLFAll();

  Chroma::initialize(&argc, &argv);

  

  // Setup the QDP++
  const int foo[] = {2,2,2,2};
  multi1d<int> nrow(Nd);
  nrow = foo;  // Use only Nd elements
  Layout::setLattSize(nrow);
  Layout::create();

  // Params to create a map object disk
  MapObjectDiskParams p("./mapObjTest");

  // Make a disk map object -- keys are ints, data floats
  MapObjectDisk<char,float> made_map(p);

  testMapObjInsertions(made_map);
  testMapObjLookups(made_map);


  // Use factory to make a MapObjectDisk<KeyPropColorVec_t,LatticeFermion>
  try {
    std::istringstream is(xml_in_str);
    XMLReader xml_in(is);
    Handle< MapObject<KeyPropColorVec_t, LatticeFermion> > obj_handle(
								      TheMapObjKeyPropColorVecFactory::Instance().createObject("MAP_OBJ_DISK", xml_in, "/MapObject"));
    
    MapObject<KeyPropColorVec_t, LatticeFermion>& pc_map=*obj_handle;
    
    // Make an array of LF-s filled with noise
    multi1d<LatticeFermion> lf_array(10);
    for(int i=0; i < 10; i++) { 
      gaussian(lf_array[i]);
    }
    
    
    testMapKeyPropColorVecInsterions(pc_map, lf_array);
    testMapKeyPropColorVecLookups(pc_map, lf_array);
  }
  catch(const std::string& e) { 
    QDPIO::cout << "Caught: " << e << endl;
  }

  Chroma::finalize();
  return 0;
}


void testMapObjInsertions(MapObjectDisk<char, float>& the_map)
{
 // Open the map for 'filling'
  QDPIO::cout << "Opening MapObjectDisk<char,float> for writing..."; 
  if( the_map.openWrite() ) { 
    QDPIO::cout << "OK" << endl;
  }
  else { 
    fail();
  }

  QDPIO::cout << "Inserting (key,value): " << endl;
  // store a quadratic
  for(char i=0; i < 10; i++) { 
    float val = (float)i;
    val *= val;
    char key = 'a'+i; 

    try { 
      the_map.insert(key, val);
      QDPIO::cout << "  ( " << key <<", "<< val <<" )" << endl;
    }
    catch(...) { 
      fail();
    }
  }
  
  QDPIO::cout << "Closing MapObjectDisk<char,float> for writing..." ;
  if( the_map.closeWrite() ) { 
    QDPIO::cout << "... OK" << endl;
  }
  else { 
    fail();
  }

}


void testMapObjLookups(MapObjectDisk<char, float>& the_map)
{
  /* Now reopen - random access */
  QDPIO::cout << "Opening MapObjectDisk<char,float> for reading..."; 
  if( the_map.openRead() ) { 
    QDPIO::cout << "OK" << endl;
  }
  else { 
    fail();
  }

  QDPIO::cout << "Forward traversal test: " << endl;
  // Traverse in sequence
  
  QDPIO::cout << "Looking up key: " ;
  for(char i=0; i<10; i++) {
    char key = 'a'+i;
    float val;
    
    
    try{ 
      the_map.lookup(key, val);
      QDPIO::cout << " " << key;
    }
    catch(...) { 
      fail();
    }
    
    float refval = i*i;
    float diff = fabs(refval-val);
    if ( diff > 1.0e-8 ) {
      fail();
    }
    else { 
      QDPIO::cout << ".";
    }
  }

  QDPIO::cout << endl << "OK" << endl;


  QDPIO::cout << "Reverse traversal check" << endl;
  QDPIO::cout << "Looking up key: " ;
  // Traverse in reverse

  for(char i=9; i >= 0; i--) {
    char key='a'+i; 
    float val;
    try {
      the_map.lookup(key, val);
      QDPIO::cout << " " << key;
    }
    catch(...) { 
      fail();
    }

    float refval = i*i;
    float diff = fabs(refval-val);
    if ( diff > 1.0e-8 ) {
      fail();
    }
    else { 
      QDPIO::cout << "." << endl;
    }

  }
  QDPIO::cout << endl << "OK" << endl;


  // 20 'random' reads
  QDPIO::cout << "Random access... 20 reads" << endl;
  QDPIO::cout << "Looking up key: " ;
  for(int j=0; j < 20; j++){ 
    char key = 'a'+std::rand() % 10; // Pick randomly in 0..10
    float val;

    try { 
      the_map.lookup(key, val);
      QDPIO::cout << " " << key ;
    }
    catch(...) {
      fail();
    }

    float refval = (key-'a')*(key-'a');

    float diff = fabs(refval-val);
    if ( diff > 1.0e-8 ) {
      fail();
    }
    else { 
      QDPIO::cout <<".";
    }
  }
  QDPIO::cout << endl << "OK" << endl;

  QDPIO::cout << "Closing MapObjectDisk<char,float> for reading..."; 
  if( the_map.closeRead() ) { 
    QDPIO::cout << "OK" << endl;
  }
  else { 
    fail();
  }
   
  
}



void testMapKeyPropColorVecInsterions(MapObject<KeyPropColorVec_t, LatticeFermion>& pc_map, const multi1d<LatticeFermion>& lf_array)
{
  // Create the key-type
  KeyPropColorVec_t the_key = {0,0,0};

  // OpenMap for Writing
  QDPIO::cout << "Opening Map<KeyPropColorVec_t,LF> for writing..." << endl;
  if( pc_map.openWrite() ) { 
    QDPIO::cout << "OK" << endl;
  }
  else {
    fail();
  }

  QDPIO::cout << "Inserting array element : ";
  for(int i=0; i < 10; i++) { 
    the_key.colorvec_src = i;

    try { 
      pc_map.insert(the_key, lf_array[i]);
      QDPIO::cout << " "<< i << endl;
    }
    catch(...) {
      fail();
    }
  }

  QDPIO::cout << "Closing Map<KeyPropColorVec_t,LF> for writing..." << endl;
  if( pc_map.closeWrite() ) { 
    QDPIO::cout << "OK" << endl;
  }
  else { 
    fail();
  }
}

void testMapKeyPropColorVecLookups(MapObject<KeyPropColorVec_t, LatticeFermion>& pc_map, const multi1d<LatticeFermion>& lf_array)
{

  // Open map in read mode
  QDPIO::cout << "Opening Map<KeyPropColorVec_t,LF> for reading.." << endl;

  if( pc_map.openRead() ) { 
    QDPIO::cout << "OK" << endl;
  }
  else { 
    fail();
  }

  QDPIO::cout << "Increasing lookup test:" << endl;
  QDPIO::cout << "Looking up with colorvec_src = ";
  // Create the key-type
  KeyPropColorVec_t the_key = {0,0,0};

  for(int i=0; i < lf_array.size(); i++) {
    LatticeFermion lf_tmp;

    the_key.colorvec_src=i;
    try{
      pc_map.lookup(the_key, lf_tmp);
      QDPIO::cout << " " << i;
    }
    catch(...) { 
      fail();
    }

    // Compare with lf_array
    LatticeFermion diff = lf_tmp - lf_array[i];
    Double diff_norm = sqrt(norm2(diff))/Double(Nc*Ns*Layout::vol());
    if(  toDouble(diff_norm) < 1.0e-6 )  { 
      QDPIO::cout << "." ;
    }
    else { 
      fail();
    }

  }
  QDPIO::cout << endl << "OK" << endl;

  QDPIO::cout << "Random access lookup test" << endl;
  QDPIO::cout << "Looking up with colorvec_src = " ;
  // Hey DJ! Spin that disk...
  for(int j=0; j < 100; j++) {
    int i = random() % lf_array.size();
    LatticeFermion lf_tmp;
    the_key.colorvec_src=i;
    try{
      pc_map.lookup(the_key, lf_tmp);
      QDPIO::cout << " " << i;
    }
    catch(...) {
      fail();
    }

    // Compare with lf_array
    LatticeFermion diff = lf_tmp - lf_array[i];
    Double diff_norm = sqrt(norm2(diff))/Double(Nc*Ns*Layout::vol());
    if(  toDouble(diff_norm) < 1.0e-6 )  { 
      QDPIO::cout << ".";
    }
    else { 
      fail();
    }
  }

  
  QDPIO::cout << "Closing Map<KeyPropColorVec_t,LF> for reading..." << endl;
  if( pc_map.closeRead() ) {
    QDPIO::cout << endl << "OK"  << endl;
  }
  else { 
    fail();
  }


}


