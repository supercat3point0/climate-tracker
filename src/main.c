/*
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * main.c - entry point
 * Copyright (C) 2025 Bennett Jann
 *
 * This file is part of climate-tracker.
 *
 * climate-tracker is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * climate-tracker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdio.h>

#include <glib.h>
#include <glib/gstdio.h>
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

  const char *data_dir = g_get_user_data_dir();
  char *dir = g_build_path("/", data_dir, "net.catech-software.climate-tracker", NULL);
  g_mkdir(dir, 0700);
  char *path = g_build_path("/", dir, "history.xml", NULL);
  free(dir);
  if (g_access(path, F_OK) != 0) g_close(g_creat(path, 600), NULL);
  free(path);
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
