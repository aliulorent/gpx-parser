#include "GPXParser.h"

xmlDoc* createTree(GPXdoc* doc);

void dummyDeleteRoute(void* data);
void dummyDeleteTrack(void* data);
char* createGPXJSON(char* fileName, char* schema);
char* fileToRouteList(char* fileName);
char* fileToTrackList(char* fileName);
void* renameTrack(char* fileName, char* trackName, char* newName);
void renameRoute(char* fileName, char* routeName, char* newName);
void fileFromJSON(char* fileName, char* jsonString);
void writeAddRoute(char* fileName, char* routeString);
void writeAddWaypoint(char* fileName, char* routeName, char* wpString);
char *dataToJSON(const GPXData *data);
char* dataListToJSON(const List *list);
char *fileToDataList(char* fileName, char* name, char* type);
char *tracksBetweenList(char* fileName, char* schema, float latOne, float latTwo, float lonOne, float lonTwo, float delta);
char *routesBetweenList(char* fileName, char* schema, float latOne, float latTwo, float lonOne, float lonTwo, float delta);
char* wpToJSON(const Waypoint *wp);
char* wpListToJSON(const List *list);
char* fileRouteToWPList(char* fileName, char* routeName);