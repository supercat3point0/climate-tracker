#include <gtk/gtk.h>

static void print_hello(GtkWidget *widget, gpointer data) {
  g_print("Hello, World!\n");
}

static void activate(GtkApplication *app, gpointer user_data) {
  GtkBuilder *build;
  GtkWidget *window;
  GtkWidget *button;

  build = gtk_builder_new_from_resource("/net/catech-software/climate-tracker/climate-tracker.ui");

  window = GTK_WIDGET(gtk_builder_get_object(build, "window"));
  gtk_window_set_application(GTK_WINDOW(window), app);

  button = GTK_WIDGET(gtk_builder_get_object(build, "button"));
  g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

  g_object_unref(build);

  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("net.catech_software.climate_tracker", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
