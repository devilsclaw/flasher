#ifdef GTK

extern GtkWidget* progress_bar;
#define ud_progress(perc) \
  if(progress_bar) { \
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progress_bar),perc); \
    gdk_flush(); \
    gdk_window_process_all_updates();}

#endif //GTK
