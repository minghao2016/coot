/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */


#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#if defined _MSC_VER
#include <string>
#else
#include <unistd.h>
#include <string.h>
#endif

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "seq-view-interface.h"
#include "c-interface.h"  // for set_sequence_view_is_displayed (thank you FR)
#include "c-interface-gtk-widgets.h"

GtkWidget*
seq_lookup_widget(GtkWidget       *widget,
		  const gchar     *widget_name) {

  GtkWidget *parent = NULL, *found_widget = NULL;

  if (widget) { 
    for (;;)
      {
	if (GTK_IS_MENU (widget))
	  parent = gtk_menu_get_attach_widget (GTK_MENU (widget));
	else
	  // parent = widget->parent;
	  parent = gtk_widget_get_parent(widget);
	if (parent == NULL)
	  break;
	widget = parent;
      }
    found_widget = (GtkWidget*) g_object_get_data (G_OBJECT (widget), widget_name);
    if (!found_widget)
      g_warning ("Widget not found: %s", widget_name);
  }
  return found_widget;
}

GtkWidget*
create_sequence_view_dialog (void)
{
  GtkWidget *sequence_view_dialog;
  GtkWidget *dialog_vbox1;
  GtkWidget *sequence_view_scrolledwindow;
  GtkWidget *dialog_action_area1;
  GtkWidget *hbox1;
  GtkWidget *sequence_view_close_button;
  GtkWidget *aa;

  sequence_view_dialog = gtk_dialog_new ();
  g_object_set_data (G_OBJECT (sequence_view_dialog), "sequence_view_dialog", sequence_view_dialog);
  gtk_window_set_title (GTK_WINDOW (sequence_view_dialog), "Sequence View");
  gtk_window_set_policy (GTK_WINDOW (sequence_view_dialog), TRUE, TRUE, FALSE);

  // dialog_vbox1 = GTK_DIALOG (sequence_view_dialog)->vbox;
  dialog_vbox1 = gtk_dialog_get_content_bar(GTK_DIALOG(sequence_view_dialog));
  g_object_set_data (G_OBJECT (sequence_view_dialog), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  sequence_view_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (sequence_view_scrolledwindow);
  g_object_set_data_full(G_OBJECT (sequence_view_dialog),
			 "sequence_view_scrolledwindow",
			 sequence_view_scrolledwindow,
			 NULL);
  gtk_widget_show (sequence_view_scrolledwindow);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), sequence_view_scrolledwindow, TRUE, TRUE, 0);

  // dialog_action_area1 = GTK_DIALOG (sequence_view_dialog)->action_area;
  dialog_action_area1 = gtk_dialog_get_action_area(GTK_DIALOG(sequence_view_dialog));

  g_object_set_data(G_OBJECT(sequence_view_dialog), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog_action_area1), 10);

  hbox1 = gtk_hbox_new (FALSE, 0);
  g_object_ref (hbox1);
  g_object_set_data_full (G_OBJECT (sequence_view_dialog), "hbox1", hbox1, NULL);
  gtk_widget_show (hbox1);
  gtk_box_pack_start (GTK_BOX (dialog_action_area1), hbox1, TRUE, TRUE, 0);

  sequence_view_close_button = gtk_button_new_with_label ("  Close ");
  g_object_ref(sequence_view_close_button);
  g_object_set_data_full (G_OBJECT (sequence_view_dialog), 
			  "sequence_view_close_button", sequence_view_close_button, NULL);

  g_signal_connect (G_OBJECT(sequence_view_close_button), "clicked",
		      G_CALLBACK(on_sequence_view_close_button_clicked),
		      NULL);

  g_signal_connect (G_OBJECT (sequence_view_dialog), "destroy",
                      G_CALLBACK (on_sequence_view_dialog_destroy),
                      sequence_view_dialog);

  gtk_widget_show (sequence_view_close_button);
  gtk_box_pack_start (GTK_BOX (hbox1), sequence_view_close_button, TRUE, TRUE, 0);

  return sequence_view_dialog;
}

void
on_sequence_view_close_button_clicked     (GtkButton *button,
					   gpointer         user_data)
{
   GtkWidget *window = seq_lookup_widget(GTK_WIDGET(button), "sequence_view_dialog");
   gtk_widget_destroy(window);
}


void
on_sequence_view_dialog_destroy            (GtkWidget       *object,
                                            gpointer         user_data)
{
   GtkWidget *dialog = (GtkWidget *) user_data;
   int imol = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(dialog), "imol"));
/*    printf("debug:: on_sequence_view_dialog_destroy unsetting sequence view for molecule %d using widget %ld\n", imol, dialog); */
   set_sequence_view_is_displayed(0, imol);
}
