/*
   Copyright 2009 Brain Research Institute, Melbourne, Australia

   Written by J-Donald Tournier, 13/11/09.

   This file is part of MRtrix.

   MRtrix is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   MRtrix is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __gui_mrview_mode_volume_h__
#define __gui_mrview_mode_volume_h__

#include "app.h"
#include "gui/mrview/mode/base.h"

namespace MR
{
  namespace GUI
  {
    namespace MRView
    {
      namespace Mode
      {

        class Volume : public Base
        {
          public:
            Volume (Window& parent) :
              Base (parent, FocusContrast | MoveTarget | TiltRotate | ShaderTransparency | ShaderThreshold),
              use_depth_testing (false),
              volume_shader (*this) { }

            virtual void paint (Projection& projection);
          protected:
            GL::VertexBuffer volume_VB, volume_VI;
            GL::VertexArrayObject volume_VAO;
            bool use_depth_testing;
            GL::Texture depth_texture;

            class Shader : public Displayable::Shader {
              public:
                Shader (const Volume& mode) : mode (mode), clip1 (false), clip2 (false), clip3 (false), use_depth_testing (false) { }
                virtual std::string vertex_shader_source (const Displayable& object);
                virtual std::string fragment_shader_source (const Displayable& object);
                virtual bool need_update (const Displayable& object) const;
                virtual void update (const Displayable& object);
              protected:
                const Volume& mode;
                bool clip1, clip2, clip3;
                bool use_depth_testing;
            } volume_shader;

        };

      }
    }
  }
}

#endif





