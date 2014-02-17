/*******************************************************************************
    Copyright (C) 2014 Brain Research Institute, Melbourne, Australia
    
    Permission is hereby granted under the Patent Licence Agreement between
    the BRI and Siemens AG from July 3rd, 2012, to Siemens AG obtaining a
    copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to possess, use, develop, manufacture,
    import, offer for sale, market, sell, lease or otherwise distribute
    Products, and to permit persons to whom the Software is furnished to do
    so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*******************************************************************************/


/* Author:  G. Jungman */

/* modified 17/05/2009 J-Donald Tournier (d.tournier@brain.org.au)
   to provide single-precision equivalent functions */

#include <stdlib.h>
#include <string.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector_float.h>
#include <gsl/gsl_matrix_float.h>
#include <gsl/gsl_permute_vector_float.h>
#include <gsl/gsl_blas.h>

#include <gsl/gsl_linalg.h>

#include "math/LU.h"

#define REAL float

/* Factorise a general N x N matrix A into,
 *
 *   P A = L U
 *
 * where P is a permutation matrix, L is unit lower triangular and U
 * is upper triangular.
 *
 * L is stored in the strict lower triangular part of the input
 * matrix. The diagonal elements of L are unity and are not stored.
 *
 * U is stored in the diagonal and upper triangular part of the
 * input matrix.
 *
 * P is stored in the permutation p. Column j of P is column k of the
 * identity matrix, where k = permutation->data[j]
 *
 * signum gives the sign of the permutation, (-1)^n, where n is the
 * number of interchanges in the permutation.
 *
 * See Golub & Van Loan, Matrix Computations, Algorithm 3.4.1 (Gauss
 * Elimination with Partial Pivoting).
 */
namespace MR
{
  namespace Math
  {
    namespace LU
    {

      int gsl_linalg_LU_decomp (gsl_matrix_float* A, gsl_permutation* p, int* signum)
      {
        if (A->size1 != A->size2) {
          GSL_ERROR ("LU decomposition requires square matrix", GSL_ENOTSQR);
        }
        else if (p->size != A->size1) {
          GSL_ERROR ("permutation length must match matrix size", GSL_EBADLEN);
        }
        else {
          const size_t N = A->size1;
          size_t i, j, k;

          *signum = 1;
          gsl_permutation_init (p);

          for (j = 0; j < N - 1; j++) {
            /* Find maximum in the j-th column */

            REAL ajj, max = fabsf (gsl_matrix_float_get (A, j, j));
            size_t i_pivot = j;

            for (i = j + 1; i < N; i++) {
              REAL aij = fabsf (gsl_matrix_float_get (A, i, j));

              if (aij > max) {
                max = aij;
                i_pivot = i;
              }
            }

            if (i_pivot != j) {
              gsl_matrix_float_swap_rows (A, j, i_pivot);
              gsl_permutation_swap (p, j, i_pivot);
              *signum = - (*signum);
            }

            ajj = gsl_matrix_float_get (A, j, j);

            if (ajj != 0.0) {
              for (i = j + 1; i < N; i++) {
                REAL aij = gsl_matrix_float_get (A, i, j) / ajj;
                gsl_matrix_float_set (A, i, j, aij);

                for (k = j + 1; k < N; k++) {
                  REAL aik = gsl_matrix_float_get (A, i, k);
                  REAL ajk = gsl_matrix_float_get (A, j, k);
                  gsl_matrix_float_set (A, i, k, aik - aij * ajk);
                }
              }
            }
          }

          return GSL_SUCCESS;
        }
      }

      int gsl_linalg_LU_solve (const gsl_matrix_float* LU, const gsl_permutation* p, const gsl_vector_float* b, gsl_vector_float* x)
      {
        if (LU->size1 != LU->size2) {
          GSL_ERROR ("LU matrix must be square", GSL_ENOTSQR);
        }
        else if (LU->size1 != p->size) {
          GSL_ERROR ("permutation length must match matrix size", GSL_EBADLEN);
        }
        else if (LU->size1 != b->size) {
          GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
        }
        else if (LU->size2 != x->size) {
          GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
        }
        else {
          /* Copy x <- b */

          gsl_vector_float_memcpy (x, b);

          /* Solve for x */

          gsl_linalg_LU_svx (LU, p, x);

          return GSL_SUCCESS;
        }
      }


      int gsl_linalg_LU_svx (const gsl_matrix_float* LU, const gsl_permutation* p, gsl_vector_float* x)
      {
        if (LU->size1 != LU->size2) {
          GSL_ERROR ("LU matrix must be square", GSL_ENOTSQR);
        }
        else if (LU->size1 != p->size) {
          GSL_ERROR ("permutation length must match matrix size", GSL_EBADLEN);
        }
        else if (LU->size1 != x->size) {
          GSL_ERROR ("matrix size must match solution/rhs size", GSL_EBADLEN);
        }
        else {
          /* Apply permutation to RHS */

          gsl_permute_vector_float (p, x);

          /* Solve for c using forward-substitution, L c = P b */

          gsl_blas_strsv (CblasLower, CblasNoTrans, CblasUnit, LU, x);

          /* Perform back-substitution, U x = c */

          gsl_blas_strsv (CblasUpper, CblasNoTrans, CblasNonUnit, LU, x);

          return GSL_SUCCESS;
        }
      }


      int gsl_linalg_LU_refine (const gsl_matrix_float* A, const gsl_matrix_float* LU, const gsl_permutation* p, const gsl_vector_float* b, gsl_vector_float* x, gsl_vector_float* residual)
      {
        if (A->size1 != A->size2) {
          GSL_ERROR ("matrix a must be square", GSL_ENOTSQR);
        }
        if (LU->size1 != LU->size2) {
          GSL_ERROR ("LU matrix must be square", GSL_ENOTSQR);
        }
        else if (A->size1 != LU->size2) {
          GSL_ERROR ("LU matrix must be decomposition of a", GSL_ENOTSQR);
        }
        else if (LU->size1 != p->size) {
          GSL_ERROR ("permutation length must match matrix size", GSL_EBADLEN);
        }
        else if (LU->size1 != b->size) {
          GSL_ERROR ("matrix size must match b size", GSL_EBADLEN);
        }
        else if (LU->size1 != x->size) {
          GSL_ERROR ("matrix size must match solution size", GSL_EBADLEN);
        }
        else {
          /* Compute residual, residual = (A * x  - b) */

          gsl_vector_float_memcpy (residual, b);
          gsl_blas_sgemv (CblasNoTrans, 1.0, A, x, -1.0, residual);

          /* Find correction, delta = - (A^-1) * residual, and apply it */

          gsl_linalg_LU_svx (LU, p, residual);
          gsl_blas_saxpy (-1.0, residual, x);

          return GSL_SUCCESS;
        }
      }

      int gsl_linalg_LU_invert (const gsl_matrix_float* LU, const gsl_permutation* p, gsl_matrix_float* inverse)
      {
        size_t i, n = LU->size1;

        int status = GSL_SUCCESS;

        gsl_matrix_float_set_identity (inverse);

        for (i = 0; i < n; i++) {
          gsl_vector_float_view c = gsl_matrix_float_column (inverse, i);
          int status_i = gsl_linalg_LU_svx (LU, p, & (c.vector));

          if (status_i)
            status = status_i;
        }

        return status;
      }

      double gsl_linalg_LU_det (gsl_matrix_float* LU, int signum)
      {
        size_t i, n = LU->size1;

        double det = (double) signum;

        for (i = 0; i < n; i++) {
          det *= gsl_matrix_float_get (LU, i, i);
        }

        return det;
      }


      double gsl_linalg_LU_lndet (gsl_matrix_float* LU)
      {
        size_t i, n = LU->size1;

        double lndet = 0.0;

        for (i = 0; i < n; i++) {
          lndet += log (fabsf (gsl_matrix_float_get (LU, i, i)));
        }

        return lndet;
      }


      int gsl_linalg_LU_sgndet (gsl_matrix_float* LU, int signum)
      {
        size_t i, n = LU->size1;

        int s = signum;

        for (i = 0; i < n; i++) {
          double u = gsl_matrix_float_get (LU, i, i);

          if (u < 0) {
            s *= -1;
          }
          else if (u == 0) {
            s = 0;
            break;
          }
        }

        return s;
      }
    }
  }
}

