#include "main.h"

// Variables
bool bImage=false, bFit=false, bCairo=true;
SortType iSortMode=TimeAZ;
char *sFilePath=NULL, *sConfFile;
cairo_t *crContext;
cairo_surface_t *crSurface=NULL;
GdkPixbuf *pbfImage;
GdkWindow *windowDraw, *windowScroll;
GKeyFile *objKeyFile;
FmFolderView* objPreView=NULL;
typedef int (*FmFilter)(struct _FmFileInfo *, void *);
pGtkWidget frmMain, cntScroll, cntDraw;
double iScale=1;
magic_t objMagic=NULL;

bool fFilter(FmFileInfo *file, void G_GNUC_UNUSED *data){
	return fm_file_info_is_image(file);
}

FmFolderModel* GetFolderModel(char *sPath){
	FmFolder* folder=fm_folder_from_path_name((const char*)sPath);
	FmFolderModel* model=fm_folder_model_new(folder, false);
	g_object_unref(folder);
	fm_folder_model_add_filter(model, (FmFilter)fFilter, NULL);
	fm_folder_model_apply_filters(model);
	return model;
}

G_GNUC_CONST double ScaleCalc(double w1, double h1, double w2, double h2){
	double iSc1=w1/w2, iSc2=(h1-32)/h2;
	return MIN(iSc1, iSc2);}

G_GNUC_CONST uintptr_t WHCalc(uintptr_t i1, uintptr_t i2, double scale){
	return rint((i1>>1)-((i2>>1)*scale))/scale;}

void Expose(void){
	if (bImage){
		windowDraw=gtk_widget_get_window(GTK_WIDGET(cntDraw));
		uintptr_t iImageWidth;
		uintptr_t iImageHeight;
		if (!bCairo){
			iImageWidth=gdk_pixbuf_get_width(pbfImage);
			iImageHeight=gdk_pixbuf_get_height(pbfImage);
		} else {
			iImageWidth=cairo_image_surface_get_width(crSurface);
			iImageHeight=cairo_image_surface_get_height(crSurface);
		}
		uintptr_t iWidth=0;
		uintptr_t iHeight=0;
		if (bFit){
			windowScroll=gtk_widget_get_window(GTK_WIDGET(cntScroll));
			iScale=ScaleCalc(gdk_window_get_width(windowScroll), gdk_window_get_height(windowScroll), iImageWidth, iImageHeight);
		}
		gtk_widget_set_size_request(cntDraw, rint(iImageWidth*iScale), rint(iImageHeight*iScale));
		uintptr_t iDrawWidth=gdk_window_get_width(windowDraw);
		uintptr_t iDrawHeight=gdk_window_get_height(windowDraw);
		crContext=gdk_cairo_create(windowDraw);
		cairo_scale(crContext, iScale, iScale);
		if (iDrawWidth>rint(iImageWidth*iScale)){
			iWidth=WHCalc(iDrawWidth, iImageWidth, iScale);
		}
		if (iDrawHeight>rint(iImageHeight*iScale)){
			iHeight=WHCalc(iDrawHeight, iImageHeight, iScale);
		}
		if (!bCairo){
			gdk_cairo_set_source_pixbuf(crContext, pbfImage, iWidth, iHeight);
		} else {
			cairo_set_source_surface(crContext, crSurface, iWidth, iHeight);
		}
		cairo_pattern_set_filter(cairo_get_source(crContext), CAIRO_FILTER_FAST);
		if (iScale>=2.0) cairo_pattern_set_filter(cairo_get_source(crContext), CAIRO_FILTER_NEAREST);
		cairo_paint(crContext);
		cairo_stroke(crContext);
		cairo_destroy(crContext);
	}
}

bool OfType(char *filepath, char *filetype){
	char *sTmp=(char*)magic_file(objMagic, (const char*)filepath);
	return !!strstr((const char*)sTmp, (const char*)filetype);
}

INLINE void SettingsRead(void){
	sConfFile=g_build_filename(((char*)g_get_user_config_dir()), "bildvy.conf", NULL);
	
	objKeyFile=g_key_file_new();
	if(!g_key_file_load_from_file(objKeyFile, (const char*)sConfFile, G_K_FLAGS, NULL)){
		g_printerr("Error: configuration file does not exist.\n");
		g_file_set_contents((const char*)sConfFile, "\n", -1, NULL);
		if(!g_key_file_load_from_file(objKeyFile, (const char*)sConfFile, G_K_FLAGS, NULL)){
			g_printerr("Error: error while loading configurations file.\n");
			exit(EXIT_FAILURE);
		}
	}
	
	if ((g_key_file_has_key(objKeyFile, "Settings", "Width", NULL))&&(g_key_file_has_key(objKeyFile, "Settings", "Height", NULL))) gtk_window_resize(GTK_WINDOW(frmMain), g_key_file_get_integer(objKeyFile, "Settings", "Width", NULL), g_key_file_get_integer(objKeyFile, "Settings", "Height", NULL));
	if ((g_key_file_has_key(objKeyFile, "Settings", "x", NULL))&&(g_key_file_has_key(objKeyFile, "Settings", "y", NULL))) gtk_window_move(GTK_WINDOW(frmMain), g_key_file_get_integer(objKeyFile, "Settings", "x", NULL), g_key_file_get_integer(objKeyFile, "Settings", "y", NULL));
	if (g_key_file_has_key(objKeyFile, "Settings", "BestFit", NULL)) bFit=g_key_file_get_boolean(objKeyFile, "Settings", "BestFit", NULL);
	if (g_key_file_has_key(objKeyFile, "Settings", "Sort", NULL)) iSortMode=g_key_file_get_integer(objKeyFile, "Settings", "Sort", NULL);
}

void SettingsWrite(void){
	int iX, iY;
	g_key_file_set_integer(objKeyFile, "Settings", "Width", GtkWindowGetWdth(GTK_WIDGET(frmMain)));
	g_key_file_set_integer(objKeyFile, "Settings", "Height", GtkWindowGetHght(GTK_WIDGET(frmMain)));
	gtk_window_get_position(GTK_WINDOW(frmMain), &iX, &iY);
	g_key_file_set_integer(objKeyFile, "Settings", "x", iX);
	g_key_file_set_integer(objKeyFile, "Settings", "y", iY);
	g_key_file_set_boolean(objKeyFile, "Settings", "BestFit", bFit);
	g_key_file_set_integer(objKeyFile, "Settings", "Sort", iSortMode);
	GKeyFileUnload(objKeyFile, sConfFile);
}

void SortRefresh(void){
	FmFolderModel* objModel=GetFolderModel(g_path_get_dirname((const char*)sFilePath));
	fm_folder_view_set_model(objPreView, objModel);
	
	switch (iSortMode){
		case NameAZ: fm_folder_model_set_sort(objModel, FM_FOLDER_MODEL_COL_NAME, FM_SORT_ASCENDING); break;
		case NameZA: fm_folder_model_set_sort(objModel, FM_FOLDER_MODEL_COL_NAME, FM_SORT_DESCENDING); break;
		case TimeAZ: fm_folder_model_set_sort(objModel, FM_FOLDER_MODEL_COL_MTIME, FM_SORT_ASCENDING); break;
		case TimeZA: fm_folder_model_set_sort(objModel, FM_FOLDER_MODEL_COL_MTIME, FM_SORT_DESCENDING); break;
	}
	
	g_object_unref(objModel);
	gtk_widget_show_all(GTK_WIDGET(objPreView));
}

bool frmMainDelete(pGtkWidget G_GNUC_UNUSED widget, GdkEvent G_GNUC_UNUSED *event, void G_GNUC_UNUSED *data){
	SettingsWrite();
	return false;
}

void FileOpen(bool bChf){
	if (OfType(sFilePath, (char *)"image")){
		if (!bCairo) g_object_unref(G_OBJECT(pbfImage));
		if(OfType(sFilePath, (char *)"PNG")){
			bCairo=true;
			cairo_surface_destroy(crSurface);
			crSurface=cairo_image_surface_create_from_png((const char*)sFilePath);
			bImage=true;
		} else {
			pbfImage=gdk_pixbuf_new_from_file((const char*)sFilePath, NULL);
			bImage=true;
			bCairo=false;}
		Expose();
		if (bChf) SortRefresh();
	} else {
		GtkWidget *dlgError;
		dlgError=gtk_message_dialog_new(GTK_WINDOW(frmMain), GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "The file doesn't look like an image.");
		gtk_window_set_title(GTK_WINDOW(dlgError), "Error!");
		gtk_widget_show(dlgError);
		g_signal_connect_object(G_OBJECT(dlgError), "response", G_CALLBACK(gtk_widget_destroy), G_OBJECT(dlgError), G_CONNECT_AFTER);
	}
}

void mmiOpenAction(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* user_data){
	GtkWidget *dlgFileChooser;
	int iResponse;
	
	dlgFileChooser=gtk_file_chooser_dialog_new("Open file", GTK_WINDOW(frmMain), GTK_FILE_CHOOSER_ACTION_OPEN, "Cancel", 1, "Open", 0, NULL);
	iResponse=gtk_dialog_run(GTK_DIALOG(dlgFileChooser));
	if (iResponse==0){
		g_free(sFilePath);
		sFilePath=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dlgFileChooser));
		FileOpen(true);
	}
	gtk_widget_destroy(dlgFileChooser);
}

bool mmiQuitAction(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* data){
	SettingsWrite();
	gtk_main_quit();
	return false;}

void mmiZoomOutAction(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* data){
	bFit=false;
	iScale=iScale-0.1;
	Expose();
}

void mmiZoomInAction(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* data){
	bFit=false;
	iScale=iScale+0.1;
	Expose();
}

void mmiZoom100Action(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* data){
	bFit=false;
	iScale=1.0;
	Expose();
}

void mmiZoomFitAction(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* data){
	bFit=true;
	iScale=1.0;
	Expose();
}

void mmiNameAZAction(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* data){
	iSortMode=NameAZ;
	SortRefresh();
}

void mmiNameZAAction(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* data){
	iSortMode=NameZA;
	SortRefresh();
}

void mmiTimeAZAction(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* data){
	iSortMode=TimeAZ;
	SortRefresh();
}

void mmiTimeZAAction(GtkMenuItem G_GNUC_UNUSED *menuitem, G_GNUC_UNUSED void* data){
	iSortMode=TimeZA;
	SortRefresh();
}

void mmiBoutAction(GtkMenuItem G_GNUC_UNUSED *menuitem, void* data){
	GtkWidget *dlgBout;
	
	dlgBout=gtk_about_dialog_new();
	gtk_window_set_transient_for(GTK_WINDOW(dlgBout), GTK_WINDOW(data));
	gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dlgBout), "Bildvy");
	gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dlgBout), "4.0.0.450");
	gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dlgBout), "Copyright 2012-2016 Sokolov N. P. <0xE4524FFE@gmail.com>");
	gtk_about_dialog_set_license_type(GTK_ABOUT_DIALOG(dlgBout), GTK_LICENSE_CUSTOM);
	gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dlgBout), "https://github.com/0xe4524ffe/bildvy");
	gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dlgBout), "By using this software you accept FLWP EULA\nYou can find FLWP EULA at https://github.com/0xe4524ffe/FLWP-EULA if wasn't included\n");
	gtk_about_dialog_set_logo_icon_name(GTK_ABOUT_DIALOG(dlgBout), NULL);
	g_signal_connect_object(G_OBJECT(dlgBout), "response", G_CALLBACK(gtk_widget_destroy), G_OBJECT(dlgBout), G_CONNECT_AFTER);
	gtk_widget_show(dlgBout);
}

void cntDrawDraw(pGtkWidget G_GNUC_UNUSED widget, cairo_t G_GNUC_UNUSED *cr, void G_GNUC_UNUSED *data){
	Expose();
}

void objPreViewSelect(FmFolderView G_GNUC_UNUSED *fv, intptr_t G_GNUC_UNUSED n_sel, void G_GNUC_UNUSED *data){
	FmPathList *list=fm_folder_view_dup_selected_file_paths(objPreView);
	FmPath *fmpath=FM_PATH(fm_list_peek_head(FM_LIST(list)));
	sFilePath=fm_path_to_str(fmpath);
	fm_path_list_unref(list);
	FileOpen(false);
}

#include "gui.h"

int main(int argc, char *argv[]){

	objMagic=magic_open(MAGIC_MIME); //poff! some magic here
	magic_load(objMagic, NULL);
	
	gtk_init(&argc, &argv);	//gtk initialization
	fm_gtk_init(NULL);
	GUIBuild();
	gtk_widget_show_all(frmMain);
	
	SettingsRead();
	
	if (argc>1){
		sFilePath=argv[1];
		if (!(g_path_is_absolute((const char*)sFilePath))) sFilePath=g_build_filename(g_get_current_dir(), argv[1], NULL);
		FileOpen(true);
	}
	
	gtk_main();
	fm_gtk_finalize();
	magic_close(objMagic);
	return EXIT_SUCCESS;
}
