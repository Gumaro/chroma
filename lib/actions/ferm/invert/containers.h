// -*- C++ -*-
// $Id: containers.h,v 1.2 2007-10-04 21:39:04 edwards Exp $

#ifndef _INV_STATHO_CONTAINERS__H
#define _INV_STATHO_CONTAINERS__H

#include "chromabase.h"
#include "handle.h"

namespace LinAlg
{
  //! Hold vectors
  /*! \ingroup invert */
  template<class T> class Vectors
  {
  public:
    multi1d<T> vec ;
    int N ; // number of active vectors size of vec must be larger or equal to N

    Vectors():N(0){} 
    Vectors(const multi1d<T>& v):vec(v),N(v.size()){} 
    Vectors(int size):N(0){vec.resize(size) ; } 
    
    ~Vectors(){}
    
    void AddVector(const T& v,const Subset& s){
      if(N<vec.size()){
	vec[N][s] = v ;
	N++ ;
      }
    }

    void NormalizeAndAddVector(const T& v,const Double& inorm, 
			       const Subset& s){
      if(N<vec.size()){// inorm is the inverse of the norm
	vec[N][s] = v  ;
	vec[N][s] *= inorm  ;
	N++ ;
      }
    }
    void AddOrReplaceVector(const T& v,const Subset& s){
      if(N<vec.size()){
	vec[N][s] = v ;
	N++ ;
      }
      else{// replace the last vector
	vec[N-1] = v ;
      }
    }

    // This will only add as many vectors as they fit
    void AddVectors(multi1d<T>& v,const Subset& s){
      for(int i(0);i<v.size();i++)
	AddVector(v[i],s) ;
    }

    
    void ResetN(int N) ;
    int size() const { return vec.size();}
    int Nvecs() const { return N; } 
    T& operator[](int i){ return vec[i];}
  };


  //! Holds eigenvalues and eigenvectors
  /*! \ingroup invert */
  template<class T> class RitzPairs
  {
  public:
    Vectors<Double> eval;
    Vectors<T>      evec;
    int Neig;

    RitzPairs(int N) {init(N);}

    void init(int N)
      {
	eval.resize(N);
	evec.resize(N);
	Neig = 0;
      }

    void AddVector(const T& v,const Subset& s){
      if(N<vec.size()){
	vec[N][s] = v ;
	N++ ;
      }
    }

    // This will only add as many vectors as they fit
    void AddVectors(multi1d<T>& v,const Subset& s){
      for(int i(0);i<v.size();i++)
	AddVector(v[i],s) ;
    }
  };

  //! This is a square matrix
  /*! \ingroup invert */
  template<class T> class Matrix
  {
  public:
    multi2d<T> mat ;
    int N ;  // active size 

    Matrix():N(0){} 
    Matrix(const multi2d<T>& v):mat(v),N(v.size1())
    {
      if(v.size1() != v.size2())
	QDPIO::cerr<<"WARNING!!! Matrix should be square!! CHECK YOUR CODE!\n";
    }

    Matrix(int size):N(0){mat.resize(size,size) ; } 
    
    ~Matrix(){}
    
    void ResetN(int N) ;
    int size() const { return mat.size1();}
    int ld() const { return mat.size1();}
    int Nmat() const { return N; } 
    T& operator()(int i,int j){ return mat(i,j);}
  };
  
}
 #endif 
