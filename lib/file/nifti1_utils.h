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


#ifndef __file_nifti1_utils_h__
#define __file_nifti1_utils_h__

#include "file/nifti1.h"

namespace MR
{
  namespace Image
  {
    class Header;
  }
  namespace File
  {
    namespace NIfTI
    {

      void check (Image::Header& H, bool single_file);
      //! \todo add straight Analyse support
      size_t read (Image::Header& H, const nifti_1_header& NH);
      void check (Image::Header& H, bool single_file);
      //! \todo need to double-check new transform handling code
      void write (nifti_1_header& NH, const Image::Header& H, bool single_file);

    }
  }
}

#endif

