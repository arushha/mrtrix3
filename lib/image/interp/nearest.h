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


#ifndef __image_interp_nearest_h__
#define __image_interp_nearest_h__

#include "image/transform.h"
#include "datatype.h"

namespace MR
{
  namespace Image
  {
    namespace Interp
    {

      //! \addtogroup interp
      // @{

      //! This class provides access to the voxel intensities of a data set, using nearest-neighbour interpolation.
      /*! Interpolation is only performed along the first 3 (spatial) axes.
       * The (integer) position along the remaining axes should be set using the
       * template DataSet class.
       * The spatial coordinates can be set using the functions voxel(), image(),
       * and scanner().
       * For example:
       * \code
       * Image::Voxel<float> voxel (image);
       *
       * // create an Interp::Nearest object using voxel as the parent data set:
       * DataSet::Interp::Nearest<Image::Voxel<float> > interp (voxel);
       *
       * // set the scanner-space position to [ 10.2 3.59 54.1 ]:
       * interp.scanner (10.2, 3.59, 54.1);
       *
       * // get the value at this position:
       * float value = interp.value();
       * \endcode
       *
       * The template \a voxel class must be usable with this type of syntax:
       * \code
       * int xdim = voxel.dim(0);    // return the dimension
       * int ydim = voxel.dim(1);    // along the x, y & z dimensions
       * int zdim = voxel.dim(2);
       * float v[] = { voxel.vox(0), voxel.vox(1), voxel.vox(2) };  // return voxel dimensions
       * voxel[0] = 0;               // these lines are used to
       * voxel[1]--;                 // set the current position
       * voxel[2]++;                 // within the data set
       * float f = voxel.value();
       * Math::Transform<float> M = voxel.transform(); // a valid 4x4 transformation matrix
       * \endcode
       */

      template <class VoxelType>
        class Nearest : public VoxelType, public Transform
      {
        public:
          typedef typename VoxelType::value_type value_type;

          using Transform::set_to_nearest;
          using Transform::image2voxel;
          using Transform::scanner2voxel;
          using Transform::operator!;
          using Transform::out_of_bounds;
          using Transform::bounds;

          //! construct an Nearest object to obtain interpolated values using the
          // parent DataSet class
          Nearest (const VoxelType& parent, value_type value_when_out_of_bounds = DataType::default_out_of_bounds_value<value_type>()) :
            VoxelType (parent),
            Transform (parent),
            out_of_bounds_value (value_when_out_of_bounds) { }

          //! Set the current position to <b>voxel space</b> position \a pos
          /*! This will set the position from which the image intensity values will
           * be interpolated, assuming that \a pos provides the position as a
           * (floating-point) voxel coordinate within the dataset. */
          bool voxel (const Point<float>& pos) {
            set_to_nearest (pos);
            if (out_of_bounds)
              return true;

            (*this)[0] = Math::round<ssize_t> (pos[0]);
            (*this)[1] = Math::round<ssize_t> (pos[1]);
            (*this)[2] = Math::round<ssize_t> (pos[2]);
            return false;
          }


          //! Set the current position to <b>image space</b> position \a pos
          /*! This will set the position from which the image intensity values will
           * be interpolated, assuming that \a pos provides the position as a
           * coordinate relative to the axes of the dataset, in units of
           * millimeters. The origin is taken to be the centre of the voxel at [
           * 0 0 0 ]. */
          bool image (const Point<float>& pos) {
            return voxel (image2voxel (pos));
          }
          //! Set the current position to the <b>scanner space</b> position \a pos
          /*! This will set the position from which the image intensity values will
           * be interpolated, assuming that \a pos provides the position as a
           * scanner space coordinate, in units of millimeters. */
          bool scanner (const Point<float>& pos) {
            return voxel (scanner2voxel (pos));
          }

          value_type value () const {
            if (out_of_bounds) {
              return out_of_bounds_value;
            }

            return VoxelType::value();
          }

          const value_type out_of_bounds_value;
      };

      //! @}

    }
  }
}

#endif


