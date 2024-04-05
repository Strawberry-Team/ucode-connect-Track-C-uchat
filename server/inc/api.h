#pragma once

#include "server.h"
#include "database.h"


// functions from "process_client_request.c" file
void handle_login(cJSON *json);
