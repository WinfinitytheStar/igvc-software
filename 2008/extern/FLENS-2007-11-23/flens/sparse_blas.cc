/*
 *   Copyright (c) 2007, Michael Lehn
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the FLENS development group nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <cassert>
#include <flens/sparse_blas.h>
#include <iostream>

#ifdef MKL
#    ifdef MAC
#        include <Intel_MKL/mkl_spblas.h>
#    else
#        include <mkl_spblas.h>
#    endif
#endif

namespace flens {

//-- crs - compressed row storage ----------------------------------------------

void
crs_gemv(Transpose trans, int m, int n, double alpha,
         const double *a, const int *ia, const int *ja,
         const double *x, double beta, double *y)
{
    assert((beta==double(0)) || (beta==double(1)));

    // shift to index base 1
    a = a-1;
    ia = ia-1;
    ja = ja-1;
    x = x-1;
    y = y-1;

    const bool init = (beta==double(0));

    if (trans==NoTrans) {
        for (int i=1; i<=m; ++i) {
            if (init) {
                y[i] = 0;
            }
            for (int k=ia[i]; k<ia[i+1]; ++k) {
                y[i] += alpha*a[k]*x[ja[k]];
            }
        }
    } else {
        if (init) {
            for (int i=1; i<=n; ++i) {
                y[i] = 0;
            }
        }
        for (int i=1; i<=m; ++i) {
            for (int k=ia[i]; k<ia[i+1]; ++k) {
                y[ja[k]] += alpha*a[k]*x[i];
            }
        }
    }
}

void
crs_symv(StorageUpLo upLo, int m, double alpha,
         const double *a, const int *ia, const int *ja,
         const double *x, double beta, double *y)
{
    assert((beta==double(0)) || (beta==double(1)));

    // shift to index base 1
    a = a-1;
    ia = ia-1;
    ja = ja-1;
    x = x-1;
    y = y-1;

    if (beta==double(0)) {
        for (int i=1; i<=m; ++i) {
            y[i] = 0;
        }
    }

    /*
    if (upLo==Upper) {
        for (int i=1; i<=m; ++i) {
            assert(ja[ia[i]]==i);
            y[i] += alpha*a[ia[i]]*x[i];
            for (int k=ia[i]+1; k<ia[i+1]; ++k) {
                y[i] += alpha*a[k]*x[ja[k]];
                y[ja[k]] += alpha*a[k]*x[i];
            }
        }
    } else {
        for (int i=1; i<=m; ++i) {
            for (int k=ia[i]; k<ia[i+1]-1; ++k) {
                y[i] += alpha*a[k]*x[ja[k]];
                y[ja[k]] += alpha*a[k]*x[i];
            }
            assert(ja[ia[i+1]-1]==i);
            y[i] += alpha*a[ia[i+1]-1]*x[i];
        }
    }
    */
    double a_k, x_i, y_i;
    if (upLo==Upper) {
        for (int i=1; i<=m; ++i) {
            assert(ja[ia[i]]==i);
            y_i = y[i];
            x_i = x[i];
            y_i += alpha*a[ia[i]]*x_i;
            for (int k=ia[i]+1; k<ia[i+1]; ++k) {
                a_k = a[k];
                y_i += alpha*a_k*x[ja[k]];
                y[ja[k]] += alpha*a_k*x_i;
            }
            y[i] = y_i;
        }
    } else {
        for (int i=1; i<=m; ++i) {
            y_i = y[i];
            x_i = x[i];
            for (int k=ia[i]; k<ia[i+1]-1; ++k) {
                a_k = a[k];
                y_i += alpha*a_k*x[ja[k]];
                y[ja[k]] += alpha*a_k*x_i;
            }
            assert(ja[ia[i+1]-1]==i);
            y_i += alpha*a[ia[i+1]-1]*x_i;
            y[i] = y_i;
        }
    }
}

//-- csr - compressed sparse row - (the Intel variant for crs) -----------------

void
csrmv(Transpose Trans, int m, int k, double alpha, char *matdescra,
      double  *values, int *columns,  int *pointerB, int *pointerE,
      double *x, double beta, double *y)
{
    #ifdef MKL
        char trans = (Trans==NoTrans) ? 'N' : 'T';

        mkl_dcsrmv (&trans, &m, &k, &alpha, matdescra,
                    values, columns, pointerB, pointerE,
                    x, &beta, y);
    #else
        assert(0);
    #endif
}

} // namespace flens
