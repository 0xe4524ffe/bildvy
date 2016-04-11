#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libfm/fm-gtk.h>
#include <magic.h>

#define INLINE inline __attribute__((__always_inline__))


#define G_K_FLAGS (G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS)

typedef GtkWidget* pGtkWidget;

typedef enum _SortType{
	NameAZ,
	NameZA,
	TimeAZ,
	TimeZA
} SortType;

void RefreshSort(void);

INLINE int GtkWindowGetWdth(pGtkWidget window){
	while (gtk_events_pending()) gtk_main_iteration();
	GdkWindow *wwindow=gtk_widget_get_window(window);
	return gdk_window_get_width(wwindow);
}

INLINE int GtkWindowGetHght(pGtkWidget window){
	while (gtk_events_pending()) gtk_main_iteration();
	GdkWindow *wwindow=gtk_widget_get_window(window);
	return gdk_window_get_height(wwindow);
}

INLINE void GKeyFileUnload(GKeyFile *key, char *path){
	uintptr_t len;
	char *data=g_key_file_to_data(key, &len, NULL);
	g_file_set_contents((const char*)path, (const char*)data, len, NULL);
	g_free(data);
	g_key_file_free(key);
}
