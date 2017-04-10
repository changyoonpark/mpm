#include "include/helper.h"
#include "sifakis/sifakisSVD.h"


template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

namespace myMath{
    std::ostream& operator<<( std::ostream& os, const Vector3D& v ) {
        os << "(" << v.x << "," << v.y << "," << v.z << ")";
        return os;
    }


 void Matrix3x3::clamp( double l1, double l2 ){

    // const Matrix3x3& A( *this );

    for(int i = 0; i < 3; i ++){
      if(entries[i][i] > l2){
        entries[i][i] = l2;
      } else if(entries[i][i] < l1){
        entries[i][i] = l1;      
      }
    }

  }

  void Matrix3x3::tensorProductComponent(Matrix3x3& B, int i, int j, int k, int l){
    const Matrix3x3& A( *this );
    return A(i,j) * B(k,l);
  }

  void Matrix3x3::singularValueDecompose( Matrix3x3& U, Matrix3x3& S, Matrix3x3& V){

    const Matrix3x3& A( *this );
    // float a11 = 1.0, a12 = 0.0, a13 = 0.0;
    // float a21 = 0.0, a22 = 1.0, a23 = 0.0;
    // float a31 = 0.0, a32 = 0.0, a33 = 1.0;
    
  //   -0.558253, -0.046168, -0.505735],[-0.411397, 0.036585, 0.199707
  //  ...: ],[ 0.285389, -0.313789, 0.200189]

    // float a11 = -0.558253, a12 = -0.046168, a13 = -0.505735;
    // float a21 = -0.411397, a22 = 0.036585, a23 = 0.199707;
    // float a31 = 0.285389, a32 = -0.313789, a33 = 0.200189;

    float u11, u12, u13;
    float u21, u22, u23;
    float u31, u32, u33;

    float s11, s22, s33;

    float v11, v12, v13;
    float v21, v22, v23;
    float v31, v32, v33;

    svd(
        
        this->getAsFloat(0,0),this->getAsFloat(0,1),this->getAsFloat(0,2),
        this->getAsFloat(1,0),this->getAsFloat(1,1),this->getAsFloat(1,2),
        this->getAsFloat(2,0),this->getAsFloat(2,1),this->getAsFloat(2,2),
        // a11, a12, a13,
        // a21, a22, a23,
        // a31, a32, a33,
        
        s11, s22, s33,

        u11, u12, u13,
        u21, u22, u23,
        u31, u32, u33,

        v11, v12, v13,
        v21, v22, v23,
        v31, v32, v33

    );



    U(0,0) = (double)u11; U(0,1) = (double)u12; U(0,2) = (double)u13;
    U(1,0) = (double)u21; U(1,1) = (double)u22; U(1,2) = (double)u23;
    U(2,0) = (double)u31; U(2,1) = (double)u32; U(2,2) = (double)u33;

    S(0,0) = (double)s11;
    S(1,1) = (double)s22;
    S(2,2) = (double)s33;

    V(0,0) = (double)v11; V(0,1) = (double)v12; V(0,2) = (double)v13;
    V(1,0) = (double)v21; V(1,1) = (double)v22; V(1,2) = (double)v23;
    V(2,0) = (double)v31; V(2,1) = (double)v32; V(2,2) = (double)v33;

  }



  float Matrix3x3::getAsFloat( int i, int j ) {
    return (float) entries[j][i];
  }

  double& Matrix3x3::operator()( int i, int j ) {
    return entries[j][i];
  }

  const double& Matrix3x3::operator()( int i, int j ) const {
    return entries[j][i];
  }

  Vector3D& Matrix3x3::operator[]( int j ) {
      return entries[j];
  }

  const Vector3D& Matrix3x3::operator[]( int j ) const {
    return entries[j];
  }

  void Matrix3x3::zero( double val ) {
    // sets all elements to val
    entries[0] = entries[1] = entries[2] = Vector3D( val, val, val );
  }

  double Matrix3x3::det( void ) const {
    const Matrix3x3& A( *this );

    return -A(0,2)*A(1,1)*A(2,0) + A(0,1)*A(1,2)*A(2,0) +
            A(0,2)*A(1,0)*A(2,1) - A(0,0)*A(1,2)*A(2,1) -
            A(0,1)*A(1,0)*A(2,2) + A(0,0)*A(1,1)*A(2,2) ;
  }

  double Matrix3x3::norm( void ) const {
    return sqrt( entries[0].norm2() +
                 entries[1].norm2() +
                 entries[2].norm2() );
  }

  double Matrix3x3::norm2( void ) const {
    return entries[0].norm2() +
           entries[1].norm2() +
           entries[2].norm2();
  }


  Matrix3x3 Matrix3x3::operator-( void ) const {

   // returns -A
    const Matrix3x3& A( *this );
    Matrix3x3 B;

    B(0,0) = -A(0,0); B(0,1) = -A(0,1); B(0,2) = -A(0,2);
    B(1,0) = -A(1,0); B(1,1) = -A(1,1); B(1,2) = -A(1,2);
    B(2,0) = -A(2,0); B(2,1) = -A(2,1); B(2,2) = -A(2,2);

    return B;
  }

  void Matrix3x3::operator+=( const Matrix3x3& B ) {

    Matrix3x3& A( *this );
    double* Aij = (double*) &A;
    const double* Bij = (const double*) &B;

    *Aij++ += *Bij++;
    *Aij++ += *Bij++;
    *Aij++ += *Bij++;
    *Aij++ += *Bij++;
    *Aij++ += *Bij++;
    *Aij++ += *Bij++;
    *Aij++ += *Bij++;
    *Aij++ += *Bij++;
    *Aij++ += *Bij++;
  }

  Matrix3x3 Matrix3x3::operator+( const Matrix3x3& B ) const {
    const Matrix3x3& A( *this );
    Matrix3x3 C;

    for( int i = 0; i < 3; i++ )
    for( int j = 0; j < 3; j++ )
    {
       C(i,j) = A(i,j) + B(i,j);
    }

    return C;
  }

  Matrix3x3 Matrix3x3::operator-( const Matrix3x3& B ) const {
    const Matrix3x3& A( *this );
    Matrix3x3 C;

    for( int i = 0; i < 3; i++ )
    for( int j = 0; j < 3; j++ )
    {
       C(i,j) = A(i,j) - B(i,j);
    }

    return C;
  }

  Matrix3x3 Matrix3x3::operator*( double c ) const {
    const Matrix3x3& A( *this );
    Matrix3x3 B;

    for( int i = 0; i < 3; i++ )
    for( int j = 0; j < 3; j++ )
    {
       B(i,j) = c*A(i,j);
    }

    return B;
  }

  Matrix3x3 operator*( double c, const Matrix3x3& A ) {

    Matrix3x3 cA;
    const double* Aij = (const double*) &A;
    double* cAij = (double*) &cA;

    *cAij++ = c * (*Aij++);
    *cAij++ = c * (*Aij++);
    *cAij++ = c * (*Aij++);
    *cAij++ = c * (*Aij++);
    *cAij++ = c * (*Aij++);
    *cAij++ = c * (*Aij++);
    *cAij++ = c * (*Aij++);
    *cAij++ = c * (*Aij++);
    *cAij++ = c * (*Aij++);

    return cA;
  }

  Matrix3x3 Matrix3x3::operator*( const Matrix3x3& B ) const {
    const Matrix3x3& A( *this );
    Matrix3x3 C;

    for( int i = 0; i < 3; i++ )
    for( int j = 0; j < 3; j++ )
    {
       C(i,j) = 0.;

       for( int k = 0; k < 3; k++ )
       {
          C(i,j) += A(i,k)*B(k,j);
       }
    }

    return C;
  }

  Vector3D Matrix3x3::operator*( const Vector3D& x ) const {
    return x[0]*entries[0] +
           x[1]*entries[1] +
           x[2]*entries[2] ;
  }

  Matrix3x3 Matrix3x3::T( void ) const {
    const Matrix3x3& A( *this );
    Matrix3x3 B;

    for( int i = 0; i < 3; i++ )
    for( int j = 0; j < 3; j++ )
    {
       B(i,j) = A(j,i);
    }

    return B;
  }

  Matrix3x3 Matrix3x3::inv( void ) const {
    const Matrix3x3& A( *this );
    Matrix3x3 B;

    B(0,0) = -A(1,2)*A(2,1) + A(1,1)*A(2,2); B(0,1) =  A(0,2)*A(2,1) - A(0,1)*A(2,2); B(0,2) = -A(0,2)*A(1,1) + A(0,1)*A(1,2);
    B(1,0) =  A(1,2)*A(2,0) - A(1,0)*A(2,2); B(1,1) = -A(0,2)*A(2,0) + A(0,0)*A(2,2); B(1,2) =  A(0,2)*A(1,0) - A(0,0)*A(1,2);
    B(2,0) = -A(1,1)*A(2,0) + A(1,0)*A(2,1); B(2,1) =  A(0,1)*A(2,0) - A(0,0)*A(2,1); B(2,2) = -A(0,1)*A(1,0) + A(0,0)*A(1,1);

    B /= det();

    return B;
  }

  void Matrix3x3::operator/=( double x ) {
    Matrix3x3& A( *this );
    double rx = 1./x;

    for( int i = 0; i < 3; i++ )
    for( int j = 0; j < 3; j++ )
    {
       A( i, j ) *= rx;
    }
  }

  Matrix3x3 Matrix3x3::identity( void ) {
    Matrix3x3 B;

    B(0,0) = 1.; B(0,1) = 0.; B(0,2) = 0.;
    B(1,0) = 0.; B(1,1) = 1.; B(1,2) = 0.;
    B(2,0) = 0.; B(2,1) = 0.; B(2,2) = 1.;

    return B;
  }

  Matrix3x3 Matrix3x3::crossProduct( const Vector3D& u ) {
    Matrix3x3 B;

    B(0,0) =   0.;  B(0,1) = -u.z;  B(0,2) =  u.y;
    B(1,0) =  u.z;  B(1,1) =   0.;  B(1,2) = -u.x;
    B(2,0) = -u.y;  B(2,1) =  u.x;  B(2,2) =   0.;

    return B;
  }

  Matrix3x3 outer( const Vector3D& u, const Vector3D& v ) {
    Matrix3x3 B;
    double* Bij = (double*) &B;

    *Bij++ = u.x*v.x;
    *Bij++ = u.y*v.x;
    *Bij++ = u.z*v.x;
    *Bij++ = u.x*v.y;
    *Bij++ = u.y*v.y;
    *Bij++ = u.z*v.y;
    *Bij++ = u.x*v.z;
    *Bij++ = u.y*v.z;
    *Bij++ = u.z*v.z;

    return B;
  }

  std::ostream& operator<<( std::ostream& os, const Matrix3x3& A ) {
    for( int i = 0; i < 3; i++ )
    {
       os << "[ ";

       for( int j = 0; j < 3; j++ )
       {
          os << A(i,j) << " ";
       }

       os << "]" << std::endl;
    }

    return os;
  }

  Vector3D& Matrix3x3::column( int i ) {
    return entries[i];
  }

  const Vector3D& Matrix3x3::column( int i ) const {
    return entries[i];
  }

}