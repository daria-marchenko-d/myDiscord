myDiscord.sql: --Users table

CREATE TABLE UserAccount (user_id SERIAL PRIMARY KEY,
                                                 first_name VARCHAR(50),
                                                            email VARCHAR(100) UNIQUE NOT NULL,
                                                                                      password_hash TEXT NOT NULL,
                                                                                                         role VARCHAR(20) CHECK (role IN ('user',
                                                                                                                                          'moderator',
                                                                                                                                          'administrator')) DEFAULT 'user',
                                                                                                                                                                    status VARCHAR(20) DEFAULT 'online' -- online, away, busy
 );

--Channels table

CREATE TABLE Channel
    (channel_id SERIAL PRIMARY KEY,
                               channel_name VARCHAR(50) NOT NULL,
                                                        is_private BOOLEAN DEFAULT FALSE,
                                                                                   creator_id INT REFERENCES UserAccount(user_id) ON DELETE
     SET NULL);

--Messages table

CREATE TABLE Message
    (message_id SERIAL PRIMARY KEY,
                               content TEXT NOT NULL,
                                            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                                                        sender_id INT REFERENCES UserAccount(user_id) ON DELETE CASCADE,
                                                                                                                                channel_id INT REFERENCES Channel(channel_id) ON DELETE CASCADE,
                                                                                                                                                                                        is_encrypted BOOLEAN DEFAULT FALSE);

--Reactions table

CREATE TABLE Reaction
    (reaction_id SERIAL PRIMARY KEY,
                                emoji VARCHAR(10) NOT NULL,
                                                  message_id INT REFERENCES Message(message_id) ON DELETE CASCADE,
                                                                                                          user_id INT REFERENCES UserAccount(user_id) ON DELETE CASCADE);

-- private messages table

CREATE TABLE PrivateMessage (private_message_id SERIAL PRIMARY KEY,
                                                               content TEXT NOT NULL,
                                                                            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                                                                                                        sender_id INT REFERENCES UserAccount(user_id),
                                                                                                                                 receiver_id INT REFERENCES UserAccount(user_id),
                                                                                                                                                            is_encrypted BOOLEAN DEFAULT FALSE);

--Threads table

CREATE TABLE Thread (thread_id SERIAL PRIMARY KEY,
                                              thread_name VARCHAR(100),
                                                          channel_id INT REFERENCES Channel(channel_id),
                                                                                    creator_id INT REFERENCES UserAccount(user_id),
                                                                                                              created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP);

--Files table

CREATE TABLE FileUpload
    (file_id SERIAL PRIMARY KEY,
                            file_name VARCHAR(255),
                                      file_path TEXT NOT NULL,
                                                     uploader_id INT REFERENCES UserAccount(user_id),
                                                                                message_id INT REFERENCES Message(message_id) ON DELETE
     SET NULL);

