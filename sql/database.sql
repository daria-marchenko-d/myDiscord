-- Base de données : MyDiscord
-- Créée pour supporter les fonctionnalités utilisateur, messagerie, canaux, etc.

-- 🔐 Utilisateurs
CREATE TABLE IF NOT EXISTS users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    firstname TEXT NOT NULL,
    lastname TEXT NOT NULL,
    email TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,
    role TEXT CHECK(role IN ('member', 'moderator', 'admin')) DEFAULT 'member',
    status TEXT CHECK(status IN ('online', 'offline', 'busy', 'away')) DEFAULT 'offline'
);

-- 📡 Canaux (channels)
CREATE TABLE IF NOT EXISTS channels (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    is_private BOOLEAN NOT NULL DEFAULT 0
);

-- 💬 Messages
CREATE TABLE IF NOT EXISTS messages (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    channel_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    content TEXT NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    is_encrypted BOOLEAN DEFAULT 0,
    FOREIGN KEY(channel_id) REFERENCES channels(id) ON DELETE CASCADE,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- 😀 Réactions (emojis)
CREATE TABLE IF NOT EXISTS reactions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    message_id INTEGER NOT NULL,
    user_id INTEGER NOT NULL,
    emoji TEXT NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(message_id) REFERENCES messages(id) ON DELETE CASCADE,
    FOREIGN KEY(user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- 📎 Transfert de fichiers
CREATE TABLE IF NOT EXISTS files (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    channel_id INTEGER NOT NULL,
    filename TEXT NOT NULL,
    filepath TEXT NOT NULL,
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(user_id) REFERENCES users(id),
    FOREIGN KEY(channel_id) REFERENCES channels(id)
);

-- 🏷️ Mentions (@utilisateurs dans messages)
CREATE TABLE IF NOT EXISTS mentions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    message_id INTEGER NOT NULL,
    mentioned_user_id INTEGER NOT NULL,
    FOREIGN KEY(message_id) REFERENCES messages(id) ON DELETE CASCADE,
    FOREIGN KEY(mentioned_user_id) REFERENCES users(id) ON DELETE CASCADE
);

-- 🔑 Permissions personnalisées par canal (optionnel si besoin)
-- CREATE TABLE IF NOT EXISTS channel_permissions (
--     id INTEGER PRIMARY KEY AUTOINCREMENT,
--     user_id INTEGER,
--     channel_id INTEGER,
--     can_send BOOLEAN DEFAULT 1,
--     can_delete BOOLEAN DEFAULT 0,
--     can_manage BOOLEAN DEFAULT 0,
--     FOREIGN KEY(user_id) REFERENCES users(id),
--     FOREIGN KEY(channel_id) REFERENCES channels(id)
-- );
