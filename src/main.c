#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include <glib.h>
#include <gtk/gtk.h>

static GtkBuilder *build;

G_MODULE_EXPORT void calculate_footprint(void) {
  double footprint = 0;

  // add 105lbs for every dollar spent on electricity
  GtkSpinButton *electric = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "electric"));
  footprint += gtk_spin_button_get_value(electric) * 105;

  // add 105lbs for every dollar spent on gas
  GtkSpinButton *gas = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "gas"));
  footprint += gtk_spin_button_get_value(gas) * 105;

  // add 113lbs for every dollar spent on oil
  GtkSpinButton *oil = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "oil"));
  footprint += gtk_spin_button_get_value(oil) * 113;

  // add 0.79lbs for every mile driven
  GtkSpinButton *mileage = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "mileage"));
  footprint += gtk_spin_button_get_value(mileage) * 0.79;

  // add 1100lbs for every flight less than 4 hours
  GtkSpinButton *flights = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "flights"));
  footprint += gtk_spin_button_get_value(flights) * 1100;

  // add 4400lbs for every flight over 4 hours
  GtkSpinButton *flights4 = GTK_SPIN_BUTTON(gtk_builder_get_object(build, "flights4"));
  footprint += gtk_spin_button_get_value(flights4) * 4400;

  // add 184lbs if you don't recycle newspaper
  GtkCheckButton *newspaper = GTK_CHECK_BUTTON(gtk_builder_get_object(build, "newspaper"));
  if (!gtk_check_button_get_active(newspaper)) footprint += 184;

  // add 166lbs if you don't recycle aluminum and tin
  GtkCheckButton *aluminum = GTK_CHECK_BUTTON(gtk_builder_get_object(build, "aluminum"));
  if (!gtk_check_button_get_active(aluminum)) footprint += 166;

  // use snprintf to calculate the size of the formatted string
  // use %g instead of %f so there aren't any trailing zeros
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

  const char *user_data_dir = g_get_user_data_dir();
  const char *data_dir = "/net.catech_software.climate_tracker";
  const char *file = "/file.txt";
  char *path = malloc(strlen(user_data_dir) + strlen(data_dir) + strlen(file) + 1);
  strcpy(path, user_data_dir);
  strcat(path, data_dir);
 #ifdef _WIN32
  mkdir(path);
#else
  mkdir(path, 0700);
#endif
  strcat(path, file);
  FILE *fp = fopen(path, "w");
  free(path);
  fprintf(fp, "Hello, World!\n");
  fclose(fp);
}

static void shutdown(void) {
  g_object_unref(build);
}

int main(int argc, char *argv[]) {
  GtkApplication *app = gtk_application_new("net.catech_software.climate_tracker", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  g_signal_connect(app, "shutdown", G_CALLBACK(shutdown), NULL);
  int status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
