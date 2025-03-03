#include <stdlib.h>
#include <stdio.h>

#include <gtk/gtk.h>

static GtkBuilder *build;

G_MODULE_EXPORT void calculate_footprint(void) {
  double footprint = 0;

  GtkSpinButton *electric = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "electric"));
  footprint += gtk_spin_button_get_value(electric) * 105;

  GtkSpinButton *gas = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "gas"));
  footprint += gtk_spin_button_get_value(gas) * 105;

  GtkSpinButton *oil = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "oil"));
  footprint += gtk_spin_button_get_value(oil) * 113;

  GtkSpinButton *mileage = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "mileage"));
  footprint += gtk_spin_button_get_value(mileage) * 0.79;

  GtkSpinButton *flights = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "flights"));
  footprint += gtk_spin_button_get_value(flights) * 1100;

  GtkSpinButton *flights4 = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "flights4"));
  footprint += gtk_spin_button_get_value(flights4) * 4400;

  GtkCheckButton *newspaper = GTK_CHECK_BUTTON(gtk_builder_get_object(build, "newspaper"));
  if (!gtk_check_button_get_active(newspaper)) footprint += 184;

  GtkCheckButton *aluminum = GTK_CHECK_BUTTON(gtk_builder_get_object(build, "aluminum"));
  if (!gtk_check_button_get_active(aluminum)) footprint += 166;

  char *str = malloc((snprintf(NULL, 0, "Carbon footprint: %glbs", footprint) + 1) * sizeof(char));
  sprintf(str, "Carbon footprint: %glbs", footprint);
  GtkLabel *label = GTK_LABEL(gtk_builder_get_object(build, "footprint"));
  gtk_label_set_text(label, str);
  free(str);
}

static void activate(GtkApplication *app) {
  build = gtk_builder_new_from_resource("/net/catech-software/climate-tracker/climate-tracker.ui");

  GtkApplicationWindow *window = GTK_APPLICATION_WINDOW(gtk_builder_get_object(build, "window"));
  gtk_window_set_application(GTK_WINDOW(window), app);

  gtk_window_present(GTK_WINDOW(window));

  calculate_footprint();
}

int main(int argc, char *argv[]) {
  GtkApplication *app = gtk_application_new("net.catech_software.climate_tracker", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  g_object_unref(build);

  return status;
}
