#include <gtk/gtk.h>

#include <graphviz/gvplugin_device.h>

#include "callbacks.h"

#define INFO(x) printf("CALLBACK: %s\n",#x);

void init(GVJ_t *job){
  b;la;
  
  GtkWidget *window1, *drawingarea1;
  window1 = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  g_object_set_data(G_OBJECT(window1), "job", (gpointer) job);
  gtk_window_set_title(GTK_WINDOW(window1), "DotEdit");
  
  drawingarea1 = gtk_drawing_area_new();
  g_object_set_data(G_OBJECT(drawingarea1), "job", (gpointer) job);
  gtk_widget_show(drawingarea1);
  gtk_container_add(GTK_CONTAINER(window1), drawingarea1);
  gtk_widget_set_size_request(drawingarea1, 300, 300);
  gtk_widget_set_events(drawingarea1, GDK_EXPOSURE_MASK | GDK_POINTER_MOTION_MASK | GDK_BUTTON_MOTION_MASK | GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK);
  
  g_signal_connect((gpointer) window1, "delete_event",
                   G_CALLBACK(on_window1_delete_event),
                   NULL);
  g_signal_connect((gpointer) drawingarea1, "expose_event",
                   G_CALLBACK(on_drawingarea1_expose_event),
                   NULL);
  g_signal_connect((gpointer) drawingarea1, "motion_notify_event",
                   G_CALLBACK(on_drawingarea1_motion_notify_event),
                   NULL);
  g_signal_connect((gpointer) drawingarea1, "configure_event",
                   G_CALLBACK(on_drawingarea1_configure_event),
                   NULL);
  g_signal_connect((gpointer) drawingarea1, "button_press_event",
                   G_CALLBACK(on_drawingarea1_button_press_event),
                   NULL);
  g_signal_connect((gpointer) drawingarea1, "button_release_event",
                   G_CALLBACK(on_drawingarea1_button_release_event),
                   NULL);
                   
    
    
    
    
    gtk_widget_show(window1);
}

void
on_new1_activate(GtkMenuItem     *menuitem,
                 gpointer         user_data) {
  GtkWindow *window1;
  GVJ_t *job;

  INFO(on_new1_activate);
  
  window1 = GTK_WINDOW(menuitem);
  job = (GVJ_t*)g_object_get_data(G_OBJECT(window1), "job");
  
  (job->callbacks->read)(job, NULL, "dot");
  
  // should there be specific menus for (un)directed graphs etc?
  //  - I think the directed flag only affects layout and rendering
  //      so I plan to make it look like a graph attribute.
  //      Similarly "strict".
}

void
ui_open_graph(GtkWindow *window1, gchar *filename) {
  GVJ_t *job;
  GtkWidget *dialog;

  INFO(ui_open_graph);
  
  job = (GVJ_t*)g_object_get_data(G_OBJECT(window1), "job");
  dialog = gtk_file_chooser_dialog_new(
             "Open graph", window1, GTK_FILE_CHOOSER_ACTION_OPEN,
             "Cancel", GTK_RESPONSE_CANCEL,
             "Open", GTK_RESPONSE_ACCEPT,
             NULL);
  if(filename)
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), filename);
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  gtk_widget_destroy(dialog);
  if(filename) {
    (job->callbacks->read)(job, filename, "dot");
//  if (!file) // we'll probably want to create a error dialog function
//      fprintf(stderr, "Could not open file: %s\n", filename);
//  else
    g_object_set_data_full(G_OBJECT(window1),
                           "activefilename", filename, (GDestroyNotify)g_free);
  }
}

void
on_open1_activate(GtkMenuItem     *menuitem,
                  gpointer         user_data) {
  GtkWindow *window1;
  gchar *filename;
  
  INFO(on_open1_activate);

  window1 = GTK_WINDOW(menuitem);
  filename = (char*)g_object_get_data(G_OBJECT(window1), "activefilename");
  ui_open_graph(window1, filename);
}

void
ui_save_graph(GtkWindow *window1, gchar *filename) {
  GVJ_t *job;
  GtkWidget *dialog;
  
  INFO(ui_save_graph);

  job = (GVJ_t *)g_object_get_data(G_OBJECT(window1), "job");
  
  dialog = gtk_file_chooser_dialog_new(
             "Save graph as", window1, GTK_FILE_CHOOSER_ACTION_SAVE,
             "Cancel", GTK_RESPONSE_CANCEL,
             "Save", GTK_RESPONSE_ACCEPT,
             NULL);
  filename = (char*)g_object_get_data(G_OBJECT(window1), "activefilename");
  if(filename)
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(dialog), filename);
  if(gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
  gtk_widget_destroy(dialog);
  if(filename) {
    (job->callbacks->render)(job, "dot", filename);
    g_object_set_data_full(G_OBJECT(window1),
                           "activefilename", filename, (GDestroyNotify)g_free);
  }
}

void
on_save1_activate(GtkMenuItem     *menuitem,
                  gpointer         user_data) {
  GtkWindow *window1;
  gchar *filename;
  
  window1 = GTK_WINDOW(menuitem);
  filename = (gchar *)g_object_get_data(G_OBJECT(window1), "activefilename");
  ui_save_graph(window1, filename);
}


void
on_save_as1_activate(GtkMenuItem     *menuitem,
                     gpointer         user_data) {
  GtkWindow *window1;

  INFO(on_save_as1_activate);

  window1 = GTK_WINDOW(menuitem);
  ui_save_graph(window1, NULL);
}


void
on_quit1_activate(GtkMenuItem     *menuitem,
                  gpointer         user_data) {

  INFO(on_quit1_activate);

  gtk_widget_destroy(GTK_WIDGET(gtk_widget_get_toplevel(GTK_WIDGET(menuitem))));
  gtk_main_quit();
}


void
on_cut1_activate(GtkMenuItem     *menuitem,
                 gpointer         user_data) {
  INFO(on_cut1_activate);
  // I am thinking that we will annotate a node as to whether it is selected,
  // then retrieve a list of selected nodes for these operations
}


void
on_copy1_activate(GtkMenuItem     *menuitem,
                  gpointer         user_data) {
  INFO(on_copy1_activate);
                  
}


void
on_paste1_activate(GtkMenuItem     *menuitem,
                   gpointer         user_data) { 
 INFO(on_paste1_activate);
                   
}


void
on_delete1_activate(GtkMenuItem     *menuitem,
                    gpointer         user_data) {
                    
 INFO(on_delete1_activate);
}


void
on_about1_activate(GtkMenuItem     *menuitem,
                   gpointer         user_data) {
  gchar *authors[] = {
    "John Ellson",
    "Emden Gansner",
    "Stephen North",
    "special thanks to Michael Lawrence",
    NULL
  };
  INFO(on_about1_activate);
  GtkWindow *window = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(menuitem)));
  gtk_show_about_dialog(window,
                        "name", "DotEdit",
                        "program-name", "DotEdit",
                        "version", "0.1",
                        "copyright", "(C) 2011 AT&T Intellectual Procerty.",
                        "license", "Eclipse Public License v1.0.",
                        "website", "http://www.graphviz.org",
                        "comments", "Visualize and edit graphs of nodes and edges",
                        "authors", authors,
                        NULL);
}


gboolean
on_drawingarea1_expose_event(GtkWidget       *widget,
                             GdkEventExpose  *event,
                             gpointer         user_data) {
  GVJ_t *job;
  cairo_t *cr;

  INFO(on_drawingarea1_expose_event);

  job = (GVJ_t *)g_object_get_data(G_OBJECT(widget),"job");
  cr = gdk_cairo_create(widget->window);
  
  (job->callbacks->motion)(job, job->pointer);
  
  job->context = (void *)cr;
  job->external_context = TRUE;
  job->width = widget->allocation.width;
  job->height = widget->allocation.height;
  if(job->has_been_rendered) {
    (job->callbacks->refresh)(job);
  } else {
    (job->callbacks->refresh)(job);
    
// FIXME - copy image to keyhole
//      the keyhole image is a fixed size and doesn;t need to be recomputed
//      each time.   save a pixmap, then each time, show pixmap and overlay
//      with scaled view rectangle.

  }
  cairo_destroy(cr);
  
  
  return FALSE;
}


gboolean
on_drawingarea1_motion_notify_event(GtkWidget       *widget,
                                    GdkEventMotion  *event,
                                    gpointer         user_data) {
  GVJ_t *job;
  
  INFO(on_drawingarea1_motion_notify_event);

  job = (GVJ_t *)g_object_get_data(G_OBJECT(widget),"job");
  job->pointer.x = event->x;
  job->pointer.y = event->y;
  gtk_widget_queue_draw(widget);
  
  printf("tooltip = \"%s\"\n", job->active_tooltip);
    
  return FALSE;
}






gboolean
on_window1_delete_event(GtkWidget       *widget,
                        GdkEvent        *event,
                        gpointer         user_data) {

  INFO(on_window1_delete_event);

  gtk_main_quit();
  return FALSE;
}


gboolean
on_drawingarea1_configure_event(GtkWidget       *widget,
                                GdkEventConfigure *event,
                                gpointer         user_data) {
  GVJ_t *job;
  double zoom_to_fit;

  INFO(on_drawingarea1_configure_event);
  
  /*FIXME - should allow for margins */
  /*      - similar zoom_to_fit code exists in: */
  /*      plugin/gtk/callbacks.c */
  /*      plugin/xlib/gvdevice_xlib.c */
  /*      lib/gvc/gvevent.c */
  
  job = (GVJ_t *)g_object_get_data(G_OBJECT(widget),"job");
  if(! job->has_been_rendered) {
    zoom_to_fit = MIN((double) event->width / (double) job->width,
                      (double) event->height / (double) job->height);
    if(zoom_to_fit < 1.0)  /* don't make bigger */
      job->zoom *= zoom_to_fit;
  } else if(job->fit_mode) {
    zoom_to_fit = MIN((double) event->width / (double) job->width,
                      (double) event->height / (double) job->height);
    job->zoom *= zoom_to_fit;
  }
  if(event->width > job->width || event->height > job->height)
    job->has_grown = TRUE;
  job->width = event->width;
  job->height = event->height;
  job->needs_refresh = TRUE;
  
  return FALSE;
}


gboolean
on_drawingarea1_button_press_event(GtkWidget       *widget,
                                   GdkEventButton  *event,
                                   gpointer         user_data) {
  GVJ_t *job;
  pointf pointer;

  INFO(on_drawingarea1_button_press_event);

  job = (GVJ_t *)g_object_get_data(G_OBJECT(widget),"job");
  pointer.x = event->x;
  pointer.y = event->y;
  (job->callbacks->button_press)(job, event->button, pointer);
  
  return FALSE;
}


gboolean
on_drawingarea1_button_release_event(GtkWidget       *widget,
                                     GdkEventButton  *event,
                                     gpointer         user_data) {
  GVJ_t *job;
  pointf pointer;

  INFO(on_drawingarea1_button_release_event);

  job = (GVJ_t *)g_object_get_data(G_OBJECT(widget),"job");
  pointer.x = event->x;
  pointer.y = event->y;
  (job->callbacks->button_release)(job, event->button, pointer);
  
  return FALSE;
}


gboolean
on_drawingarea1_scroll_event(GtkWidget       *widget,
                             GdkEvent        *event,
                             gpointer         user_data) {
  GVJ_t *job;
  pointf pointer;
  
  INFO(on_drawingarea1_scroll_event);

  job = (GVJ_t *)g_object_get_data(G_OBJECT(widget),"job");
  pointer.x = ((GdkEventScroll *)event)->x;
  pointer.y = ((GdkEventScroll *)event)->y;
  switch(((GdkEventScroll *)event)->direction) {
    case GDK_SCROLL_UP:
      (job->callbacks->button_press)(job, 4, pointer);
      break;
    case GDK_SCROLL_DOWN:
      (job->callbacks->button_press)(job, 5, pointer);
      break;
    case GDK_SCROLL_LEFT:
    case GDK_SCROLL_RIGHT:
      break;
  }
  gtk_widget_queue_draw(widget);
  
  return FALSE;
}

gboolean
on_button1_button_press_event(GtkWidget       *widget,
                              GdkEventButton  *event,
                              gpointer         user_data) {
                              
                              
  INFO(on_button1_button_press_event);

  fprintf(stderr, "will delete selected object\n");
  
  return FALSE;
}

