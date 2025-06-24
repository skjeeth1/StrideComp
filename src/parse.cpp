struct Vec {
    long x;
    long y;
};

struct Vec current_location = {0, 0};
struct Vec origin_location = {0, 0};
int bound_radius = 10 ;
String authorizedNumber = "+919876543210";

void toUpperCase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

//set origin func
void set_origin() {
    origin.x = cur_location.x;
    origin.y = cur_location.y;

    Serial.println("ORIGIN SET:");
    Serial.print("X: "); Serial.println(origin.x);
    Serial.print("Y: "); Serial.println(origin.y);
}

//set distance func
void set_distance(int distance) {
    Serial.print("Previous Distance: ");
    Serial.println(geofence_dist);

    geofence_dist = distance;

    Serial.print("New Distance Set To: ");
    Serial.println(geofence_dist);
}

// parses the message recieved by sim module 
void parseMessage(char *message)
{    
  
    if (message.indexOf(authorizedNumber) == -1) {
      Serial.println(" Unauthorized sender.");
      return;
    }
  
    Serial.println("Authorized sender.");
  
    toUpperCase(message);  

    if (strstr(message, "SET ORIGIN") != NULL)
    {
        set_origin();
    }

    char *distPtr = strstr(message, "SET DISTANCE ");
    if (distPtr != NULL)
    {
        distPtr += strlen("SET DISTANCE ");
        int new_distance = atoi(distPtr);
        if (new_distance > 0)
        {
            set_distance(new_distance);
        }
    }
}
