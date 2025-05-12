#include "gui.h"
#include "network.h"
#include "crypto.h"
#include <gtk/gtk.h>
#include <string.h>
#include <pthread.h>
#include <gtk/gtkwindow.h>

static GtkWidget *entry_email;
static GtkWidget *entry_password;
static GtkWidget *entry_message;
static GtkWidget *text_view;
static GtkTextBuffer *buffer;
static GtkWidget *login_window;
static GtkWidget *chat_window;

// Création compte
static GtkWidget *entry_fname, *entry_lname;
static GtkWidget *entry_reg_email, *entry_reg_password;

// ➤ Affiche un message dans la zone de texte
static void append_to_view(const char *msg) {
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(buffer, &end);
    gtk_text_buffer_insert(buffer, &end, msg, -1);
    gtk_text_buffer_insert(buffer, &end, "\n", -1);
}

// ➤ Thread de réception des messages
static void *receive_thread(void *arg) {
    char recv_buf[1024];
    while (1) {
        int len = recv_message(recv_buf, sizeof(recv_buf));
        if (len <= 0) {
            g_idle_add((GSourceFunc)append_to_view, g_strdup("❌ Déconnecté du serveur."));
            break;
        }
        recv_buf[len] = '\0';
        g_idle_add((GSourceFunc)append_to_view, g_strdup(recv_buf));
    }
    return NULL;
}

static void on_send_clicked(GtkButton *button, gpointer user_data) {
    const char *text = gtk_editable_get_text(GTK_EDITABLE(entry_message));
    if (text && strlen(text) > 0) {
        send_message(text);
        append_to_view(text);
        gtk_editable_set_text(GTK_EDITABLE(entry_message), "");
    }
}

// ➤ Affiche la fenêtre de chat
static void show_chat_window(GtkApplication *app) {
    chat_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(chat_window), "MyDiscord Chat");
    gtk_window_set_default_size(GTK_WINDOW(chat_window), 400, 300);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(chat_window), box);

    text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_box_append(GTK_BOX(box), text_view);

    entry_message = gtk_entry_new();
    gtk_box_append(GTK_BOX(box), entry_message);

    GtkWidget *send_btn = gtk_button_new_with_label("Envoyer");
    gtk_box_append(GTK_BOX(box), send_btn);
    g_signal_connect(send_btn, "clicked", G_CALLBACK(on_send_clicked), NULL);

    gtk_window_present(GTK_WINDOW(chat_window));

    pthread_t tid;
    pthread_create(&tid, NULL, receive_thread, NULL);
}

static void on_login_clicked(GtkButton *button, gpointer user_data) {
    GtkApplication *app = GTK_APPLICATION(user_data);

    const char *email = gtk_editable_get_text(GTK_EDITABLE(entry_email));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(entry_password));

    char hashed[128];
    xor_hash(password, hashed, "supercle");

    if (connect_to_server("127.0.0.1", 4242) != 0) {
        g_warning("❌ Connexion au serveur impossible.");
        return;
    }

    char auth[256];
    snprintf(auth, sizeof(auth), "%s:%s", email, hashed);
    printf("[CLIENT] Email envoyé : %s\n", email);
    printf("[CLIENT] Hash envoyé  : %s\n", hashed);
    send_message(auth);

    char response[128];
    int len = recv_message(response, sizeof(response));
    response[len] = '\0';

    if (strstr(response, "OK")) {
        gtk_widget_set_visible(login_window, FALSE);
        show_chat_window(app);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(login_window),
            GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
            "Identifiants invalides !");
        gtk_window_present(GTK_WINDOW(dialog));
    }
}

// ➤ Création de compte
static void on_register_clicked(GtkButton *btn, gpointer user_data) {
    const char *fname = gtk_editable_get_text(GTK_EDITABLE(entry_fname));
    const char *lname = gtk_editable_get_text(GTK_EDITABLE(entry_lname));
    const char *email = gtk_editable_get_text(GTK_EDITABLE(entry_reg_email));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(entry_reg_password));

    char hash[128];
    xor_hash(password, hash, "supercle");

    char msg[512];
    snprintf(msg, sizeof(msg), "REGISTER:%s:%s:%s:%s", email, hash, fname, lname);

    if (connect_to_server("127.0.0.1", 4242) != 0) {
        g_warning("Connexion au serveur échouée");
        return;
    }

    send_message(msg);

    char response[128];
    int len = recv_message(response, sizeof(response));
    response[len] = '\0';

    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL,
        GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
        strstr(response, "OK") ? "✅ Compte créé avec succès !" : "❌ Échec : email déjà utilisé.");
    gtk_window_present(GTK_WINDOW(dialog));
}

// ➤ Fenêtre de création de compte
static void show_register_window(GtkButton *btn, gpointer user_data) {
    GtkWidget *window = gtk_window_new();
    gtk_window_set_title(GTK_WINDOW(window), "Créer un compte");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 250);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_window_set_child(GTK_WINDOW(window), box);

    entry_fname = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_fname), "Prénom");
    gtk_box_append(GTK_BOX(box), entry_fname);

    entry_lname = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_lname), "Nom");
    gtk_box_append(GTK_BOX(box), entry_lname);

    entry_reg_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_reg_email), "Email");
    gtk_box_append(GTK_BOX(box), entry_reg_email);

    entry_reg_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_reg_password), "Mot de passe");
    gtk_entry_set_visibility(GTK_ENTRY(entry_reg_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_reg_password);

    GtkWidget *reg_btn = gtk_button_new_with_label("Créer");
    gtk_box_append(GTK_BOX(box), reg_btn);
    g_signal_connect(reg_btn, "clicked", G_CALLBACK(on_register_clicked), NULL);

    gtk_window_present(GTK_WINDOW(window));
}

// ➤ Fenêtre de login
static void show_login_window(GtkApplication *app) {
    login_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(login_window), "Connexion - MyDiscord");
    gtk_window_set_default_size(GTK_WINDOW(login_window), 300, 200);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_window_set_child(GTK_WINDOW(login_window), box);

    entry_email = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Email");
    gtk_box_append(GTK_BOX(box), entry_email);

    entry_password = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Mot de passe");
    gtk_entry_set_visibility(GTK_ENTRY(entry_password), FALSE);
    gtk_box_append(GTK_BOX(box), entry_password);

    GtkWidget *login_btn = gtk_button_new_with_label("Se connecter");
    gtk_box_append(GTK_BOX(box), login_btn);
    g_signal_connect(login_btn, "clicked", G_CALLBACK(on_login_clicked), app);

    GtkWidget *reg_btn = gtk_button_new_with_label("Créer un compte");
    gtk_box_append(GTK_BOX(box), reg_btn);
    g_signal_connect(reg_btn, "clicked", G_CALLBACK(show_register_window), NULL);

    gtk_window_present(GTK_WINDOW(login_window));
}

// ➤ Démarrage
void init_gui(int argc, char *argv[]) {
    GtkApplication *app = gtk_application_new("com.mydiscord.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(show_login_window), NULL);
    g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);
}
