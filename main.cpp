#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include "external/tinyxml2/tinyxml2.h"

using namespace tinyxml2;
using namespace std;

struct GPSPoint {
    double latitude;
    double longitude;
    double elevation;
    string timestamp;
};

vector<GPSPoint> parseGPX(const string& filename) {
    vector<GPSPoint> points;
    XMLDocument doc;

    if (doc.LoadFile(filename.c_str()) != XML_SUCCESS) {
        cerr << "Failed to load GPX file!" << endl;
        return points;
    }

    XMLElement* root = doc.FirstChildElement("gpx");
    if (!root) return points;

    XMLElement* trk = root->FirstChildElement("trk");
    if (!trk) return points;

    XMLElement* trkseg = trk->FirstChildElement("trkseg");
    if (!trkseg) return points;

    for (XMLElement* trkpt = trkseg->FirstChildElement("trkpt"); trkpt; trkpt = trkpt->NextSiblingElement("trkpt")) {
        GPSPoint point;
        trkpt->QueryDoubleAttribute("lat", &point.latitude);
        trkpt->QueryDoubleAttribute("lon", &point.longitude);

        XMLElement* ele = trkpt->FirstChildElement("ele");
        if (ele && ele->GetText()) point.elevation = stod(ele->GetText());

        XMLElement* time = trkpt->FirstChildElement("time");
        if (time && time->GetText()) point.timestamp = time->GetText();

        points.push_back(point);
    }

    return points;
}

void saveHistory(const vector<GPSPoint>& points, const string& filename){
    // file pointer
    fstream fout;

    // opens an existing csv file or creates a new file.
    fout.open(filename, ios::out | ios::app);

    //write point info to file
    for (const auto& pt : points) {
        fout << pt.latitude << ", "
             << pt.longitude << ", "
             << pt.elevation << ", "
             << pt.timestamp<< ", "
             << "\n";
    }

}

vector<GPSPoint> loadHistory(const string& filename){
    //create new point vector for ride
    vector<GPSPoint> ride = parseGPX(filename);
    return ride;
}

//haversine function to calculate distance between two points
double haversine(double lat1, double lon1, double lat2, double lon2){

}

bool isPointExplored(const GPSPoint& pt, const vector<GPSPoint>& history){
//load points from history into vector (using loadHistory)
//compare point to points in history
//use haversine function 
//return 0 if not explored , 1 if explored
}




int main() {
    string rideName = "ride.gpx";  // Make sure this file exists
    //create points vector from parsing ride data
    vector<GPSPoint> ride = parseGPX(rideName);

    // history file name
    string filename = "history.csv"; 

    saveHistory(ride, filename);
    // for (const auto& pt : ride) {
    //     cout << "Lat: " << pt.latitude
    //          << ", Lon: " << pt.longitude
    //          << ", Elev: " << pt.elevation
    //          << ", Time: " << pt.timestamp << endl;
    // }

    return 0;
}
