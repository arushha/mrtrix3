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


#include "command.h"
#include "math/matrix.h"
#include "point.h"
#include "dwi/tractography/file.h"


using namespace MR;
using namespace App;

void usage ()
{
  DESCRIPTION
  + "convert ascii track files into MRtrix format."
  + "The input ascii files should consist of 3xN matrices, corresponding to the [ X Y Z ] coordinates of the N points making up the track. All the input tracks will be included into the same output MRtrix track file.";

  ARGUMENTS
  + Argument ("input",
              "The input tracks to be included into the output track file.")
  .allow_multiple()
  .type_file ()

  + Argument ("output",
              "The output tracks file in MRtrix format").type_file();
}





void run ()
{

  DWI::Tractography::Properties properties;

  DWI::Tractography::Writer<> writer (argument.back(), properties);

  for (size_t n = 0; n < argument.size()-1; n++) {
    Math::Matrix<float> M;
    try {
      M.load (argument[n]);
      if (M.columns() != 3)
        throw Exception ("file \"" + argument[n] + "\" does not contain 3 columns - ignored");

      DWI::Tractography::Streamline<float> tck (M.rows());
      for (size_t i = 0; i < M.rows(); i++) {
        tck[i].set (M (i,0), M (i,1), M (i,2));
      }
      writer (tck);
      writer.total_count++;
    }
    catch (Exception) { }
  }

}
