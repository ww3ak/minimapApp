#include <iostream>
#include <vector>
#include <string>
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

int main() {
    string filename = "ride.gpx";  // Make sure this file exists
    vector<GPSPoint> ride = parseGPX(filename);

    for (const auto& pt : ride) {
        cout << "Lat: " << pt.latitude
             << ", Lon: " << pt.longitude
             << ", Elev: " << pt.elevation
             << ", Time: " << pt.timestamp << endl;
    }

    return 0;
}
