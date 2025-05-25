#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include "external/tinyxml2/tinyxml2.h"

using namespace tinyxml2;
using namespace std;

// Earth's radius in meters
const double EARTH_RADIUS_METERS = 6371000.0; 

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
        cerr << "Failed to load GPX file: " << filename << endl;
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
        if (ele && ele->GetText()) {
            point.elevation = stod(ele->GetText());
        } else {
            point.elevation = 0.0;
        }

        XMLElement* time = trkpt->FirstChildElement("time");
        if (time && time->GetText()) {
            point.timestamp = time->GetText();
        }

        points.push_back(point);
    }

    return points;
}

void saveHistory(const vector<GPSPoint>& points, const string& filename) {
    fstream fout;
    fout.open(filename, ios::out | ios::app);
    
    if (!fout.is_open()) {
        cerr << "Failed to open history file for writing!" << endl;
        return;
    }

    for (const auto& pt : points) {
        fout << pt.latitude << "," 
             << pt.longitude << "," 
             << pt.elevation << "," 
             << pt.timestamp << "\n";
    }
    fout.close();
}

vector<GPSPoint> loadHistory(const string& filename) {
    vector<GPSPoint> history;
    ifstream fin(filename);
    
    if (!fin.is_open()) {
        cerr << "History file not found, starting with empty history." << endl;
        return history;
    }

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        
        stringstream ss(line);
        string item;
        GPSPoint point;
        
        // Parse CSV line
        if (getline(ss, item, ',')) point.latitude = stod(item);
        if (getline(ss, item, ',')) point.longitude = stod(item);
        if (getline(ss, item, ',')) point.elevation = stod(item);
        if (getline(ss, item, ',')) point.timestamp = item;
        
        history.push_back(point);
    }
    fin.close();
    return history;
}

double degreesToRadians(double degrees) {
    return degrees * 3.14159 / 180.0;
}

double haversineDistance(const GPSPoint& p1, const GPSPoint& p2) {
    double lat1 = degreesToRadians(p1.latitude);
    double lon1 = degreesToRadians(p1.longitude);
    double lat2 = degreesToRadians(p2.latitude);
    double lon2 = degreesToRadians(p2.longitude);

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double a = pow(sin(dLat / 2), 2) +
               cos(lat1) * cos(lat2) * pow(sin(dLon / 2), 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return EARTH_RADIUS_METERS * c;
}

bool isPointExplored(const GPSPoint& pt, const vector<GPSPoint>& history) {
    // Check if point is within 10 meters of any historical point
    for (const auto& histPt : history) {
        double dist = haversineDistance(pt, histPt);
        if (dist <= 10.0) {
            // Point has been explored
            return true;  
        }
    }
    // Point is new
    return false;  
}

int main() {
    string rideName = "ride.gpx"; 
    string testName = "test_ride.gpx";  
    string filename = "history.csv"; 

    // Parse current ride data
    vector<GPSPoint> ride = parseGPX(rideName);
    if (ride.empty()) {
        cerr << "No data found in " << rideName << endl;
        return 1;
    }

    // Save current ride to history 
    saveHistory(ride, filename);
    cout << "Saved " << ride.size() << " points to history." << endl;

    // Load complete history
    vector<GPSPoint> historyPoints = loadHistory(filename);
    cout << "Loaded " << historyPoints.size() << " points from history." << endl;

    // Analyze test ride
    vector<GPSPoint> test_ride = parseGPX(testName);
    if (test_ride.empty()) {
        cerr << "No data found in " << testName << endl;
        return 1;
    }

    // Initialize counters
    int totalPoints = 0;
    int exploredPoints = 0;
    int newPoints = 0;

    // check each point in test ride
    for (const auto& pt : test_ride) {
        bool explored = isPointExplored(pt, historyPoints);
        totalPoints++;
        
        if (explored) {
            exploredPoints++;
        } else {
            newPoints++;
        }
    }

    // Display results
    cout << "\n=== Ride Analysis ===" << endl;
    cout << "Total points analyzed: " << totalPoints << endl;
    cout << "Previously explored: " << exploredPoints << endl;
    cout << "New/unexplored: " << newPoints << endl;
    
    if (totalPoints > 0) {
        double explorationRate = (double)newPoints / totalPoints * 100.0;
        cout << "New exploration rate: " << explorationRate << "%" << endl;
    }

    return 0;
}