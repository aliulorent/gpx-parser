#include "GPXHelpers.h"

int main(){

    //char* temp = GPXtoJSON(doc);
    //printf("%s\n", temp);
    GPXdoc* doc = JSONtoGPX("{\"version\":1.1,\"creator\":\"WebTool\"}");
    printf("Creator: %s\n", doc->creator);
    printf("Version: %f\n", doc->version);
    printf("Namespace: %s\n", doc->namespace);
    Waypoint* wp = JSONtoWaypoint("{\"lat\":43.537299,\"lon\":-80.218267}");
    printf("Lat: %f\n", wp->latitude);
    printf("Long: %f\n", wp->longitude);
    Route* rt = JSONtoRoute("{\"name\":\"Reynolds Walk\"}");
    printf("Name: %s\n", rt->name);
    printf("Adding route & waypoint to doc\n");
    addWaypoint(rt, wp);
    addRoute(doc, rt);
    Route *rt2 = (Route*)getFromBack(doc->routes);
    Waypoint* wp2 = (Waypoint*)getFromBack(rt2->waypoints);
    printf("Last Route name: %s\n", rt2->name);
    printf("Last waypoint in route: %f %f\n", wp2->latitude, wp2->longitude);
    //printf("%s\n", doc->creator);
    //char* temp = malloc(100);
    //strcpy(temp, "test");
    //Waypoint *wp = JSONtoWaypoint(temp);
    //printf("Creator: %s\n", doc->creator);
    //printf("Version: %f\n", new->version);
    //printf("Namespace: %s\n", new->namespace);
    deleteGPXdoc(doc);

}