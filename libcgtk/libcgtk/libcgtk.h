#ifndef LIBCGTK_H
#define LIBCGTK_H

#ifdef GTK
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

GtkWidget* GCreateWindow(const char* Name,int width,int height);
GtkWidget* GCreateButton(GtkWidget *container,const char* Name,int x,int y,int width,int height);
GtkWidget* GCreateEntry(GtkWidget *container,int x,int y,int width,int height,int size);
GtkWidget* GCreateLabel(GtkWidget *container,const char* input,int x,int y);
GtkWidget* GCreateProgress(GtkWidget *container,int x,int y,int width,int height);
GtkWidget* GCreateFileSelectButton(GtkWidget *container,const char* title,const char* path,GtkFileChooserAction action,int x,int y,int width,int height);
GtkWidget* GCreateRadio(GtkWidget *container,GtkWidget *pack,int x,int y,int width,int height);
void GMessageBox(GtkWidget *container,const char* body,const char* title,GtkButtonsType type);

#endif //GTK
#endif //LIBCGTK_H
