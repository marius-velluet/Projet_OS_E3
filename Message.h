typedef struct {
    int  server_id;   // 4 chiffres
    int  place_id;    // 4 chiffres  
    int  menu_id;     // 4 chiffres
    int  request_id;  // pour la table de routage
    char payload[512];
} Message;
