create table chats
(
    id         INTEGER not null
        constraint chats_pk
            primary key autoincrement,
    title      TEXT,
    type       INTEGER not null,
    icon_id    INTEGER,
    created_at INTEGER not null,
    updated_at INTEGER not null
);

create table icons
(
    id   INTEGER not null
        constraint icons_pk
            primary key autoincrement,
    path TEXT    not null
);

create table users
(
    id         INTEGER not null,
    username      TEXT    not null
        constraint users_pk
            unique,
    password   TEXT    not null,
    icon_id    INTEGER,
    created_at INTEGER not null,
    updated_at INTEGER not null
);

create table messages
(
    id         INTEGER not null
        constraint messages_pk
            primary key autoincrement,
    user_id    INTEGER not null
        constraint messages_users_id_fk
            references users (id),
    chat_id    INTEGER not null,
    body       TEXT    not null,
    created_at INTEGER not null,
    updated_at INTEGER not null,
    deleted_at INTEGER
);

create index index_name
    on messages (chat_id);

create index index_id
    on users (id);

create table users_auth_tokens
(
    id           INTEGER not null
        constraint users_auth_tokens_pk
            primary key autoincrement,
    user_id      INTEGER not null,
    token        TEXT    not null,
    is_expired   INTEGER not null,
    created_at   INTEGER not null,
    expired_at   INTEGER not null,
    last_used_at INTEGER not null
);

create index users_auth_tokens_user_id_is_expired_index
    on users_auth_tokens (user_id, is_expired);

create table users_chats
(
    id         INTEGER not null
        constraint users_chats_pk
            primary key autoincrement,
    chat_id    INTEGER not null,
    user_id    INTEGER not null,
    role       INTEGER not null,
    created_at INTEGER not null,
    constraint users_chats_unique_key
        unique (chat_id, user_id)
);

create index users_chats_chat_id_index
    on users_chats (chat_id);

create index users_chats_user_id_index
    on users_chats (user_id);