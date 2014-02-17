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


#ifndef __mrtrix_exception_h__
#define __mrtrix_exception_h__

#include <cerrno>
#include <string>
#include <vector>

#include "types.h"

namespace MR
{
  namespace App 
  {
    extern int log_level;
    extern std::string NAME;
  }

  //! print primary output to stdout as-is. 
  /*! This function is intended for cases where the command's primary output is text, not
   * image data, etc. It is \e not designed for error or status reports: it
   * prints to stdout, whereas all reporting functions print to stderr. This is
   * to allow the output of the command to be used directly in text processing
   * pipeline or redirected to file. 
   * \note the use of stdout is normally reserved for piping data files (or at
   * least their filenames) between MRtrix commands. This function should
   * therefore never be used in commands that produce output images, as the two
   * different types of output may then interfere and cause unexpected issues. */
  extern void (*print) (const std::string& msg);




  //! display error, warning, debug, etc. message to user 
  /*! types are: 0: error; 1: warning; 2: additional information; 3:
   * debugging information; anything else: none. */
  extern void (*report_to_user_func) (const std::string& msg, int type);

#define CONSOLE(msg) if (App::log_level >= 1) report_to_user_func (msg, -1)
#define FAIL(msg) if (App::log_level >= 0) report_to_user_func (msg, 0)
#define WARN(msg) if (App::log_level >= 1) report_to_user_func (msg, 1)
#define INFO(msg) if (App::log_level >= 2) report_to_user_func (msg, 2)
#define DEBUG(msg) if (App::log_level >= 3) report_to_user_func (msg, 3)




  class Exception
  {
    public:
      Exception (const std::string& msg) {
        description.push_back (msg);
      }
      Exception (const Exception& previous_exception, const std::string& msg) :
        description (previous_exception.description) {
        description.push_back (msg);
      }

      void display (int log_level = 0) const {
        display_func (*this, log_level);
      }

      size_t num () const {
        return description.size();
      }
      const std::string& operator[] (size_t n) const {
        return description[n];
      }

      static void (*display_func) (const Exception& E, int log_level);

      std::vector<std::string> description;
  };


  void display_exception_cmdline (const Exception& E, int log_level);
  void cmdline_print_func (const std::string& msg);
  void cmdline_report_to_user_func (const std::string& msg, int type);



    class LogLevelLatch
    {
      public:
        LogLevelLatch (const int new_level) :
          prev_level (App::log_level) {
            App::log_level = new_level;
          }

        ~LogLevelLatch () {
          App::log_level = prev_level;
        }

      private:
        const int prev_level;
    };

}

#endif

