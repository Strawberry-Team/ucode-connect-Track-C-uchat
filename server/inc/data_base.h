#include "server.h"

typedef struct s_user_data
{
    int id;
    char *username;
    unsigned char *password;
    int image_id;
    char *filename;
    char *extension;
    char *file_path;
} t_user_data;


// functions from "define_client_request.c" file
t_user_data db_get_user_data()


