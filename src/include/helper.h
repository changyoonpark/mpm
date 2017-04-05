#ifndef __MYMATH
#define __MYMATH

#include <ostream>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>


template<typename Out>
void split(const std::string &s, char delim, Out result);
std::vector<std::string> split(const std::string &s, char delim);


namespace myMath {

/**
 * Defines 3D vectors.
 */

struct int3{
  public:
    int nx,ny,nz;
    int3(int a, int b, int c) : nx( a ), ny( b ), nz( c ) { }

    inline int3 operator+( const int3& i ) const {
      return int3( nx + i.nx, ny + i.ny, nz + i.nz );
    }    
};

class Vector3D {
 public:

  // components
  double x, y, z;

  /**
   * Constructor.
   * Initializes tp vector (0,0,0).
   */
  Vector3D() : x( 0.0 ), y( 0.0 ), z( 0.0 ) { }

  /**
   * Constructor.
   * Initializes to vector (x,y,z).
   */
  Vector3D( double x, double y, double z) : x( x ), y( y ), z( z ) { }

  /**
   * Constructor.
   * Initializes to vector (c,c,c)
   */
  Vector3D( double c ) : x( c ), y( c ), z( c ) { }

  /**
   * Constructor.
   * Initializes from existing vector
   */
  Vector3D( const Vector3D& v ) : x( v.x ), y( v.y ), z( v.z ) { }

  // returns reference to the specified component (0-based indexing: x, y, z)


  inline double& operator[] ( const int& index ) {
    return ( &x )[ index ];
  }

  // returns const reference to the specified component (0-based indexing: x, y, z)
  inline const double& operator[] ( const int& index ) const {
    return ( &x )[ index ];
  }

  inline bool operator==( const Vector3D& v) const {
    return v.x == x && v.y == y && v.z == z;
  }

  // negation
  inline Vector3D operator-( void ) const {
    return Vector3D( -x, -y, -z );
  }

  // addition
  inline Vector3D operator+( const Vector3D& v ) const {
    return Vector3D( x + v.x, y + v.y, z + v.z );
  }

  // subtraction
  inline Vector3D operator-( const Vector3D& v ) const {
    return Vector3D( x - v.x, y - v.y, z - v.z );
  }

  // right scalar multiplication
  inline Vector3D operator*( const double& c ) const {
    return Vector3D( x * c, y * c, z * c );
  }

  // scalar division
  inline Vector3D operator/( const double& c ) const {
    const double rc = 1.0/c;
    return Vector3D( rc * x, rc * y, rc * z );
  }

  // addition / assignment
  inline void operator+=( const Vector3D& v ) {
    x += v.x; y += v.y; z += v.z;
  }

  // subtraction / assignment
  inline void operator-=( const Vector3D& v ) {
    x -= v.x; y -= v.y; z -= v.z;
  }

  // scalar multiplication / assignment
  inline void operator*=( const double& c ) {
    x *= c; y *= c; z *= c;
  }

  // scalar division / assignment
  inline void operator/=( const double& c ) {
    (*this) *= ( 1./c );
  }

  /**
   * Returns Euclidean length.
   */
  inline double norm( void ) const {
    return sqrt( x*x + y*y + z*z );
  }

  /**
   * Returns Euclidean length squared.
   */
  inline double norm2( void ) const {
    return x*x + y*y + z*z;
  }

  /**
   * Returns unit vector.
   */
  inline Vector3D unit( void ) const {
    double rNorm = 1. / sqrt( x*x + y*y + z*z );
    return Vector3D( rNorm*x, rNorm*y, rNorm*z );
  }

  /**
   * Divides by Euclidean length.
   */
  inline void normalize( void ) {
    (*this) /= norm();
  }

}; // class Vector3D

// left scalar multiplication
inline Vector3D operator* ( const double& c, const Vector3D& v ) {
  return Vector3D( c * v.x, c * v.y, c * v.z );
}

// dot product (a.k.a. inner or scalar product)
inline double dot( const Vector3D& u, const Vector3D& v ) {
  return u.x*v.x + u.y*v.y + u.z*v.z ;
}

// cross product
inline Vector3D cross( const Vector3D& u, const Vector3D& v ) {
  return Vector3D( u.y*v.z - u.z*v.y,
                   u.z*v.x - u.x*v.z,
                   u.x*v.y - u.y*v.x );
}


class Matrix3x3 {

  public:

  // The default constructor.
  Matrix3x3(void) { }

  Matrix3x3(const Vector3D& row1, const Vector3D& row2, const Vector3D& row3){

      (*this)(0,0) = row1.x;
      (*this)(0,1) = row1.y;
      (*this)(0,2) = row1.z;

      (*this)(1,0) = row2.x;
      (*this)(1,1) = row2.y;
      (*this)(1,2) = row2.z;

      (*this)(2,0) = row3.x;
      (*this)(2,1) = row3.y;
      (*this)(2,2) = row3.z;

  }
  // Constructor for row major form data.
  // Transposes to the internal column major form.
  // REQUIRES: data should be of size 9 for a 3 by 3 matrix..
  Matrix3x3(double * data)
  {
    for( int i = 0; i < 3; i++ ) {
      for( int j = 0; j < 3; j++ ) {
          // Transpostion happens within the () query.
          (*this)(i,j) = data[i*3 + j];
      }
    }
  }

  //Clamps the diagonals and returns the residual
  void clamp(double l1, double l2);
  
  /**
   * Sets all elements to val.
   */
  void zero(double val = 0.0 );

  /**
   * Returns the determinant of A.
   */
  double det( void ) const;

  /**
   * Returns the Frobenius norm of A.
   */
  double norm( void ) const;

  double norm2( void ) const;
  /**
   * Returns the 3x3 identity matrix.
   */
  static Matrix3x3 identity( void );

  void singularValueDecompose( Matrix3x3& U, Matrix3x3& S, Matrix3x3& V);
  /**
   * Returns a matrix representing the (left) cross product with u.
   */
  static Matrix3x3 crossProduct( const Vector3D& u );

  /**
   * Returns the ith column.
   */
        Vector3D& column( int i );
  const Vector3D& column( int i ) const;

  /**
   * Returns the transpose of A.
   */
  Matrix3x3 T( void ) const;

  /**
   * Returns the inverse of A.
   */
  Matrix3x3 inv( void ) const;

  // accesses element (i,j) of A using 0-based indexing
  float getAsFloat( int i, int j );
        double& operator()( int i, int j );
  const double& operator()( int i, int j ) const;

  // accesses the ith column of A
        Vector3D& operator[]( int i );
  const Vector3D& operator[]( int i ) const;

  // increments by B
  void operator+=( const Matrix3x3& B );

  // returns -A
  Matrix3x3 operator-( void ) const;

  // returns A+B
  Matrix3x3 operator+( const Matrix3x3& B ) const;

  // returns A-B
  Matrix3x3 operator-( const Matrix3x3& B ) const;

  // returns c*A
  Matrix3x3 operator*( double c ) const;

  // returns A*B
  Matrix3x3 operator*( const Matrix3x3& B ) const;

  // returns A*x
  Vector3D operator*( const Vector3D& x ) const;

  // divides each element by x
  void operator/=( double x );

  protected:

  // column vectors
  Vector3D entries[3];

}; // class Matrix3x3

// returns the outer product of u and v
Matrix3x3 outer( const Vector3D& u, const Vector3D& v );

// returns c*A
Matrix3x3 operator*( double c, const Matrix3x3& A );

// prints entries
std::ostream& operator<<( std::ostream& os, const Matrix3x3& A );

// prints components
std::ostream& operator<<( std::ostream& os, const Vector3D& v );


} // namespace CGL





#endif // MYMATH
