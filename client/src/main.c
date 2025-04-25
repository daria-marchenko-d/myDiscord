#include <gtk/gtk.h>
#include "ui.h"
#include "db.h"

int main(int argc, char *argv[])
{
    // Initialize database connection
    if (!db_init())
    {
        fprintf(stderr, "Failed to initialize database connection.\n");
        return 1;
    }

    // Create GTK application
    GtkApplication *app = gtk_application_new("org.example.mydiscord", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);

    // Run the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Cleanup
    g_object_unref(app);
    db_close(); // Close the database connection

    return status;
}
