
INLINE void GUIBuild(void){
	//useless variables
	pGtkWidget cntV2Box, cntPaned,
	mmbMenu, mmiFile, mmmFile, mmiOpen, mmiQuit,
	mmiView, mmmView, mmiZoomOut, mmiZoomIn, mmiZoom100, mmiZoomFit,
	mmiSort, mmmSort, mmiNameAZ, mmiNameZA, mmiTimeAZ, mmiTimeZA,
	mmiHelp, mmmHelp, mmiBout;

	//build gui
	frmMain=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(frmMain), "Image Viewer");
	pbfIcon=gdk_pixbuf_new_from_file("/usr/share/icons/bildvy.png", NULL);
	gtk_window_set_icon(GTK_WINDOW(frmMain), pbfIcon);
	mmbMenu=gtk_menu_bar_new();
	mmiFile=gtk_menu_item_new_with_mnemonic("_File");
	gtk_container_add(GTK_CONTAINER(mmbMenu), mmiFile);
	mmmFile=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mmiFile), mmmFile);
	mmiOpen=gtk_menu_item_new_with_mnemonic("_Open");
	gtk_container_add(GTK_CONTAINER(mmmFile), mmiOpen);
	mmiQuit=gtk_menu_item_new_with_mnemonic("_Quit");
	gtk_container_add(GTK_CONTAINER(mmmFile), mmiQuit);
	mmiView=gtk_menu_item_new_with_mnemonic("_View");
	gtk_container_add(GTK_CONTAINER(mmbMenu), mmiView);
	mmmView=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mmiView), mmmView);
	mmiZoomOut=gtk_menu_item_new_with_mnemonic("_- Zoom Out");
	gtk_container_add(GTK_CONTAINER(mmmView), mmiZoomOut);
	mmiZoomIn=gtk_menu_item_new_with_mnemonic("_+ Zoom In");
	gtk_container_add(GTK_CONTAINER(mmmView), mmiZoomIn);
	mmiZoom100=gtk_menu_item_new_with_mnemonic("_0 Original Size");
	gtk_container_add(GTK_CONTAINER(mmmView), mmiZoom100);
	mmiZoomFit=gtk_menu_item_new_with_mnemonic("Zoom To Fit");
	gtk_container_add(GTK_CONTAINER(mmmView), mmiZoomFit);
	mmiSort=gtk_menu_item_new_with_mnemonic("Sort");
	gtk_container_add(GTK_CONTAINER(mmbMenu), mmiSort);
	mmmSort=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mmiSort), mmmSort);
	mmiNameAZ=gtk_menu_item_new_with_mnemonic("By Name A-Z");
	mmiNameZA=gtk_menu_item_new_with_mnemonic("By Name Z-A");
	mmiTimeAZ=gtk_menu_item_new_with_mnemonic("By Time A-Z");
	mmiTimeZA=gtk_menu_item_new_with_mnemonic("By Time Z-A");
	gtk_container_add(GTK_CONTAINER(mmmSort), mmiNameAZ);
	gtk_container_add(GTK_CONTAINER(mmmSort), mmiNameZA);
	gtk_container_add(GTK_CONTAINER(mmmSort), mmiTimeAZ);
	gtk_container_add(GTK_CONTAINER(mmmSort), mmiTimeZA);
	mmiHelp=gtk_menu_item_new_with_mnemonic("_Help");
	gtk_container_add(GTK_CONTAINER(mmbMenu), mmiHelp);
	mmmHelp=gtk_menu_new();
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(mmiHelp), mmmHelp);
	mmiBout=gtk_menu_item_new_with_mnemonic("_About");
	gtk_container_add(GTK_CONTAINER(mmmHelp), mmiBout);
	cntDraw=gtk_drawing_area_new();
	cntScroll=gtk_scrolled_window_new(NULL, NULL);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(cntScroll), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_container_add(GTK_CONTAINER(cntScroll), cntDraw);
	objPreView=(FmFolderView*) fm_standard_view_new(FM_FV_THUMBNAIL_VIEW, NULL, NULL);
		fm_folder_view_set_selection_mode(objPreView, GTK_SELECTION_BROWSE);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(objPreView), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
	cntPaned=gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
		gtk_paned_add1(GTK_PANED(cntPaned), GTK_WIDGET(objPreView));
		gtk_paned_add2(GTK_PANED(cntPaned), GTK_WIDGET(cntScroll));
	cntV2Box=gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
		gtk_box_pack_start(GTK_BOX(cntV2Box), mmbMenu, false, false, 0);
		gtk_box_pack_start(GTK_BOX(cntV2Box), cntPaned, true, true, 0);
	gtk_container_add(GTK_CONTAINER(frmMain), cntV2Box);

	//connect signals
	g_signal_connect_swapped(G_OBJECT(frmMain), "delete-event", G_CALLBACK(frmMainDelete), NULL);
	g_signal_connect_swapped(G_OBJECT(frmMain), "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(G_OBJECT(mmiOpen), "activate", G_CALLBACK(mmiOpenAction), NULL);
	g_signal_connect(G_OBJECT(mmiQuit), "activate", G_CALLBACK(mmiQuitAction), NULL);
	g_signal_connect(G_OBJECT(mmiZoomOut), "activate", G_CALLBACK(mmiZoomOutAction), NULL);
	g_signal_connect(G_OBJECT(mmiZoomIn), "activate", G_CALLBACK(mmiZoomInAction), NULL);
	g_signal_connect(G_OBJECT(mmiZoom100), "activate", G_CALLBACK(mmiZoom100Action), NULL);
	g_signal_connect(G_OBJECT(mmiZoomFit), "activate", G_CALLBACK(mmiZoomFitAction), NULL);
	g_signal_connect(G_OBJECT(mmiNameAZ), "activate", G_CALLBACK(mmiNameAZAction), NULL);
	g_signal_connect(G_OBJECT(mmiNameZA), "activate", G_CALLBACK(mmiNameZAAction), NULL);
	g_signal_connect(G_OBJECT(mmiTimeAZ), "activate", G_CALLBACK(mmiTimeAZAction), NULL);
	g_signal_connect(G_OBJECT(mmiTimeZA), "activate", G_CALLBACK(mmiTimeZAAction), NULL);
	g_signal_connect(G_OBJECT(mmiBout), "activate", G_CALLBACK(mmiBoutAction), NULL);
	g_signal_connect(G_OBJECT(cntDraw), "draw", G_CALLBACK(cntDrawDraw), NULL);
	g_signal_connect(G_OBJECT(objPreView), "sel-changed", G_CALLBACK(objPreViewSelect), NULL);
}
