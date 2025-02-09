#include <stdlib.h>
#include <gtk/gtk.h>

G_MODULE_EXPORT void print_hello(GtkButton *button) {
  g_print("%s pressed\n", gtk_buildable_get_buildable_id(GTK_BUILDABLE(button)));
}

static void activate(GtkApplication *app) {
  GtkBuilder *build;
  GtkApplicationWindow *window;

  build = gtk_builder_new_from_resource("/net/catech-software/climate-tracker/climate-tracker.ui");

  window = GTK_APPLICATION_WINDOW(gtk_builder_get_object(build, "window"));
  gtk_window_set_application(GTK_WINDOW(window), app);

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
