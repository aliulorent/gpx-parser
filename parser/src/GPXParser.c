#include"GPXParser.h"
/*
GPX File Parser and Utilities
Written By: Lorent Aliu
This code was written for a project for the course CIS2750 - Software Systems Development and Integration at the University of Guelph
*/

GPXdoc* createGPXdoc(char* fileName){

    /*Creating XML tree*/
    xmlDoc *doc = NULL;
    doc = xmlReadFile(fileName, NULL, 0);

    /*Checking if tree creation worked*/
    if(doc == NULL){
        return NULL;
    }

    /*Initializing GPX struct*/
    GPXdoc* gpx = (GPXdoc*)malloc(sizeof(GPXdoc));
    strcpy(gpx->namespace, "");
    gpx->version = 0.0;
    gpx->creator = malloc(sizeof(char*));
    strcpy(gpx->creator, "");
    gpx->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    gpx->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    gpx->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);

    /*Initializing Root node*/
    xmlNode *rootNode = NULL;
    rootNode = xmlDocGetRootElement(doc);

    /*Getting GPX Namespace*/
    if(rootNode->ns->href!=NULL){
        strcpy(gpx->namespace, (char*)rootNode->ns->href);
    }

    /*Getting GPX version and Creator*/
    xmlAttr *currAttr = NULL;
    char *eptr;
    double vers;
    for(currAttr = rootNode->properties;currAttr !=NULL;currAttr = currAttr->next){
        if(strcmp((char*)currAttr->name, "version")==0){
            if(currAttr->children->content!=NULL){
                vers = strtod((char*)currAttr->children->content, &eptr);
                gpx->version = vers;
            }
        }
        if(strcmp((char*)currAttr->name, "creator")==0){
            if(currAttr->children->content!=NULL){
                free(gpx->creator);
                gpx->creator = (char*)malloc(sizeof(char*)*strlen((char*)currAttr->children->content)+1);
                strcpy(gpx->creator, (char*)currAttr->children->content);
            }
        }
    }
    
    xmlNode *currNode = NULL;
    xmlNode *tempNode = NULL;
    xmlNode *tempNode2 = NULL;
    xmlNode *tempNode3 = NULL;
    xmlAttr *tempAttr = NULL;
    double numba;
    int length;
    for(currNode = rootNode->children; currNode!=NULL; currNode = currNode->next){

        if(strcmp((char*)currNode->name, "wpt")==0){
            /***********************************************************************************************/
            /*Getting Waypoints*/
            tempNode = NULL;
            tempAttr = NULL;
            numba = 0;
            length = 0;
            /*Getting Waypoint lat & long*/
            Waypoint* wp = (Waypoint*)malloc(sizeof(Waypoint));
            wp->name = (char*)malloc(sizeof(char*));
            strcpy(wp->name, "");
            wp->longitude = 0;
            wp->latitude = 0;
            wp->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
            for(tempAttr = currNode->properties; tempAttr!=NULL; tempAttr = tempAttr->next){
                if(strcmp((char*)tempAttr->name, "lat")==0){
                    if(tempAttr->children->content!=NULL){
                        numba = strtod((char*)tempAttr->children->content, &eptr);
                        wp->latitude = numba;
                    }
                }
                if(strcmp((char*)tempAttr->name, "lon")==0){
                    if(tempAttr->children->content){
                        numba = strtod((char*)tempAttr->children->content, &eptr);
                        wp->longitude = numba;
                    }
                }
            }
            /*Getting Waypoint Name*/
            for(tempNode = currNode->children; tempNode!=NULL; tempNode = tempNode->next){
                if(strcmp((char*)tempNode->name, "name")==0){
                    if(tempNode->children->content!=NULL){
                        free(wp->name);
                        wp->name = (char*)malloc(sizeof(char*)*strlen((char*)tempNode->children->content)+1);
                        strcpy(wp->name, (char*)tempNode->children->content);
                    }
                }
                if((strcmp((char*)tempNode->name, "name")!=0) && (strcmp((char*)tempNode->name, "text")!=0) && tempNode!=NULL){
                    length = strlen(((char*)tempNode->children->content)+1);
                    GPXData* data = (GPXData*)malloc(sizeof(GPXData) + length * sizeof(char)+500);
                    strcpy(data->name, (char*)tempNode->name);
                    if(tempNode->children->content!=NULL){
                        strcpy(data->value, (char*)tempNode->children->content);
                    }
                    insertBack(wp->otherData, (void*)data);
                }
            }
            insertBack(gpx->waypoints, (void*)wp);

            /***********************************************************************************************/
        
        }
        /***********************************************************************************************/
        /*Getting Routes*/
        
        if(strcmp((char*)currNode->name, "rte")==0){

            tempNode = NULL;
            tempAttr = NULL;
            numba = 0;
            length = 0;
            Route* rou = (Route*)malloc(sizeof(Route));
            rou->name = (char*)malloc(sizeof(char*));
            strcpy(rou->name, "");
            rou->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
            rou->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
            /*Setting route name*/
            for(tempNode = currNode->children; tempNode!=NULL; tempNode = tempNode->next){
                if(strcmp((char*)tempNode->name, "name")==0){
                    if(tempNode->children->content!=NULL){
                        free(rou->name);
                        rou->name = (char*)malloc(sizeof(char*)*strlen((char*)tempNode->children->content)+1);
                        strcpy(rou->name, (char*)tempNode->children->content);
                    }
                }
                if((strcmp((char*)tempNode->name, "name")!=0) && (strcmp((char*)tempNode->name, "text")!=0) && (strcmp((char*)tempNode->name, "rtept")!=0) && tempNode!=NULL){
                    length = strlen(((char*)tempNode->children->content)+1);
                    GPXData* datart = (GPXData*)malloc(sizeof(GPXData) + length * sizeof(char) + 500);
                    strcpy(datart->name, (char*)tempNode->name);
                    if(tempNode->children->content!=NULL){
                        strcpy(datart->value, (char*)tempNode->children->content);
                    }
                    insertBack(rou->otherData, (void*)datart);
                }
                /*Setting up waypoints list within route*/
                if(strcmp((char*)tempNode->name, "rtept")==0){

                    Waypoint* wprt = (Waypoint*)malloc(sizeof(Waypoint));
                    wprt->name = (char*)malloc(sizeof(char*));
                    strcpy(wprt->name, "");
                    wprt->latitude = 0;
                    wprt->longitude = 0;
                    wprt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
                    /*Setting up lon and lat*/
                    for(tempAttr = tempNode->properties; tempAttr!=NULL; tempAttr = tempAttr->next){
                        if(strcmp((char*)tempAttr->name, "lat")==0){
                            if(tempAttr->children->content!=NULL){
                                numba = strtod((char*)tempAttr->children->content, &eptr);
                                wprt->latitude = numba;
                            }
                        }
                        if(strcmp((char*)tempAttr->name, "lon")==0){
                            if(tempAttr->children->content!=NULL){
                                numba = strtod((char*)tempAttr->children->content, &eptr);
                                wprt->longitude = numba;
                            }
                        }
                    }
                    if(tempNode->children!=NULL){
                        for(tempNode2 = tempNode->children;tempNode2!=NULL;tempNode2=tempNode2->next){
                            if(strcmp((char*)tempNode2->name, "name")==0){
                                if(tempNode2->children->content!=NULL){
                                    free(wprt->name);
                                    wprt->name = (char*)malloc(sizeof(char*)*strlen((char*)tempNode2->children->content)+1);
                                    strcpy(wprt->name, (char*)tempNode2->children->content);
                                }
                            }
                        }
                        for(tempNode2 = tempNode->children;tempNode2!=NULL;tempNode2=tempNode2->next){
                            if((strcmp((char*)tempNode2->name, "name")!=0) && (strcmp((char*)tempNode2->name, "text")!=0) && (strcmp((char*)tempNode2->name, "rtept")!=0) && tempNode2!=NULL){
                            length = strlen(((char*)tempNode2->children->content)+1);
                            GPXData* datartwp = (GPXData*)malloc(sizeof(GPXData) + length * sizeof(char) + 500);
                            strcpy(datartwp->name, (char*)tempNode2->name);
                            if(tempNode2->children->content!=NULL){
                                strcpy(datartwp->value, (char*)tempNode2->children->content);
                            }
                            insertBack(wprt->otherData, (void*)datartwp);
                            }
                        }
                    }
                    insertBack(rou->waypoints, (void*)wprt);
                }
            }
            insertBack(gpx->routes, (void*)rou);
        }
        /***********************************************************************************************/
        /*Getting Tracks*/
        if(strcmp((char*)currNode->name, "trk")==0){
            tempNode = NULL;
            tempAttr = NULL;
            numba = 0;
            Track* trk = (Track*)malloc(sizeof(Track));
            trk->name = (char*)malloc(sizeof(char*));
            strcpy(trk->name, "");
            trk->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
            trk->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
            for(tempNode = currNode->children; tempNode!=NULL; tempNode = tempNode->next){
                if(strcmp((char*)tempNode->name, "name")==0){
                    if(tempNode->children->content!=NULL){
                        free(trk->name);
                        trk->name = (char*)malloc(sizeof(char*)*strlen((char*)tempNode->children->content)+1);
                        strcpy(trk->name, (char*)tempNode->children->content);
                    }
                }
                if((strcmp((char*)tempNode->name, "name")!=0) && (strcmp((char*)tempNode->name, "text")!=0) && (strcmp((char*)tempNode->name, "trkseg")!=0) && tempNode!=NULL){
                    
                    length = strlen(((char*)tempNode->children->content)+1);
                    GPXData* datatr = (GPXData*)malloc(sizeof(GPXData) + length * sizeof(char)+500);
                    strcpy(datatr->name, (char*)tempNode->name);
                    if(tempNode->children->content!=NULL){
                        strcpy(datatr->value, (char*)tempNode->children->content);
                    }
                    insertBack(trk->otherData, (void*)datatr);
                }
                if(strcmp((char*)tempNode->name, "trkseg")==0){
                    
                    TrackSegment* trksg = (TrackSegment*)malloc(sizeof(TrackSegment));
                    trksg->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
                    for(tempNode2 = tempNode->children;tempNode2!=NULL;tempNode2=tempNode2->next){
                        if(strcmp((char*)tempNode2->name, "trkpt")==0){
                            Waypoint* wptrk = (Waypoint*)malloc(sizeof(Waypoint));
                            wptrk->name = (char*)malloc(sizeof(char*));
                            strcpy(wptrk->name, "");
                            wptrk->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
                            for(tempAttr = tempNode2->properties; tempAttr!=NULL; tempAttr = tempAttr->next){
                                if(strcmp((char*)tempAttr->name, "lat")==0){
                                    if(tempAttr->children->content!=NULL){
                                        numba = strtod((char*)tempAttr->children->content, &eptr);
                                        wptrk->latitude = numba;
                                    }
                                }
                                if(strcmp((char*)tempAttr->name, "lon")==0){
                                    if(tempAttr->children->content!=NULL){
                                        numba = strtod((char*)tempAttr->children->content, &eptr);
                                        wptrk->longitude = numba;
                                    }
                                }
                            }
                            if(tempNode2->children!=NULL){
                                for(tempNode3 = tempNode2->children;tempNode3!=NULL;tempNode3=tempNode3->next){
                                    if(strcmp((char*)tempNode3->name, "name")==0){
                                        if(tempNode3->children->content!=NULL){
                                            free(wptrk->name);
                                            wptrk->name = (char*)malloc(sizeof(char*)*strlen((char*)tempNode3->children->content)+1);
                                            strcpy(wptrk->name, (char*)tempNode3->children->content);
                                        }
                                    }
                                }
                                for(tempNode3 = tempNode2->children;tempNode3!=NULL;tempNode3=tempNode3->next){
                                    if((strcmp((char*)tempNode3->name, "name")!=0) && (strcmp((char*)tempNode3->name, "text")!=0) && (strcmp((char*)tempNode3->name, "rtept")!=0) && tempNode3!=NULL){
                                        length = strlen(((char*)tempNode3->children->content)+1);
                                        GPXData* datatrwp = (GPXData*)malloc(sizeof(GPXData) + length * sizeof(char) + 500);
                                        strcpy(datatrwp->name, (char*)tempNode3->name);
                                        if(tempNode3->children->content!=NULL){
                                            strcpy(datatrwp->value, (char*)tempNode3->children->content);
                                        }
                                        insertBack(wptrk->otherData, (void*)datatrwp);
                                        }
                                }
                            }
                            insertBack(trksg->waypoints, (void*)wptrk);
                        }
                    }
                    insertBack(trk->segments, (void*)trksg);
                }

            }
            insertBack(gpx->tracks, (void*)trk);
        }
    }

    /*Freeing the XML tree*/
    xmlFreeDoc(doc);
    xmlCleanupParser();

    return gpx;
}
void deleteGPXdoc(GPXdoc* doc){

    freeList(doc->waypoints);
    freeList(doc->routes);
    freeList(doc->tracks);
    free(doc->creator);
    free(doc);

    return;
}
char* GPXdocToString(GPXdoc* doc){

    char* tempStr;
    GPXdoc* gpx = doc;
    int len;
    if(doc == NULL){
        return NULL;
    }
    len = strlen(gpx->creator) + strlen(gpx->namespace) + strlen(gpx->waypoints->printData(getFromBack(gpx->waypoints))) + strlen(gpx->routes->printData(getFromBack(gpx->routes))) + strlen(gpx->tracks->printData(getFromBack(gpx->tracks)))+ 200;
    tempStr = (char*)malloc(sizeof(char)*len + sizeof(double) +1);
    sprintf(tempStr, "Creator: %s\nVersion: %lf\nNamespace: %s\n", gpx->creator, gpx->version, gpx->namespace);
    return tempStr;

}
char* waypointToString( void* data){
    
    char* tempStr;
    Waypoint* tempWp;
    int len;
    if(data == NULL){
        return NULL;
    }
    tempWp = (Waypoint*)data;
    len = strlen(tempWp->name);
    tempStr = (char*)malloc(sizeof(char)*len + sizeof(double) + 15 * sizeof(char));
    sprintf(tempStr, "%s : Lat: %lf Lon: %lf", tempWp->name, tempWp->latitude, tempWp->longitude);
    return tempStr;
}
char* routeToString( void* data){
    
    char* tempStr;
    Route* tempRoute;
    int len;
    if(data == NULL){
        return NULL;
    }
    tempRoute = (Route*)data;
    len = strlen(tempRoute->name);
    tempStr = (char*)malloc(sizeof(char)*len);
    sprintf(tempStr, "%s", tempRoute->name);
    return tempStr;
}
char* trackToString(void* data){

    char* tempStr;
    Track* tempTrack;
    int len;
    if(data == NULL){
        return NULL;
    }
    tempTrack = (Track*)data;
    len = strlen(tempTrack->name);
    tempStr = (char*)malloc(sizeof(char)*len);
    sprintf(tempStr, "%s", tempTrack->name);
    return tempStr;

}
char* trackSegmentToString(void* data){

    char* tempStr;
    TrackSegment* tempSg;
    int len;
    if(data == NULL){
        return NULL;
    }
    tempSg = (TrackSegment*)data;
    len = strlen(tempSg->waypoints->printData(getFromBack(tempSg->waypoints)));
    tempStr = (char*)malloc(sizeof(char) * len);
    sprintf(tempStr, "%s" , tempSg->waypoints->printData(getFromBack(tempSg->waypoints)));
    return tempStr;

}
char* gpxDataToString( void* data){
    char* tempStr;
    GPXData* tempData;
    int len;
    if(data==NULL){
        return NULL;
    }
    tempData = (GPXData*)data;
    len = strlen(tempData->name);
    tempStr = (char*)malloc(sizeof(char)*len);
    sprintf(tempStr, "%s", tempData->name);
    return tempStr;
}
void deleteWaypoint(void* data){
    Waypoint* tempWp;
    if(data == NULL){
        return;
    }
    tempWp = (Waypoint*)data;
    free(tempWp->name);
    freeList(tempWp->otherData);
    free(tempWp);
}
void deleteRoute(void* data){
    Route* tempRoute;
    if(data == NULL){
        return;
    }
    tempRoute = (Route*)data;
    freeList(tempRoute->waypoints);
    freeList(tempRoute->otherData);
    free(tempRoute->name);
    free(tempRoute);
}
void dummyDeleteRoute(void* data){


}
void deleteTrack(void* data){
    Track* tempTrack;
    if(data == NULL){
        return;
    }
    tempTrack = (Track*)data;
    free(tempTrack->name);
    freeList(tempTrack->segments);
    freeList(tempTrack->otherData);
    free(tempTrack);
}
void dummyDeleteTrack(void* data){


}
void deleteTrackSegment(void* data){

    TrackSegment* tempSg;
    if(data==NULL){
        return;
    }
    tempSg = (TrackSegment*)data;
    freeList(tempSg->waypoints);
    free(tempSg);
    
}
void deleteGpxData( void* data){
    GPXData* tempData;
    if(data == NULL){
        return;
    }
    tempData = (GPXData*)data;
    free(tempData);
}

int compareGpxData(const void *first, const void *second){
    return 0;
}
int compareWaypoints(const void *first, const void *second){
    return 0;
}
int compareRoutes(const void *first, const void *second){
    return 0;
}
int compareTrackSegments(const void *first, const void *second){
    return 0;
}
int compareTracks(const void *first, const void *second){
    return 0;
}
int getNumWaypoints(const GPXdoc* doc){

    int count = 0;
    if(doc==NULL){
        return 0;
    }
    ListIterator list = createIterator(doc->waypoints);
    while(nextElement(&list) != NULL){
        count++;
    }
    return count;
}
int getNumRoutes(const GPXdoc* doc){
    
    int count = 0;
    if(doc==NULL){
        return 0;
    }
    ListIterator list = createIterator(doc->routes);
    while(nextElement(&list) != NULL){
        count++;
    }
    return count;
}
int getNumTracks(const GPXdoc* doc){
    int count = 0;
    if(doc==NULL){
        return 0;
    }
    ListIterator list = createIterator(doc->tracks);
    while(nextElement(&list) != NULL){
        count++;
    }
    return count;
}
int getNumSegments(const GPXdoc* doc){
    int count = 0;
    if(doc==NULL){
        return 0;
    }
    void* node;
    ListIterator list = createIterator(doc->tracks);
    while((node = nextElement(&list)) != NULL) {

        Track* track = (Track*)node;
        ListIterator list2 = createIterator(track->segments);
        while(nextElement(&list2)!=NULL){
            count++;
        }
    }
    return count;
}
int getNumGPXData(const GPXdoc* doc){
    int count = 0;
    if(doc==NULL){
        return 0;
    }
    /*Start with data from Waypoints list*/
    void* node;
    void* node2;
    void* node3;
    ListIterator list = createIterator(doc->waypoints);
    while((node = nextElement(&list)) != NULL) {

        Waypoint* wp = (Waypoint*)node;
        ListIterator list2 = createIterator(wp->otherData);
        while(nextElement(&list2)!=NULL){
            count++;
        }
        if(wp->name!=NULL && strlen(wp->name)>0){
            count++;
        }
    }
    ListIterator list3 = createIterator(doc->routes);
    while((node = nextElement(&list3)) != NULL) {

        Route* rt = (Route*)node;
        ListIterator list4 = createIterator(rt->otherData);
        while(nextElement(&list4)!=NULL){
            count++;
        }
        ListIterator list5 = createIterator(rt->waypoints);
        while((node2 = nextElement(&list5))!=NULL){
            Waypoint* wp2 = (Waypoint*)node2;
            if(wp2->name!=NULL && strlen(wp2->name)>0){
                count++;
            }
            ListIterator list6 = createIterator(wp2->otherData);
            while(nextElement(&list6)!=NULL){
            count++;
            }
        }
        if(rt->name!=NULL && strlen(rt->name)>0){
            count++;
        }
    }
    ListIterator list7 = createIterator(doc->tracks);
    while((node = nextElement(&list7)) != NULL) {

        Track* tr = (Track*)node;
        ListIterator list8 = createIterator(tr->otherData);
        while(nextElement(&list8)!=NULL){
            count++;
        }
        ListIterator list9 = createIterator(tr->segments);
        while((node2 = nextElement(&list9))!=NULL){
            TrackSegment* seg = (TrackSegment*)node2;
            ListIterator list10 = createIterator(seg->waypoints);
            while((node3 = nextElement(&list10))!=NULL){
                Waypoint* wp3 = (Waypoint*)node3;
                ListIterator list11 = createIterator(wp3->otherData);
                while(nextElement(&list11)!=NULL){
                    count++;
                }
                if(wp3->name!=NULL && strlen(wp3->name)>0){
                    count++;
                }
            }
        }


        if(tr->name!=NULL && strlen(tr->name)>0){
            count++;
        }
    }


    return count;
}
Waypoint* getWaypoint(const GPXdoc* doc, char* name){

    if(doc==NULL || name == NULL){
        return NULL;
    }
    void* node;
    ListIterator list = createIterator(doc->waypoints);
    while((node = nextElement(&list))!=NULL){
        Waypoint* wp = (Waypoint*)node;
        if(strcmp(wp->name, name)==0){
            return wp;
        }
    }
    return NULL;
}
Track* getTrack(const GPXdoc* doc, char* name){

    if(doc==NULL || name == NULL){
        return NULL;
    }
    void* node;
    ListIterator list = createIterator(doc->tracks);
    while((node = nextElement(&list))!=NULL){
        Track* track = (Track*)node;
        if(strcmp(track->name, name)==0){
            return track;
        }
    }
    return NULL;
}
Route* getRoute(const GPXdoc* doc, char* name){
    
    if(doc==NULL || name == NULL){
        return NULL;
    }
    void* node;
    ListIterator list = createIterator(doc->routes);
    while((node = nextElement(&list))!=NULL){
        Route* route = (Route*)node;
        if(strcmp(route->name, name)==0){
            return route;
        }
    }
    return NULL;
}
GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile){

    if(fileName==NULL){
        return NULL;
    }
    if(gpxSchemaFile==NULL){
        return NULL;
    }

    xmlSchemaParserCtxtPtr ctxt;
    xmlSchemaPtr schema = NULL;
    xmlDocPtr docu;
    GPXdoc* gpx = NULL;

    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);

    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    docu = xmlReadFile(fileName, NULL, 0);
    if(docu==NULL){
        xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
        return NULL;
    }
    else{
        xmlSchemaValidCtxtPtr ctxt;
        int ret;
        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, docu);
        if(ret == 0){
            /*Success*/
            gpx = createGPXdoc(fileName);
        }
        xmlFreeDoc(docu);
        xmlSchemaFreeValidCtxt(ctxt);
    }

    if(schema!=NULL){
        xmlSchemaFree(schema);
    }
    
    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();
    
    return gpx;
}
xmlDoc* createTree(GPXdoc* gpx){

    xmlDocPtr doc = NULL;
    doc = xmlNewDoc(BAD_CAST "1.0");
    xmlNodePtr root = NULL, node = NULL, node2 = NULL, node3 = NULL;
    root = xmlNewNode(NULL, BAD_CAST "gpx");
    xmlDocSetRootElement(doc, root);

    /*Setting up Namespace first*/
    if(gpx->namespace!=NULL && strcmp(gpx->namespace, "")!=0){
        xmlNs* ns = xmlNewNs(NULL, BAD_CAST gpx->namespace, NULL);
        xmlSetNs(root, ns);
        root->nsDef = ns;
    }
    /*Setting version & creator*/
    char buffer[50];
    sprintf(buffer, "%g", gpx->version);
    xmlNewProp(root, BAD_CAST "creator", BAD_CAST gpx->creator);
    xmlNewProp(root, BAD_CAST "version", BAD_CAST buffer);

    /*Setting Waypoints*/
    ListIterator wplist = createIterator(gpx->waypoints);
    void* data;
    void* data2;
    void* data3;
    void* data4;
    while((data = nextElement(&wplist))!=NULL){
        Waypoint* wp = (Waypoint*)data;
        node = xmlNewChild(root, NULL, BAD_CAST "wpt", NULL);
        sprintf(buffer, "%f", wp->latitude);
        xmlNewProp(node, BAD_CAST "lat", BAD_CAST buffer);
        sprintf(buffer, "%f", wp->longitude);
        xmlNewProp(node, BAD_CAST "lon", BAD_CAST buffer);
        if(strcmp(wp->name, "")!=0){
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST wp->name);
        }
        ListIterator otherlist = createIterator(wp->otherData);
        while((data2 = nextElement(&otherlist))!=NULL){
            GPXData* extra = (GPXData*)data2;
            xmlNewChild(node, NULL, BAD_CAST extra->name, BAD_CAST extra->value);
        }
    }
    /*Setting Routes*/
    ListIterator rtlist = createIterator(gpx->routes);
    while((data = nextElement(&rtlist))!=NULL){
        Route* rt = (Route*)data;
        node = xmlNewChild(root, NULL, BAD_CAST "rte", NULL);
        if(strcmp(rt->name, "")!=0){
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST rt->name);
        }
        ListIterator otherlist = createIterator(rt->otherData);
        while((data2 = nextElement(&otherlist))!=NULL){
            GPXData* extra = (GPXData*)data2;
            xmlNewChild(node, NULL, BAD_CAST extra->name, BAD_CAST extra->value);
        }
        ListIterator wplist = createIterator(rt->waypoints);
        while((data2 = nextElement(&wplist))!=NULL){
            Waypoint* wp = (Waypoint*)data2;
            node2 = xmlNewChild(node, NULL, BAD_CAST "rtept", NULL);
            sprintf(buffer, "%f", wp->latitude);
            xmlNewProp(node2, BAD_CAST "lat", BAD_CAST buffer);
            sprintf(buffer, "%f", wp->longitude);
            xmlNewProp(node2, BAD_CAST "lon", BAD_CAST buffer);
            if(strcmp(wp->name, "")!=0){
                xmlNewChild(node2, NULL, BAD_CAST "name", BAD_CAST wp->name);
            }
            ListIterator otherlist = createIterator(wp->otherData);
            while((data3 = nextElement(&otherlist))!=NULL){
                GPXData* extra = (GPXData*)data3;
                xmlNewChild(node2, NULL, BAD_CAST extra->name, BAD_CAST extra->value);
            }
        }
    }
    /*Setting Tracks*/
    
    ListIterator trklist = createIterator(gpx->tracks);
    while((data = nextElement(&trklist))!=NULL){
        Track* trk = (Track*)data;
        node = xmlNewChild(root, NULL, BAD_CAST "trk", NULL);
        if(strcmp(trk->name, "")!=0){
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST trk->name);
        }
        ListIterator otherlist = createIterator(trk->otherData);
        while((data2 = nextElement(&otherlist))!=NULL){
            GPXData* extra = (GPXData*)data2;
            xmlNewChild(node, NULL, BAD_CAST extra->name, BAD_CAST extra->value);
        }
        
        ListIterator seglist = createIterator(trk->segments);
        while((data2 = nextElement(&seglist))!=NULL){
            TrackSegment* seg = (TrackSegment*)data2;
            node2 = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
            
            ListIterator wplist = createIterator(seg->waypoints);
            while((data3 = nextElement(&wplist))!=NULL){
                Waypoint* wp = (Waypoint*)data3;
                node3 = xmlNewChild(node2, NULL, BAD_CAST "trkpt", NULL);
                sprintf(buffer, "%f", wp->latitude);
                xmlNewProp(node3, BAD_CAST "lat", BAD_CAST buffer);
                sprintf(buffer, "%f", wp->longitude);
                xmlNewProp(node3, BAD_CAST "lon", BAD_CAST buffer);
                if(strcmp(wp->name, "")!=0){
                    xmlNewChild(node3, NULL, BAD_CAST "name", BAD_CAST wp->name);
                }
                ListIterator otherlist = createIterator(wp->otherData);
                while((data4 = nextElement(&otherlist))!=NULL){
                    GPXData* extra = (GPXData*)data4;
                    xmlNewChild(node2, NULL, BAD_CAST extra->name, BAD_CAST extra->value);
                }
            }
        }
    }
    return doc;
}
bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile){

    bool answer = false;

    if(doc==NULL){
        return false;
    }
    if(gpxSchemaFile==NULL || strlen(gpxSchemaFile)<1){
        return false;
    }
    if(strlen(doc->namespace)<1){
        return false;
    }
    if(doc->creator==NULL || strlen(doc->creator)<1){
        return false;
    }
    if(doc->version < 0){
        return false;
    }
    if(doc->waypoints==NULL){
        return false;
    }
    if(doc->routes==NULL){
        return false;
    }
    if(doc->tracks==NULL){
        return false;
    }
    
    xmlDoc* xml = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    xmlSchemaPtr schema = NULL;

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);

    xml = createTree(doc);

    if(xml==NULL){
        xmlSchemaFree(schema);
        xmlSchemaCleanupTypes();
        xmlCleanupParser();
        xmlMemoryDump();
        return false;
    }
    else{
        xmlSchemaValidCtxtPtr ctxt;
        int ret;
        ctxt = xmlSchemaNewValidCtxt(schema);
        ret = xmlSchemaValidateDoc(ctxt, xml);
        if(ret == 0){
            answer = true;
        }
        xmlSchemaFreeValidCtxt(ctxt);
        xmlFreeDoc(xml);
    }
    if(schema!=NULL){
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    return answer;
}
bool writeGPXdoc(GPXdoc* doc, char* fileName){

    if(doc==NULL){
        return false;
    }
    if(fileName==NULL || strlen(fileName)<1){
        return false;
    }
    xmlDoc* xml = createTree(doc);
    if(xml==NULL){
        return false;
    }
    if(xmlSaveFormatFileEnc(fileName, xml, NULL, 1)==-1){

        xmlFreeDoc(xml);
        xmlCleanupParser();
        xmlMemoryDump();
        return false;
    }
    else{
        xmlFreeDoc(xml);
        xmlCleanupParser();
        xmlMemoryDump();
        return true;
    }
}
float round10(float len){

    float input = len;
    int temp = (input + 5) / 10;
    temp = temp * 10;
    float result = (float)temp;
    
    return result;
}
float getRouteLen(const Route *rt){

    if(rt==NULL){
        return 0;
    }
    float result;
    float lat1, lat2, lon1, lon2;
    void* data2;
    void* data;
    float r = 6371000;
    ListIterator waypoints = createIterator(rt->waypoints);
    if((data2 = nextElement(&waypoints))!=NULL){
        Waypoint* wp1 = (Waypoint*)data2;
        lat1 = wp1->latitude;
        lon1 = wp1->longitude;
    }
    while((data = nextElement(&waypoints))!=NULL){
        Waypoint* wp = (Waypoint*)data;
        lat2 = wp->latitude;
        lon2 = wp->longitude;

        float delta1 = lat1 * (M_PI/180);
        float delta2 = lat2 * (M_PI/180);

        float latDif = (lat2-lat1) *(M_PI/180);
        float lonDif = (lon2-lon1) *(M_PI/180);

        float a = sin(latDif/2) * sin(latDif/2) + cos(delta1) * cos(delta2) * sin(lonDif/2) * sin(lonDif/2);
        float c = 2 * atan2(sqrt(a), sqrt(1-a));
        float d = r * c;
        result = result + d;
        lat1 = lat2;
        lon1 = lon2;
    }
    return result;
}
float getTrackLen(const Track *tr){

    if(tr==NULL){
        return 0;
    }
    float result;
    float lat1 = 0, lat2 = 0, lon1, lon2;
    void* data2;
    void* data;
    float r = 6371000;
    int flag = 0;
    ListIterator segments = createIterator(tr->segments);
    while((data = nextElement(&segments))!=NULL){
        TrackSegment* seg = (TrackSegment*)data;
        ListIterator waypoints = createIterator(seg->waypoints);
        while((data2 = nextElement(&waypoints))!=NULL){
            Waypoint* wp = (Waypoint*)data2;
            lat2 = wp->latitude;
            lon2 = wp->longitude;

            float delta1 = lat1 * (M_PI/180);
            float delta2 = lat2 * (M_PI/180);

            float latDif = (lat2-lat1) *(M_PI/180);
            float lonDif = (lon2-lon1) *(M_PI/180);

            float a = sin(latDif/2) * sin(latDif/2) + cos(delta1) * cos(delta2) * sin(lonDif/2) * sin(lonDif/2);
            float c = 2 * atan2(sqrt(a), sqrt(1-a));
            float d = r * c;
            if(flag!=0){
                result = result + d;
            }
            lat1 = lat2;
            lon1 = lon2;
            flag = 1;
        }
    }
    return result;
}
int numRoutesWithLength(const GPXdoc* doc, float len, float delta){
    
    if(doc==NULL){
        return 0;
    }
    if(len < 0 || delta < 0){
        return 0;
    }
    int number = 0;
    void* data;
    float length;
    ListIterator routes = createIterator(doc->routes);
    while((data = nextElement(&routes))!=NULL){

        Route* rt = (Route*)data;
        length = getRouteLen(rt);
        if(abs(length - len) <= delta){
            number++;
        }
    }
    return number;
}
int numTracksWithLength(const GPXdoc* doc, float len, float delta){
    
    if(doc==NULL){
        return 0;
    }
    if(len < 0 || delta < 0){
        return 0;
    }
    int number = 0;
    void* data;
    float length;
    ListIterator tracks = createIterator(doc->tracks);
    while((data = nextElement(&tracks))!=NULL){
        Track* trk = (Track*)data;
        length = getTrackLen(trk);
        if(abs(length - len) <= delta){
            number++;
        }
    }
    return number;
}
bool isLoopRoute(const Route* route, float delta){
    int num = 0;
    void* data;
    if(route == NULL){
        return false;
    }
    if(delta < 0){
        return false;
    }
    if(route->waypoints == NULL){
        return false;
    }
    ListIterator waypoints = createIterator(route->waypoints);
    while((data = nextElement(&waypoints))!=NULL){
        num++;
    }
    if (num<4){
        return false;
    }
    void* data2;
    void* data3;
    float r = 6371000;
    data2 = getFromFront(route->waypoints);
    data3 = getFromBack(route->waypoints);
    
    Waypoint* wp = (Waypoint*)data2;
    Waypoint* wp2 = (Waypoint*)data3;
    float lat1 = wp->latitude;
    float lon1 = wp->longitude;
    float lat2 = wp2->latitude;
    float lon2 = wp2->longitude;

    float delta1 = lat1 * (M_PI/180);
    float delta2 = lat2 * (M_PI/180);

    float latDif = (lat2-lat1) *(M_PI/180);
    float lonDif = (lon2-lon1) *(M_PI/180);

    float a = sin(latDif/2) * sin(latDif/2) + cos(delta1) * cos(delta2) * sin(lonDif/2) * sin(lonDif/2);
    float c = 2 * atan2(sqrt(a), sqrt(1-a));
    float d = r * c;
    
    if(d <= delta){
        return true;
    }
    else{
        return false;
    }
}
bool isLoopTrack(const Track *tr, float delta){
    
    if(tr == NULL){
        return false;
    }
    if(delta < 0){
        return false;
    }
    if(tr->segments == NULL){
        return false;
    }
    int num = 0;
    void* data;
    void* dataa;
    ListIterator tracks = createIterator(tr->segments);
    while((data = nextElement(&tracks))!=NULL){
        TrackSegment* seg = (TrackSegment*)data;
        ListIterator segments = createIterator(seg->waypoints);
        while((dataa = nextElement(&segments))!=NULL){
            num++;
        }
    }
    if (num<4){
        return false;
    }
    void* data2;
    void* data3;
    void* data4;
    void* data5;
    float r = 6371000;
    data2 = getFromFront(tr->segments);
    data3 = getFromBack(tr->segments);
    TrackSegment* seg1 = (TrackSegment*)data2;
    TrackSegment* seg2 = (TrackSegment*)data3;
    data4 = getFromFront(seg1->waypoints);
    data5 = getFromBack(seg2->waypoints);
    
    Waypoint* wp = (Waypoint*)data4;
    Waypoint* wp2 = (Waypoint*)data5;
    float lat1 = wp->latitude;
    float lon1 = wp->longitude;
    float lat2 = wp2->latitude;
    float lon2 = wp2->longitude;

    float delta1 = lat1 * (M_PI/180);
    float delta2 = lat2 * (M_PI/180);

    float latDif = (lat2-lat1) *(M_PI/180);
    float lonDif = (lon2-lon1) *(M_PI/180);

    float a = sin(latDif/2) * sin(latDif/2) + cos(delta1) * cos(delta2) * sin(lonDif/2) * sin(lonDif/2);
    float c = 2 * atan2(sqrt(a), sqrt(1-a));
    float d = r * c;
    
    if(d <= delta){
        return true;
    }
    else{
        return false;
    }

}
List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

    if(doc==NULL){
        return NULL;
    }
    List* routeList = initializeList(&routeToString, &dummyDeleteRoute, &compareRoutes);
    ListIterator routes = createIterator(doc->routes);
    int count = 0;
    void* data;
    void* data2;
    void* data3;
    float r = 6371000;
    while((data = nextElement(&routes))!=NULL){
        Route* rt = (Route*)data;
        data2 = getFromFront(rt->waypoints);
        data3 = getFromBack(rt->waypoints);
        //Source
        Waypoint* wp1 = (Waypoint*)data2;
        //Destination
        Waypoint* wp2 = (Waypoint*)data3;
        //COMPARING SOURCE FIRST
        float lat1 = sourceLat;
        float lon1 = sourceLong;
        float lat2 = wp1->latitude;
        float lon2 = wp1->longitude;

        float delta1 = lat1 * (M_PI/180);
        float delta2 = lat2 * (M_PI/180);

        float latDif = (lat2-lat1) *(M_PI/180);
        float lonDif = (lon2-lon1) *(M_PI/180);

        float a = sin(latDif/2) * sin(latDif/2) + cos(delta1) * cos(delta2) * sin(lonDif/2) * sin(lonDif/2);
        float c = 2 * atan2(sqrt(a), sqrt(1-a));
        float d = r * c;

        lat1 = wp2->latitude;
        lon1 = wp2->longitude;
        lat2 = destLat;
        lon2 = destLong;

        delta1 = lat1 * (M_PI/180);
        delta2 = lat2 * (M_PI/180);

        latDif = (lat2-lat1) *(M_PI/180);
        lonDif = (lon2-lon1) *(M_PI/180);

        a = sin(latDif/2) * sin(latDif/2) + cos(delta1) * cos(delta2) * sin(lonDif/2) * sin(lonDif/2);
        c = 2 * atan2(sqrt(a), sqrt(1-a));
        float d2 = r * c;

        if(d<= delta && d2 <= delta){
            insertBack(routeList, (void*)rt);
            count++;
        }
    }
    if(count==0){
        return NULL;
    }
    else{
        return routeList;
    }
}
List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta){

    if(doc==NULL){
        return NULL;
    }
    List* trackList = initializeList(&trackToString, &dummyDeleteTrack, &compareTracks);
    ListIterator tracks = createIterator(doc->tracks);
    void* data;
    void* data2;
    void* data3;
    void* data4;
    void* data5;
    int count =0;

    float r = 6371000;
    while((data = nextElement(&tracks))!=NULL){
        Track* trk = (Track*)data;
        data2 = getFromFront(trk->segments);
        TrackSegment* seg1 = (TrackSegment*)data2;
        data3 = getFromFront(seg1->waypoints);
        Waypoint* wp1 = (Waypoint*)data3;
        data4 = getFromBack(trk->segments);
        TrackSegment* seg2 = (TrackSegment*)data4;
        data5 = getFromBack(seg2->waypoints);
        Waypoint* wp2 = (Waypoint*)data5;

        float lat1 = sourceLat;
        float lon1 = sourceLong;
        float lat2 = wp1->latitude;
        float lon2 = wp1->longitude;

        float delta1 = lat1 * (M_PI/180);
        float delta2 = lat2 * (M_PI/180);

        float latDif = (lat2-lat1) *(M_PI/180);
        float lonDif = (lon2-lon1) *(M_PI/180);

        float a = sin(latDif/2) * sin(latDif/2) + cos(delta1) * cos(delta2) * sin(lonDif/2) * sin(lonDif/2);
        float c = 2 * atan2(sqrt(a), sqrt(1-a));
        float d = r * c;

        lat1 = wp2->latitude;
        lon1 = wp2->longitude;
        lat2 = destLat;
        lon2 = destLong;

        delta1 = lat1 * (M_PI/180);
        delta2 = lat2 * (M_PI/180);

        latDif = (lat2-lat1) *(M_PI/180);
        lonDif = (lon2-lon1) *(M_PI/180);

        a = sin(latDif/2) * sin(latDif/2) + cos(delta1) * cos(delta2) * sin(lonDif/2) * sin(lonDif/2);
        c = 2 * atan2(sqrt(a), sqrt(1-a));
        float d2 = r * c;

        if(d<= delta && d2 <= delta){
            insertBack(trackList, (void*)trk);
            count++;
        }
    }
    if(count==0){
        return NULL;
    }
    else{
        return trackList;
    }
}
char* trackToJSON(const Track *tr){

    if(tr == NULL){
        char* emptyString = malloc(sizeof(char)*2);
        emptyString = "{}";
        return emptyString;
    }
    int stringLength;
    char name[50];
    double length;
    char lengthStr[50];
    bool statusB;
    char status[50];
    int numPoints = 0;
    
    if(tr->name == NULL || strlen(tr->name)<1){
        strcpy(name, "None");
    }
    else{
        strcpy(name, tr->name);
    }
    length = round10(getTrackLen(tr));
    statusB = isLoopTrack(tr, 10.0);
    if(statusB==true){
        strcpy(status, "true");
    }
    if(statusB==false){
        strcpy(status, "false");
    }
    sprintf(lengthStr, "%.1f", length);
    void* data;
    ListIterator points = createIterator(tr->segments);
    while((data = nextElement(&points))!=NULL){
        TrackSegment* sg = (TrackSegment*)data;
        ListIterator points2 = createIterator(sg->waypoints);
        void *data2;
        while((data2 = nextElement(&points2))!=NULL){
            numPoints++;
        }
    }
    char numPStr[50];
    sprintf(numPStr, "%d", numPoints);
    stringLength = strlen(name) + strlen(lengthStr) + strlen(status) +strlen(numPStr) + 57;
    char* returnStr = malloc(sizeof(char)*stringLength);
    strcpy(returnStr, "{\"name\":\"");
    strcat(returnStr, name);
    strcat(returnStr, "\",\"numPoints\":");
    strcat(returnStr, numPStr);
    strcat(returnStr, ",\"len\":");
    strcat(returnStr, lengthStr);
    strcat(returnStr, ",\"loop\":");
    strcat(returnStr, status);
    strcat(returnStr, "}");

    return returnStr;
}
char* routeToJSON(const Route *rt){

    if(rt == NULL){
        char* emptyString = malloc(sizeof(char)*2);
        strcpy(emptyString, "{}");
        return emptyString;
    }
    int stringLength;
    char name[50];
    double length;
    char lengthStr[50];
    bool statusB;
    char status[50];
    int numPoints = 0;
    char numPStr[50];
    
    if(rt->name == NULL || strlen(rt->name)<1){
        strcpy(name, "None");
    }
    else{
        strcpy(name, rt->name);
    }
    length = round10(getRouteLen(rt));
    statusB = isLoopRoute(rt, 10.0);
    if(statusB==true){
        strcpy(status, "true");
    }
    if(statusB==false){
        strcpy(status, "false");
    }
    sprintf(lengthStr, "%.1f", length);
    void* data;
    ListIterator points = createIterator(rt->waypoints);
    while((data= nextElement(&points))!=NULL){
        numPoints++;
    }
    sprintf(numPStr, "%d", numPoints);
    stringLength = strlen(name) + strlen(lengthStr) + strlen(status) + strlen(numPStr) + 50;
    char* returnStr = malloc(sizeof(char)*stringLength);
    strcpy(returnStr, "{\"name\":\"");
    strcat(returnStr, name);
    strcat(returnStr, "\",\"numPoints\":");
    strcat(returnStr, numPStr);
    strcat(returnStr, ",\"len\":");
    strcat(returnStr, lengthStr);
    strcat(returnStr, ",\"loop\":");
    strcat(returnStr, status);
    strcat(returnStr, "}");

    return returnStr;
}

char* wpToJSON(const Waypoint *wp){

    if(wp == NULL){
        char* emptyString = malloc(sizeof(char)*2);
        strcpy(emptyString, "{}");
        return emptyString;
    }
    int stringLength;
    char name[50];
    char latitude[50];
    char longitude[50];
    
    if(wp->name == NULL || strlen(wp->name)<1){
        strcpy(name, "None");
    }
    else{
        strcpy(name, wp->name);
    }

    sprintf(latitude, "%f", wp->latitude);
    sprintf(longitude, "%f", wp->longitude);

    stringLength = strlen(name) + strlen(latitude) + strlen(longitude) + 50;
    char* returnStr = malloc(sizeof(char)*stringLength);
    strcpy(returnStr, "{\"name\":\"");
    strcat(returnStr, name);
    strcat(returnStr, "\",\"lat\":");
    strcat(returnStr, latitude);
    strcat(returnStr, ",\"lon\":");
    strcat(returnStr, longitude);
    strcat(returnStr, "}");

    return returnStr;
}

char* wpListToJSON(const List *list){
    
    if(list == NULL){
        char* empty = malloc(sizeof(char)*12);
        strcpy(empty, "[]");
        return empty;
    }
    void* data;
    char* returnStr = malloc(sizeof(char)*13);
    strcpy(returnStr, "[");
    int length;
    char* temp;
    int count = 0;
    ListIterator waypoints = createIterator((List*) list);
    while((data= nextElement(&waypoints))!=NULL){
        Waypoint* wp = (Waypoint*)data;
        temp = malloc(sizeof(char)* strlen(returnStr));
        strcpy(temp, returnStr);
        length = strlen(temp) + strlen(wpToJSON(wp)) + 100;
        free(returnStr);
        returnStr = malloc(sizeof(char) * length);
        strcpy(returnStr, temp);
        if(count!=0){
            strcat(returnStr, ",");
        }
        strcat(returnStr, wpToJSON(wp));
        free(temp);
        count = 1;
    }
    strcat(returnStr, "]");

    return returnStr;
}

char* routeListToJSON(const List *list){
    
    if(list == NULL){
        char* empty = malloc(sizeof(char)*12);
        strcpy(empty, "[]");
        return empty;
    }
    void* data;
    char* returnStr = malloc(sizeof(char)*13);
    strcpy(returnStr, "[");
    int length;
    char* temp;
    int count = 0;
    ListIterator routes = createIterator((List*) list);
    while((data= nextElement(&routes))!=NULL){
        Route* route = (Route*)data;
        temp = malloc(sizeof(char)* strlen(returnStr));
        strcpy(temp, returnStr);
        length = strlen(temp) + strlen(routeToJSON(route)) + 100;
        free(returnStr);
        returnStr = malloc(sizeof(char) * length);
        strcpy(returnStr, temp);
        if(count!=0){
            strcat(returnStr, ",");
        }
        strcat(returnStr, routeToJSON(route));
        free(temp);
        count = 1;
    }
    strcat(returnStr, "]");

    return returnStr;
}
char* trackListToJSON(const List *list){
    
    if(list == NULL){
        char* empty = malloc(sizeof(char)*2);
        strcpy(empty, "[]");
        return empty;
    }
    void* data;
    char* returnStr = malloc(sizeof(char)*3);
    strcpy(returnStr, "[");
    int length;
    char* temp;
    int count = 0;
    ListIterator tracks = createIterator((List*) list);
    while((data= nextElement(&tracks))!=NULL){
        Track* track = (Track*)data;
        temp = malloc(sizeof(char)* strlen(returnStr));
        strcpy(temp, returnStr);
        length = strlen(temp) + strlen(trackToJSON(track)) + 100;
        free(returnStr);
        returnStr = malloc(sizeof(char) * length);
        strcpy(returnStr, temp);
        if(count!=0){
            strcat(returnStr, ",");
        }
        strcat(returnStr, trackToJSON(track));
        free(temp);
        count = 1;
    }
    strcat(returnStr, "]");

    return returnStr;

}
char* GPXtoJSON(const GPXdoc* gpx){

    if(gpx == NULL){
        char* empty = malloc(sizeof(char)*2);
        strcpy(empty, "{}");
        return empty;
    }
    char create[50];
    strcpy(create, gpx->creator);
    char vers[50];
    sprintf(vers, "%.1f", gpx->version);

    int numWp = 0;
    int numRt = 0;
    int numTr = 0;
    int len;
    char wp[50];
    char rt[50];
    char tr[50];
    ListIterator waypoints = createIterator(gpx->waypoints);
    void* data1;
    while((data1 = nextElement(&waypoints))!=NULL){
        numWp++;
    }
    ListIterator routes = createIterator(gpx->routes);
    void* data2;
    while((data2 = nextElement(&routes))!=NULL){
        numRt++;
    }
    ListIterator tracks = createIterator(gpx->tracks);
    void* data3;
    while((data3 = nextElement(&tracks))!=NULL){
        numTr++;
    }
    sprintf(wp, "%d", numWp);
    sprintf(rt, "%d", numRt);
    sprintf(tr, "%d", numTr);

    len = strlen(create) + strlen(vers) + strlen(wp) + strlen(rt) + strlen(tr) + 69;
    char* returnStr = malloc(sizeof(char)*len);
    strcpy(returnStr, "{\"version\":");
    strcat(returnStr, vers);
    strcat(returnStr, ",\"creator\":\"");
    strcat(returnStr, create);
    strcat(returnStr, "\",\"numWaypoints\":");
    strcat(returnStr, wp);
    strcat(returnStr, ",\"numRoutes\":");
    strcat(returnStr, rt);
    strcat(returnStr, ",\"numTracks\":");
    strcat(returnStr, tr);
    strcat(returnStr, "}");

    return returnStr;
}
void addWaypoint(Route *rt, Waypoint *pt){

    if(rt == NULL || pt == NULL){
        return;
    }
    else{
        insertBack(rt->waypoints, (void*)pt);
        return;
    }
}
void addRoute(GPXdoc* doc, Route* rt){

    if(doc == NULL || rt == NULL){
        return;
    }
    else{
        insertBack(doc->routes, (void*)rt);
        return;
    }
}

GPXdoc* JSONtoGPX(const char* gpxString){
    
    if(gpxString==NULL){
        return NULL;
    }
    GPXdoc* gpx = (GPXdoc*)malloc(sizeof(GPXdoc));
    char* string = malloc(200);
    strcpy(string, gpxString);
    char* temp;
    temp = strtok(string, ":");
    char* eptr;
    double version = strtod((char*)strtok(NULL, ","), &eptr);
    temp = strtok(NULL, ":");
    char* creator;
    creator = strtok(NULL, "\"");
    gpx->creator = malloc(sizeof(char*) * strlen(creator));
    strcpy(gpx->creator, creator);
    gpx->version = version;
    free(string);
    strcpy(gpx->namespace, "http://www.topografix.com/GPX/1/1");
    gpx->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    gpx->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    gpx->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);
    eptr = temp;
    return gpx;
}

Waypoint* JSONtoWaypoint(const char* gpxString){
    
    if(gpxString==NULL){
        return NULL;
    }
    Waypoint *wp = malloc(sizeof(Waypoint));
    wp->name = malloc(5);
    strcpy(wp->name, "");
    char* string = malloc(200);
    strcpy(string, gpxString);
    char* temp;
    temp = strtok(string, ":");
    char* eptr;
    double lat = strtod((char*)strtok(NULL, ","), &eptr);
    temp = strtok(NULL, ":");
    double lon = strtod((char*)strtok(NULL, ","), &eptr);
    wp->latitude = lat;
    wp->longitude = lon;
    wp->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    eptr = temp;
    free(string);
    return wp;
}
Route* JSONtoRoute(const char* gpxString){

    if(gpxString==NULL){
        return NULL;
    }
    Route *rt = malloc(sizeof(Route));
    char* string = malloc(200);
    strcpy(string, gpxString);
    char* temp;
    temp = strtok(string, ":");
    char *name = strtok(NULL, "\"");
    rt->name = malloc(sizeof(char) * strlen(name) + 1);
    strcpy(rt->name, name);
    rt->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    rt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    string = temp;
    free(string);
    return rt;
    
}
char* createGPXJSON(char* fileName, char* schema){

    GPXdoc * doc = createValidGPXdoc(fileName, schema);

    char* JSONStr = GPXtoJSON(doc);
    if(doc!=NULL){
        deleteGPXdoc(doc);
    }
    return JSONStr;
}
char* fileToRouteList(char* fileName){

    GPXdoc *doc = createGPXdoc(fileName);
    char* returnStr = malloc(strlen(routeListToJSON(doc->routes)) + 100);
    strcpy(returnStr, routeListToJSON(doc->routes));
    return returnStr;
}
char* fileToTrackList(char* fileName){

    GPXdoc *doc = createGPXdoc(fileName);
    char* returnStr = trackListToJSON(doc->tracks);
    deleteGPXdoc(doc);
    return returnStr;
}

char* fileRouteToWPList(char* fileName, char* routeName){
    GPXdoc *doc  = createGPXdoc(fileName);
    ListIterator routes = createIterator(doc->routes);
    void* data;
    while((data = nextElement(&routes))!=NULL){
        Route* route = (Route*)data;
        if(strcmp(route->name, routeName)==0){
            char* returnStr = malloc(strlen(wpListToJSON(route->waypoints))+100);
            strcpy(returnStr, wpListToJSON(route->waypoints));
            return returnStr;
        }
    }
    char * tempStr = malloc(10);
    strcpy(tempStr, "[]");
    return tempStr;
}

void renameTrack(char* fileName, char* trackName, char* newName){

    GPXdoc *doc  = createGPXdoc(fileName);
    ListIterator tracks = createIterator(doc->tracks);
    void* data;
    while((data = nextElement(&tracks))!=NULL){
        Track* track = (Track*)data;
        if(strcmp(track->name, trackName)==0){
            free(track->name);
            track->name = malloc(strlen(newName));
            strcpy(track->name, newName);
            break;
        }
    }
    if(writeGPXdoc(doc, fileName)){
        deleteGPXdoc(doc);
        return;
    }
    else{
        deleteGPXdoc(doc);
        return;
    }
}
void renameRoute(char* fileName, char* routeName, char* newName){

    GPXdoc *doc  = createGPXdoc(fileName);
    ListIterator routes = createIterator(doc->routes);
    void* data;
    while((data = nextElement(&routes))!=NULL){
        Route* route = (Route*)data;
        if(strcmp(route->name, routeName)==0){
                free(route->name);
                route->name = malloc(strlen(newName));
                strcpy(route->name, newName);
                break;
        }
    }
    if(writeGPXdoc(doc, fileName)){
        deleteGPXdoc(doc);
        return;
    }
    else{
        deleteGPXdoc(doc);
        return;
    }
}
void fileFromJSON(char* fileName, char* jsonString){

    GPXdoc *doc = JSONtoGPX(jsonString);
    int result = writeGPXdoc(doc, fileName);

    if(result == 0){
        return;
    }
    deleteGPXdoc(doc);
    return;
}
void writeAddRoute(char* fileName, char* routeString){

    GPXdoc *doc = createGPXdoc(fileName);
    Route* route = JSONtoRoute(routeString);
    addRoute(doc, route);
    if(writeGPXdoc(doc, fileName)){
        return;
    }
    deleteGPXdoc(doc);
    return;
}
void writeAddWaypoint(char* fileName, char* routeName, char* wpString){

    GPXdoc *doc = createGPXdoc(fileName);
    Route *route = getRoute(doc, routeName);
    Waypoint *wp = JSONtoWaypoint(wpString);
    addWaypoint(route, wp);
    if(writeGPXdoc(doc, fileName)){
        return;
    }
    deleteGPXdoc(doc);
    return;
}
char *dataToJSON(const GPXData *data){


    if(data == NULL){
        char* emptyString = malloc(sizeof(char)*2);
        emptyString = "{}";
        return emptyString;
    }
    int stringLength;
    char name[100];
    if(data->name == NULL || strlen(data->name)<1){
        strcpy(name, "None");
    }
    else{
        strcpy(name, data->name);
    }
    char value[300];
    strcpy(value, data->value);
    stringLength = strlen(name) + strlen(value) + 50;

    char * returnStr = malloc(sizeof(char)* stringLength);

    strcpy(returnStr, "{\"name\":\"");
    strcat(returnStr, name);
    strcat(returnStr, "\",\"value\":\"");
    strcat(returnStr, value);
    strcat(returnStr, "\"}");

    return returnStr;
}
char* dataListToJSON(const List *list){

    if(list == NULL){
        char* empty = malloc(sizeof(char)*2);
        strcpy(empty, "[]");
        return empty;
    }
    void* data;
    char* returnStr = malloc(sizeof(char)*3);
    strcpy(returnStr, "[");
    int length;
    char* temp;
    int count = 0;
    ListIterator dataList = createIterator((List*) list);
    while((data= nextElement(&dataList))!=NULL){
        GPXData* gpxdata = (GPXData*)data;
        temp = malloc(sizeof(char)* strlen(returnStr));
        strcpy(temp, returnStr);
        length = strlen(temp) + strlen(dataToJSON(gpxdata)) + 100;
        free(returnStr);
        returnStr = malloc(sizeof(char) * length);
        strcpy(returnStr, temp);
        if(count!=0){
            strcat(returnStr, ",");
        }
        strcat(returnStr, dataToJSON(gpxdata));
        free(temp);
        count = 1;
    }
    strcat(returnStr, "]");

    return returnStr;
}
char *fileToDataList(char* fileName, char* name, char* type){

    GPXdoc *doc  = createGPXdoc(fileName);
    char* returnStr;
    if(strcmp(type, "track")==0){
        ListIterator tracks = createIterator(doc->tracks);
        void* data;
        while((data = nextElement(&tracks))!=NULL){
            Track* track = (Track*)data;
            if(strcmp(track->name, name)==0){
                returnStr = dataListToJSON(track->otherData);
                return returnStr;
                break;
            }
        }
    }

    if(strcmp(type, "route")==0){

        ListIterator routes = createIterator(doc->routes);
        void* data;
        while((data = nextElement(&routes))!=NULL){
            Route* route = (Route*)data;
            if(strcmp(route->name, name)==0){
                returnStr = dataListToJSON(route->otherData);
                return returnStr;
                break;
            }
        }
    }
    return "";
}
char *tracksBetweenList(char* fileName, char* schema, float latOne, float latTwo, float lonOne, float lonTwo, float delta){

    GPXdoc *doc = createValidGPXdoc(fileName, schema);
    if(doc==NULL){
        char* tempRet = malloc(10);
        strcpy(tempRet, "[]");
        return tempRet;
    }
    char *returnStr = trackListToJSON(getTracksBetween(doc, latOne, lonOne, latTwo, lonTwo, delta));
    deleteGPXdoc(doc);
    return returnStr;
}
char *routesBetweenList(char* fileName, char* schema, float latOne, float latTwo, float lonOne, float lonTwo, float delta){

    GPXdoc *doc = createValidGPXdoc(fileName, schema);
    if(doc==NULL){
        char* tempRet = malloc(10);
        strcpy(tempRet, "[]");
        return tempRet;
    }
    char *returnStr = routeListToJSON(getRoutesBetween(doc, latOne, lonOne, latTwo, lonTwo, delta));
    deleteGPXdoc(doc);
    return returnStr;
}