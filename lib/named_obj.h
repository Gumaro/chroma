// -*- C++ -*-
// $Id: named_obj.h,v 2.1 2005-09-26 19:49:36 edwards Exp $

/*! @file
 * @brief Named object support
 */

/*! \defgroup support Support routines
 * \ingroup lib
 *
 * Support routines
 */

#ifndef __named_obj_h__
#define __named_obj_h__

#include "chromabase.h"

#include <map>
#include <string>

namespace Chroma
{
  //--------------------------------------------------------------------------------------
  //! Typeinfo Hiding Base Clase
  /*! @ingroup support
   */
  class NamedObjectBase 
  {
  public:
    NamedObjectBase() {}

    //! Setter
    virtual void setFileXML(XMLReader& xml) = 0;

    //! Setter
    virtual void setFileXML(XMLBufferWriter& xml) = 0;

    //! Setter
    virtual void setRecordXML(XMLReader& xml) = 0;

    //! Setter
    virtual void setRecordXML(XMLBufferWriter& xml) = 0;

    //! Getter
    virtual void getFileXML(XMLReader& xml) const = 0;

    //! Getter
    virtual void getFileXML(XMLBufferWriter& xml) const = 0;

    //! Getter
    virtual void getRecordXML(XMLReader& xml) const = 0;

    //! Getter
    virtual void getRecordXML(XMLBufferWriter& xml) const = 0;

    // This is key for cleanup
    virtual ~NamedObjectBase() {}
  };


  //--------------------------------------------------------------------------------------
  //! Type specific named object
  /*! @ingroup support
   */
  template<typename T>
  class NamedObject : public NamedObjectBase 
  {
  public:
    //! Constructor
    NamedObject() {}
  
    //! Destructor
    ~NamedObject() {}

    //! Setter
    void setFileXML(XMLReader& xml) 
    { 
      std::ostringstream os;
      xml.printCurrentContext(os);
      file_xml = os.str();
    }

    //! Setter
    void setFileXML(XMLBufferWriter& xml) 
    {
      file_xml = xml.printCurrentContext();
    }

    //! Setter
    void setRecordXML(XMLReader& xml) 
    {
      std::ostringstream os;
      xml.printCurrentContext(os);
      record_xml = os.str();
    }

    //! Setter
    void setRecordXML(XMLBufferWriter& xml) 
    {
      record_xml = xml.printCurrentContext();
    }

    //! Getter
    void getFileXML(XMLReader& xml) const 
    {
      std::istringstream os(file_xml);
      xml.open(os);
    }

    //! Getter
    void getFileXML(XMLBufferWriter& xml) const 
    {
      xml.writeXML(file_xml);
    }

    //! Getter
    void getRecordXML(XMLReader& xml) const 
    {
      std::istringstream os(record_xml);
      xml.open(os);
    }

    //! Getter
    void getRecordXML(XMLBufferWriter& xml) const 
    {
      xml.writeXML(record_xml);
    }

    //! Mutable data ref
    virtual T& getData() {
      return data;
    }

    //! Const data ref
    virtual const T& getData() const {
      return data;
    }

  private:
    T           data;
    std::string file_xml;
    std::string record_xml;
  };


  //--------------------------------------------------------------------------------------
  //! The Map Itself
  /*! @ingroup support
   */
  class NamedObjectMap 
  {
  public:
    // Creation: clear the map
    NamedObjectMap() {
      the_map.clear();
    };

    // Destruction: erase all elements of the map
    ~NamedObjectMap() 
    {
      typedef std::map<std::string, NamedObjectBase*>::iterator I;
      while( ! the_map.empty() ) 
      {
	I iter = the_map.begin();

	delete iter->second;

	the_map.erase(iter);
      }
    }

    //! Create an entry of arbitrary type.
    template<typename T>
    void create(const std::string& id) 
    {
      // Lookup and throw exception if duplicate found
      typedef std::map<std::string, NamedObjectBase*>::iterator I;
      I iter = the_map.find(id);
      if( iter != the_map.end()) 
      {
	ostringstream error_stream;
        error_stream << "NamedObjectMap::create : duplicate id = " << id << endl;
        throw error_stream.str();
      }

      // Create a new object of specified type (empty)
      // Dynamic cast to Typeless base clasee.
      // Note multi1d's need to be loked up and resized appropriately
      // and no XML files are added at this point
      the_map[id] = dynamic_cast<NamedObjectBase*>(new NamedObject<T>());
    }

    //! Delete an item that we no longer neeed
    void erase(const std::string& id) 
    {
      // Do a lookup
      typedef std::map<std::string, NamedObjectBase*>::iterator I;
      I iter = the_map.find(id);
    
      // If found then delete it.
      if( iter != the_map.end() ) 
      { 
      	// Delete the data.of the record
	delete iter->second;

	// Delete the record
	the_map.erase(iter);
      }
      else 
      {
	// We attempt to erase something non existent
	ostringstream error_stream;
        error_stream << "NamedObjectMap::erase : erasing unknown id = " << id << endl;
        throw error_stream.str();
      }
    }
  
  
    //! Dump out all objects
    void dump() 
    {
      typedef std::map<std::string, NamedObjectBase*>::iterator CI;
      QDPIO::cout << "Available Keys are : " << endl;
	for(CI j = the_map.begin(); j != the_map.end(); j++) 
	  QDPIO::cout << j->first << endl << flush;
    }
  
  
    //! Look something up and return a NamedObjectBase reference
    NamedObjectBase& get(const std::string& id) 
    {
      // Find it
      typedef std::map<std::string, NamedObjectBase*>::iterator I;
      I iter = the_map.find(id);
      if (iter == the_map.end()) 
      {
	// Not found -- lookup exception
	ostringstream error_stream;
        error_stream << "NamedObjectMap::get : unknown id = " << id << endl;
        throw error_stream.str();
      }
      else 
      {
	// Found, return the reference
	return *(iter->second);
      }
    }

    //! Look something up and return a ref to the derived named object
    template<typename T>
    T& getObj(const std::string& id) 
    {
      return dynamic_cast<NamedObject<T>&>(get(id));
    }

    //! Look something up and return a ref to the derived named object
    template<typename T>
    const T& getObj(const std::string& id) const 
    {
      return dynamic_cast<NamedObject<T>&>(get(id));
    }

    //! Look something up and return a ref to actual data
    template<typename T>
    T& getData(const std::string& id) 
    {
      return dynamic_cast<NamedObject<T>&>(get(id)).getData();
    }

    //! Look something up and return a ref to actual data
    template<typename T>
    const T& getData(const std::string& id) const 
    {
      return dynamic_cast<NamedObject<T>&>(get(id)).getData();
    }

  private:
    std::map<std::string, NamedObjectBase*> the_map;
  };

}

#endif
