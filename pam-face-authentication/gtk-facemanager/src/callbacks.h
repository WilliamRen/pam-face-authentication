#include <gtk/gtk.h>


void
on_assign_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_userList_select_row                 (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_userList_unselect_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_addUser_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_removeUser_clicked                  (GtkButton       *button,
                                        gpointer         user_data);

void
on_save_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_exit_clicked                        (GtkButton       *button,
                                        gpointer         user_data);

void
on_add_clicked                         (GtkButton       *button,
                                        gpointer         user_data);

void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data);

void
on_yes_clicked                         (GtkButton       *button,
                                        gpointer         user_data);

void
on_no_clicked                          (GtkButton       *button,
                                        gpointer         user_data);
