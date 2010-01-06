#ifdef GTK

#ifdef WIN32
char slash[] = "\\";
char temp_drive[] = "e";
#else
char slash[] = "/";
char temp_drive[] = "/dev/hda";
#endif

GtkWidget* progress_bar = 0;

struct s_data{
  char* firm_in;
  GtkWidget* firm_out;
  char* dir;
  GtkWidget* drive;
}gdata;

//close everything related to gtk
static void destroy(GtkWidget *widget,gpointer data){
  gtk_main_quit();
}

static void g_flash(GtkWidget *widget,gpointer data){
  char* drive = (char*)gtk_entry_get_text(GTK_ENTRY(gdata.drive));
  if(!drive && !strlen(drive)) return;
  if(!gdata.firm_in && !strlen(gdata.firm_in)) return;

  cmd_drive(&drive);
  cmd_flashfirm(&gdata.firm_in);
}

static void g_dump1(GtkWidget *widget,gpointer data){
  char* drive = (char*)gtk_entry_get_text(GTK_ENTRY(gdata.drive));
  if(!drive && !strlen(drive)) return;

  char* firm_out = (char*)gtk_entry_get_text(GTK_ENTRY(gdata.firm_out));
  if(!firm_out && !strlen(firm_out)) return;

  char* firm_out2 = (char*)malloc(strlen((char*)(((string)gdata.dir)+slash+((string)firm_out)).c_str()));
  if(!firm_out2) return;

  if(!strcpy(firm_out2,(char*)(((string)gdata.dir)+slash+((string)firm_out)).c_str()))return;

  cmd_drive(&drive);
  cmd_dumpmain(&firm_out2);

  free(firm_out2);
}

static void g_dump2(GtkWidget *widget,gpointer data){
  char* drive = (char*)gtk_entry_get_text(GTK_ENTRY(gdata.drive));
  if(!drive && !strlen(drive)) return;

  char* firm_out = (char*)gtk_entry_get_text(GTK_ENTRY(gdata.firm_out));
  if(!firm_out && !strlen(firm_out)) return;

  char* firm_out2 = (char*)malloc(strlen((char*)(((string)gdata.dir)+slash+((string)firm_out)).c_str()));
  if(!firm_out2) return;

  if(!strcpy(firm_out2,(char*)(((string)gdata.dir)+slash+((string)firm_out)).c_str()))return;

  cmd_drive(&drive);
  cmd_dumpcore(&firm_out2);

  free(firm_out2);
}

//
static void file_select(GtkWidget *widget,gpointer data){
  gdata.firm_in = (char*)gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
}

static void save_location(GtkWidget *widget,gpointer data){
  gdata.dir = (char*)gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
}

void drive_added(GVolumeMonitor *volume_monitor,GDrive *drive,gpointer user_data){
  GMessageBox(0,"temp","temp",GTK_BUTTONS_OK);
}

void create_tab1(GtkWidget* hWindow){
  int move_x = 4;
  int move_y = 0;

  GCreateLabel(hWindow,"Firmware File:",move_x+0,move_y+40);
  GtkWidget *filesel = GCreateFileSelectButton(hWindow,"Select a File",".",GTK_FILE_CHOOSER_ACTION_OPEN,move_x+0,move_y+58,284,24);
  g_signal_connect(G_OBJECT(filesel),"file-set",G_CALLBACK(file_select),NULL);

  GtkWidget* bflash = GCreateButton(hWindow,"Flash",move_x+28,move_y+217,72,24);
  g_signal_connect(G_OBJECT(bflash),"clicked",G_CALLBACK(g_flash),NULL);
}

void create_tab2(GtkWidget* hWindow){
  int move_x = 4;
  int move_y = 0;

  GCreateLabel(hWindow,"Save Location:",move_x+0,move_y+40);
  GtkWidget *saveloc = GCreateFileSelectButton(hWindow,"Save Location",".",GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,move_x+0,move_y+58,284,24);
  g_signal_connect(G_OBJECT(saveloc),"file-set",G_CALLBACK(save_location),NULL);
  g_signal_connect(G_OBJECT(saveloc),"current-folder-changed",G_CALLBACK(save_location),NULL);

  GCreateLabel(hWindow,"Save Name:",move_x+0,move_y+86);
  gdata.firm_out = GCreateEntry(hWindow,move_x+0,move_y+102,284,24,256);
  gtk_entry_set_text(GTK_ENTRY(gdata.firm_out),"dump.bin");

  GtkWidget* bdump1 = GCreateButton(hWindow,"MAIN" ,move_x+28 ,move_y+217,72,24);
  g_signal_connect(G_OBJECT(bdump1),"clicked",G_CALLBACK(g_dump1),NULL);

  GtkWidget* bdump2 = GCreateButton(hWindow,"CORE" ,move_x+56 ,move_y+217,72,24);
  g_signal_connect(G_OBJECT(bdump2),"clicked",G_CALLBACK(g_dump2),NULL);
}

int main (int argc, char *argv[]){
  int move_x = 4;
  int move_y = 26;

  memset(&gdata,0,sizeof(gdata));

  //required to run any gtk app
  gtk_init(&argc,&argv);

  GtkWidget* hWindow = GCreateWindow(STR_NAME,308,280);
  GtkWidget* window = gtk_widget_get_toplevel(hWindow);

  g_signal_connect(G_OBJECT(window),"destroy",G_CALLBACK(destroy),NULL); //bind the click of the X to kill the program

  GtkWidget* notebook = gtk_notebook_new();
  gtk_fixed_put(GTK_FIXED(hWindow),notebook,0,0);
  gtk_widget_set_size_request(notebook,308,25);
  gtk_widget_show(notebook);

  GtkWidget* tab1 = gtk_fixed_new();
  gtk_widget_show(tab1);
  GtkWidget* tab2 = gtk_fixed_new();
  gtk_widget_show(tab2);

  GCreateLabel(hWindow,"Drive:",move_x+0,move_y+0);
  gdata.drive = GCreateEntry(hWindow,move_x+0,move_y+16,284,24,256);
  gtk_entry_set_text(GTK_ENTRY(gdata.drive),temp_drive);

  progress_bar = GCreateProgress(hWindow,move_x+0,move_y+186,284,24);

  GtkWidget* bexit  = GCreateButton(hWindow,"Exit" ,move_x+180,move_y+220,72,24);
  g_signal_connect(G_OBJECT(bexit) ,"clicked",G_CALLBACK(destroy),NULL);

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),tab1,gtk_label_new("Flash"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),tab2,gtk_label_new("Dump"));
  create_tab1(tab1);
  create_tab2(tab2);

  //    GVolumeMonitor* gm = g_volume_monitor_get();
  //    g_signal_connect(G_OBJECT(gm),"drive-changed",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"drive-connected",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"drive-disconnected",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"drive-eject-button",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"mount-added",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"mount-changed",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"mount-pre-unmount",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"mount-removed",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"volume-added",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"volume-changed",G_CALLBACK(drive_added),NULL);
  //    g_signal_connect(G_OBJECT(gm),"volume-removed",G_CALLBACK(drive_added),NULL);

  gtk_widget_show(window);

  //this is used for getting widgets responces
  gtk_main();
  return 0;
}
#endif //GTK
