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

#ifndef __viewer_window_h__
#define __viewer_window_h__

#include <QMainWindow>

#ifdef Complex
# undef Complex
#endif

#include "ptr.h"
#include "cursor.h"
#include "math/quaternion.h"
#include "mrview/image.h"

class QMenu;
class QAction;
class QActionGroup;

namespace MR {
  namespace Viewer {

    namespace Mode { class Base; }
    namespace Tool { class Base; }

    class Window : public QMainWindow
    {
      Q_OBJECT

      public:
        Window();
        ~Window();

        void add_images (VecPtr<MR::Image::Header>& list);

      private slots:
        void image_open_slot ();
        void image_save_slot ();
        void image_close_slot ();
        void image_properties_slot ();

        void select_mode_slot (QAction* action);
        void select_colourmap_slot ();
        void full_screen_slot ();

        void image_next_slot ();
        void image_previous_slot ();
        void image_reset_slot ();
        void image_interpolate_slot ();
        void image_select_slot (QAction* action);

        void OpenGL_slot ();
        void about_slot ();
        void aboutQt_slot ();

      private:
        Cursor cursors_do_not_use;

        class GLArea;

        GLArea *glarea;
        Mode::Base* mode;

        Point<> focal_point, camera_target;
        Math::Quaternion orient;
        float field_of_view;
        int proj;

        QMenu *file_menu, *view_menu, *tool_menu, *image_menu, *help_menu, *colourmap_menu;
        QAction *open_action, *save_action, *close_action, *properties_action, *quit_action;
        QAction *view_menu_mode_area, *view_menu_mode_common_area, *reset_windowing_action;
        QAction *image_interpolate_action, *full_screen_action;
        QAction **mode_actions, **colourmap_actions, *invert_colourmap_action;
        QAction *next_image_action, *prev_image_action, *image_list_area;
        QAction *OpenGL_action, *about_action, *aboutQt_action;
        QActionGroup *mode_group, *image_group, *colourmap_group;

        void paintGL ();
        void initGL ();
        void resizeGL (int width, int height);
        void mousePressEventGL (QMouseEvent* event);
        void mouseMoveEventGL (QMouseEvent* event);
        void mouseDoubleClickEventGL (QMouseEvent* event);
        void mouseReleaseEventGL (QMouseEvent* event);
        void wheelEventGL (QWheelEvent* event);

        void set_image_menu ();
        Image* current_image () { return (static_cast<Image*> (image_group->checkedAction())); }

        friend class Image;
        friend class Mode::Base;
        friend class Window::GLArea;
    };

  }
}

#endif
