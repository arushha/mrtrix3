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


#ifndef __file_dicom_csa_entry_h__
#define __file_dicom_csa_entry_h__

#include "datatype.h"
#include "get_set.h"
#include "file/dicom/element.h"

namespace MR {
  namespace File {
    namespace Dicom {

        class CSAEntry {
          public:
            CSAEntry (const uint8_t* start_p, const uint8_t* end_p, bool output_fields = false) :
              start (start_p),
              end (end_p),
              print (output_fields) {
                if (strncmp ("SV10", (const char*) start, 4)) 
                  DEBUG ("WARNING: CSA data is not in SV10 format");

                cnum = 0;
                num = getLE<uint32_t> (start+8);
                next = start + 16;
              }


            bool parse () {
              if (cnum >= num) 
                return false;
              start = next;
              if (start >= end + 84) 
                return false;
              strncpy (name, (const char*) start, 64);
              getLE<uint32_t> (start+64); // vm
              strncpy (vr, (const char*) start+68, 4);
              getLE<uint32_t> (start+72); // syngodt
              nitems = getLE<uint32_t> (start+76);
              if (print) 
                fprintf (stdout, "    [CSA] %s: ", name);
              next = start + 84;
              if (next + 4 >= end) 
                return false;

              for (uint32_t m = 0; m < nitems; m++) {
                uint32_t length = getLE<uint32_t> (next);
                size_t size = 16 + 4*((length+3)/4);
                if (next + size > end) 
                  return false;
                if (print) 
                  fprintf (stdout, "%.*s ", length, (const char*) next+16);
                next += size;
              }
              if (print) 
                fprintf (stdout, "\n");

              cnum++;
              return true;
            }

            const char* key () const { return (name); }
            int get_int () const { 
              const uint8_t* p = start + 84;
              for (uint32_t m = 0; m < nitems; m++) {
                uint32_t length = getLE<uint32_t> (p);
                if (length) 
                  return to<int> (std::string (reinterpret_cast<const char*> (p)+16, 4*((length+3)/4)));
                p += 16 + 4*((length+3)/4);
              }
              return 0;
            }

            float get_float () const { 
              const uint8_t* p = start + 84;
              for (uint32_t m = 0; m < nitems; m++) {
                uint32_t length = getLE<uint32_t> (p);
                if (length) 
                  return to<float> (std::string (reinterpret_cast<const char*> (p)+16, 4*((length+3)/4)));
                p += 16 + 4*((length+3)/4);
              }
              return NAN;
            }

            void get_float (float* v) const { 
              const uint8_t* p = start + 84;
              for (uint32_t m = 0; m < nitems; m++) {
                uint32_t length = getLE<uint32_t> (p);
                if (length) 
                  v[m] = to<float> (std::string (reinterpret_cast<const char*> (p)+16, 4*((length+3)/4)));
                p += 16 + 4*((length+3)/4);
              }
            }

            friend std::ostream& operator<< (std::ostream& stream, const CSAEntry& item) {
              stream << "[CSA] " << item.name << ":";
              const uint8_t* next = item.start + 84;

              for (uint32_t m = 0; m < item.nitems; m++) {
                uint32_t length = getLE<uint32_t> (next);
                size_t size = 16 + 4*((length+3)/4);
                while (length > 0 && !next[16+length-1]) 
                  length--;
                stream << " ";
                stream.write (reinterpret_cast<const char*> (next)+16, length);
                next += size;
              }

              return stream;
            }


          protected:
            const uint8_t* start;
            const uint8_t* next;
            const uint8_t* end;
            bool print;
            char name[65], vr[5];
            uint32_t nitems, num, cnum;

        };





    }
  }
}

#endif


