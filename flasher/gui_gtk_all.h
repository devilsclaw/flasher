/*This file is part of flasher.

  flasher is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  flasher is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with flasher.  If not, see <http://www.gnu.org/licenses/>.
  */

#ifdef GTK

extern GtkWidget* progress_bar;
#define ud_progress(perc) \
  if(progress_bar) { \
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar),perc); \
    gdk_flush(); \
    gdk_window_process_all_updates();}

#endif //GTK
