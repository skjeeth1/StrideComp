#include "parse.h"

// set origin func
void set_origin()
{
    originLocation.x = currentLocation.x;
    originLocation.y = currentLocation.y;

    Serial.println("ORIGIN SET:");
    Serial.print("X: ");
    Serial.println(originLocation.x);
    Serial.print("Y: ");
    Serial.println(originLocation.y);
}

// set distance func
void set_distance(int distance)
{
    Serial.print("Previous Distance: ");
    Serial.println(bound_radius);

    bound_radius = distance;

    Serial.print("New Distance Set To: ");
    Serial.println(bound_radius);
}

// parses the message recieved by sim module
void parseMessage(String message)
{
    // Check for authorized number
    // if (message.indexOf(authorizedNumber) == -1)
    // {
    //     Serial.println(" Unauthorized sender.");
    //     return;
    // }

    // Serial.println(" Authorized sender.");

    // Convert to uppercase for case-insensitive comparison
    message.toUpperCase();

    // Check for "SET ORIGIN"
    if (message.indexOf("SET ORIGIN") != -1)
    {
        set_origin();
    }

    // Check for "SET DISTANCE "
    int distIndex = message.indexOf("SET DISTANCE ");
    if (distIndex != -1)
    {
        String afterCommand = message.substring(distIndex + strlen("SET DISTANCE "));
        int new_distance = afterCommand.toInt();
        if (new_distance > 0)
        {
            set_distance(new_distance);
        }
    }
}
