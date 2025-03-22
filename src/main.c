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
#include <sys/stat.h>
#include <time.h>

#include <glib.h>
#include <gtk/gtk.h>

static GtkBuilder *build;
static char *path;
static FILE *fp;
static double footprint = 0;
static GtkLabel **history;
static size_t history_len = 16;
static GtkBox *history_box;

static ptrdiff_t fgetline(char **restrict lineptr, size_t *restrict n, FILE *restrict stream) {
  char chunk[256];
  size_t len = sizeof(chunk);
  *lineptr = realloc(*lineptr, len);
  (*lineptr)[0] = '\0';

  while (fgets(chunk, sizeof(chunk), stream) != NULL) {
    size_t chunk_len = strlen(chunk);
    size_t line_len = strlen(*lineptr);
    if (len - line_len < chunk_len + 1) {
      len *= 2;
      *lineptr = realloc(*lineptr, len);
    }

    memcpy(*lineptr + line_len, chunk, chunk_len);
    line_len += chunk_len;
    if ((*lineptr)[line_len - 1] == '\n') {
      (*lineptr)[line_len - 1] = '\0';
      len = strlen(*lineptr) + 1;
      *lineptr = realloc(*lineptr, len);
      if (n != NULL) *n = len;
      return len - 1;
    }
    (*lineptr)[line_len] = '\0';
  }

  return -1;
}

G_MODULE_EXPORT void calculate_footprint(void) {
  footprint = 0;

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

G_MODULE_EXPORT void save_to_history(void) {
  time_t current_time;
  time(&current_time);
  struct tm *date = localtime(&current_time);
  char time_string[11];
  // using this program after year 9999 is undefined behavior :)
  strftime(time_string, sizeof(time_string), "%Y-%m-%d", date);

  history = realloc(history, ++history_len * sizeof(GtkLabel*));
  char *str = malloc((snprintf(NULL, 0, "%s: %g", time_string, footprint) + 1) * sizeof(char));
  sprintf(str, "%s: %g", time_string, footprint);
  history[history_len - 1] = GTK_LABEL(gtk_label_new(str));
  gtk_label_set_xalign(history[history_len - 1], 0);
  gtk_box_append(history_box, GTK_WIDGET(history[history_len - 1]));

  fprintf(fp, "%s\n", str);
  free(str);
  fflush(fp);
}

G_MODULE_EXPORT void clear_history(void) {
  for (size_t i = 0; i < history_len; i++) gtk_box_remove(history_box, GTK_WIDGET(history[i]));
  history_len = 0;
  free(history);
  history = NULL; // ensure history can be realloc'd

  // clear history
  freopen(path, "w+", fp);
}

static void activate(GtkApplication *app) {
  build = gtk_builder_new_from_resource("/net/catech-software/climate-tracker/climate-tracker.ui");

  GtkApplicationWindow *window = GTK_APPLICATION_WINDOW(gtk_builder_get_object(build, "window"));
  gtk_window_set_application(GTK_WINDOW(window), app);

  calculate_footprint();

  const char *data_dir = g_get_user_data_dir();
  char *dir = g_build_path("/", data_dir, "net.catech-software.climate-tracker", NULL);
#if _WIN32
  _mkdir(dir);
#else
  mkdir(dir, 0700);
#endif
  path = g_build_path("/", dir, "history", NULL);
  free(dir);
  fp = fopen(path, "a+");
  rewind(fp);
  history = malloc(history_len * sizeof(GtkLabel*));
  size_t history_count = 0;
  history_box = GTK_BOX(gtk_builder_get_object(build, "history"));
  char *str = NULL;
  while (fgetline(&str, NULL, fp) != -1) {
    if (++history_count > history_len) {
      history_len *= 2;
      history = realloc(history, history_len * sizeof(GtkLabel*));
    }
    history[history_count - 1] = GTK_LABEL(gtk_label_new(str));
    gtk_label_set_xalign(history[history_count - 1], 0);
    gtk_box_append(history_box, GTK_WIDGET(history[history_count - 1]));
  }
  history_len = history_count;
  history = realloc(history, history_len * sizeof(GtkLabel*));
  free(str);
  fseek(fp, 0, SEEK_END);

  gtk_window_present(GTK_WINDOW(window));
}

static void shutdown(void) {
  free(history);
  fclose(fp);
  free(path);
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
