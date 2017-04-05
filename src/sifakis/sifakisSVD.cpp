//#####################################################################
// Copyright (c) 2009-2011, Eftychios Sifakis.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//   * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or
//     other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//#####################################################################

#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include "sifakisSVD.h"

#define PRINT_DEBUGGING_OUTPUT
#define USE_SCALAR_IMPLEMENTATION

#define COMPUTE_V_AS_MATRIX
#define COMPUTE_U_AS_MATRIX

#include "Singular_Value_Decomposition_Preamble.hpp"

void svd(float A11, float A12, float A13,
         float A21, float A22, float A23,
         float A31, float A32, float A33,

         float& SIG11, float& SIG22, float& SIG33,

         float& U11, float& U12, float& U13,
         float& U21, float& U22, float& U23,
         float& U31, float& U32, float& U33,
        
         float& V11, float& V12, float& V13,
         float& V21, float& V22, float& V23,
         float& V31, float& V32, float& V33){
        
#include "Singular_Value_Decomposition_Kernel_Declarations.hpp"

    ENABLE_SCALAR_IMPLEMENTATION(Sa11.f=A11;) 
    ENABLE_SCALAR_IMPLEMENTATION(Sa21.f=A21;) 
    ENABLE_SCALAR_IMPLEMENTATION(Sa31.f=A31;) 
    ENABLE_SCALAR_IMPLEMENTATION(Sa12.f=A12;) 
    ENABLE_SCALAR_IMPLEMENTATION(Sa22.f=A22;) 
    ENABLE_SCALAR_IMPLEMENTATION(Sa32.f=A32;) 
    ENABLE_SCALAR_IMPLEMENTATION(Sa13.f=A13;) 
    ENABLE_SCALAR_IMPLEMENTATION(Sa23.f=A23;) 
    ENABLE_SCALAR_IMPLEMENTATION(Sa33.f=A33;) 

#include "Singular_Value_Decomposition_Main_Kernel_Body.hpp"    

}

// int main ( void ){
//     return 0;
// }
//#####################################################################
