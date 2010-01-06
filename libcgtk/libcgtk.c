#ifdef GTK
#include <libcgtk/libcgtk.h>

GtkWidget* GCreateWindow(const char* Name,int width,int height){
  GtkWidget *window;
  GtkWidget *container;
  GdkGeometry geometery;
  //setup the main window and its look
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW(window),Name);
  gtk_container_set_border_width(GTK_CONTAINER(window),5);
  //Set size of window
  geometery.min_width = width;
  geometery.min_height = height;
  geometery.max_width = width;
  geometery.max_height = height;
  gtk_window_set_geometry_hints(GTK_WINDOW(window),window,&geometery,(GdkWindowHints)(GDK_HINT_MIN_SIZE|GDK_HINT_MAX_SIZE));

  container = gtk_fixed_new();
  gtk_container_add(GTK_CONTAINER(window),container);
  gtk_widget_set_size_request(container,width,height);
  gtk_widget_show (container);
  return(container);
};

GtkWidget* GCreateButton(GtkWidget *container,const char* Name,int x,int y,int width,int height){
  GtkWidget *button = gtk_button_new_with_label(Name);
  gtk_fixed_put(GTK_FIXED (container),button,x,y);
  gtk_widget_set_size_request(button,width,height);
  gtk_widget_show(button);
  return(button);
};

GtkWidget* GCreateEntry(GtkWidget *container,int x,int y,int width,int height,int size){
  GtkWidget* entry = gtk_entry_new();
  gtk_entry_set_max_length((GtkEntry*)entry,size);
  gtk_widget_set_size_request(entry,width,height);
  gtk_fixed_put(GTK_FIXED (container),entry,x,y);
  gtk_widget_show(entry);
  return(entry);
};

GtkWidget* GCreateLabel(GtkWidget *container,const char* input,int x,int y){
  GtkWidget* lable = gtk_label_new((gchar*)input);
  gtk_fixed_put(GTK_FIXED (container),lable,x,y);
  gtk_widget_show(lable);
  return(lable);
};

GtkWidget* GCreateFileSelectButton(GtkWidget *container,const char* title,const char* path,GtkFileChooserAction action,int x,int y,int width,int height){
  GtkWidget *fileselect = gtk_file_chooser_button_new(title,action);
  gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(fileselect),".");
  gtk_fixed_put(GTK_FIXED(container),fileselect,x,y);
  gtk_widget_set_size_request(fileselect,width,height);
  gtk_widget_show(fileselect);
  return(fileselect);
};

GtkWidget* GCreateProgress(GtkWidget *container,int x,int y,int width,int height){
  GtkWidget *progress = gtk_progress_bar_new();
  gtk_fixed_put(GTK_FIXED(container),progress,x,y);
  gtk_widget_set_size_request(progress,width,height);
  gtk_widget_show(progress);
  return(progress);
};

void GMessageBox(GtkWidget * container,const char* body,const char* title,GtkButtonsType type){
  GtkWidget* main_window = gtk_widget_get_toplevel(container);
  GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(main_window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_OTHER,type,body);
  gtk_window_set_title(GTK_WINDOW(dialog),title);
  g_signal_connect_swapped (dialog, "response",G_CALLBACK(gtk_widget_destroy),dialog);
  gtk_widget_show(dialog);
}
#endif GTK

