#include<iostream>
#include<string>
#include<vector>
using namespace std;

#define nullPoint addPoint(-99, -99)
#define debugmode true

void debug(string print) {
    if (debugmode) cout << print;
}

struct point {
    double x, y;
};
point addPoint(float x, float y) {
    point outPoint;
    outPoint.x = x;
    outPoint.y = y;
    return outPoint;
}

// outputs a point as a string in the following format: (x,y)
string printPoint(point p1) {
    return "(" + to_string(p1.x) + "," + to_string(p1.y) + ")";
}

#define nullLine addLine(nullPoint, nullPoint)

struct line {
    point p1 , p2;
};
line addLine(point a, point b) {
    line outLine;
    outLine.p1 = a;
    outLine.p2 = b;
    return outLine;
}

// outputs a line as a string in the following format: ((x1,y1),(x2,y2))
string printLine(line l1) {
    return "(" + printPoint(l1.p1) + "," + printPoint(l1.p2) + ")";
}

bool onSegment(point p, point q, point r) {
    return (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y));
}

// determines whether lines are colinear, counterclockwise, or clockwise in orientation
int orientation(point p, point q, point r) {
    int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);
    
    if (val == 0) {
        return 0; // points are all colinear (lie on the same line)
    }
    if (val > 0) {
        return 1; // points are in a counterclockwise orientation
    }
    else return 2; // points are in a clockwise orientation
}

// returns true if two lines intersect
bool doIntersect(line l1, line l2) {
    int orientation1 = orientation(l1.p1, l1.p2, l2.p1);
    int orientation2 = orientation(l1.p1, l1.p2, l2.p2);
    int orientation3 = orientation(l2.p1, l2.p2, l1.p1);
    int orientation4 = orientation(l2.p1, l2.p2, l1.p2);

    if ((orientation1 != orientation2) && (orientation3 != orientation4)) {
        return true;
    }

    if (orientation1 == 0 && onSegment(l1.p1, l2.p1, l1.p2)) {
        return true;
    }
    if (orientation2 == 0 && onSegment(l1.p1, l2.p2, l1.p2)) {
        return true;
    }
    if (orientation3 == 0 && onSegment(l2.p1, l1.p1, l2.p2)) {
        return true;
    }
    if (orientation4 == 0 && onSegment(l2.p1, l1.p2, l2.p2)) {
        return true;
    }

    return false;
}

// returns the distance between 2 points
double pointDistance(point p1, point p2) {
    cout << "distance between: " << printPoint(p1) << "," << printPoint(p2) << " = ";
    double dx = abs(p1.x - p2.x);
    double dy = abs(p1.y - p2.y);
    cout << sqrt((dx * dx) + (dy * dy)) << endl;
    return sqrt((dx * dx) + (dy * dy));
}

// solves for the point of a line intersection using the Bazier formula
// https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
point findIntersect(line l1, line l2) {
    double base = (l1.p1.x - l1.p2.x) * (l2.p1.y - l2.p2.y) - (l1.p1.y - l1.p2.y) * (l2.p1.x - l2.p2.x);
    if (base == 0) return nullPoint;
    double t = ((l1.p1.x - l2.p1.x) * (l2.p1.y - l2.p2.y) - (l1.p1.y - l2.p1.y) * (l2.p1.x - l2.p2.x)) / base;
    double u = ((l1.p1.x - l1.p2.x) * (l1.p1.y - l2.p1.y) - (l1.p1.y - l1.p2.y) * (l1.p1.x - l2.p1.x)) / base;

    cout << "u = " << u << " t = " << t << endl;
    point intersect = nullPoint;
    if (t >= 0 && t <= 1) {
        intersect.x = (l1.p1.x + t * (l1.p2.x - l1.p1.x));
        intersect.y = (l1.p1.y + t * (l1.p2.y - l1.p1.y));
    }
    else if (u >= 0 && u <= 1) {
        intersect.x = (l2.p1.x + u * (l2.p2.x - l2.p1.x));
        intersect.y = (l2.p1.y + u * (l2.p2.y - l2.p1.y));
    }
    return intersect;
}

// returns true when given duplicate points
bool comparepoint(point p1, point p2) {
    if (p1.x == p2.x && p1.y == p2.y) return true;
    return false;
}

// returns true when given duplicate lines
bool compareline(line l1, line l2) {
    if (comparepoint(l1.p1, l2.p1) && comparepoint(l1.p1, l2.p2)) return true;
    return false;
}

class polygon {
public:
    vector<line> boundaries; // must be written in order
    point center;   // center of the shape
    double maxdist;  // maximum distance from the center to the edge
    double minmax[4] = { 0,0,0,0 };    // minX, minY, maxX, maxY
    int id = 0;

    // shape details, only created after running the shapetype command
    bool validshape = false; // returns true if the shape is valid
    int sides; // returns the number of lines in the polygon
    bool convex = true; // returns true if the shape is convex

    // returns true if all lines connect to one another 
    bool check_bounds(vector<line>* newlines) {
        for (int i = 0; i < newlines->size() - 1; i++) {
            if (!comparepoint(newlines->at(i).p2, newlines->at(i + 1).p1)) {  // checks to make sure all the lines connect to one another
                cout << "invalid boundaries" << endl;
                return false;
            }
        }
        if (!comparepoint(newlines->at(newlines->size() - 1).p2, newlines->at(0).p1)) {  // checks to make sure the last point connects to the first point
            cout << "invalid boundaries" << endl;
            return false;
        }
        return true;
    }

    // updates the shape details
    bool validateshape(vector<line>* newlines) {
        if (!check_bounds(newlines)) {
            validshape = false;
            return false;
        }
        validshape = true;

        if (convex) {
            sides = get_sides(newlines);
        }
        minmax[0] = boundaries.at(0).p1.x;
        minmax[1] = boundaries.at(0).p2.x;
        minmax[2] = boundaries.at(0).p1.y;
        minmax[3] = boundaries.at(0).p2.y;

        for (int line = 1; line < newlines->size(); line++) {
            point pointarray[2] = { newlines->at(line).p1, newlines->at(line).p2 };
            for (int i = 0; i < 2; i++) {

                minmax[0] = min(pointarray[i].x, minmax[0]);
                minmax[1] = max(pointarray[i].x, minmax[1]);
                minmax[2] = min(pointarray[i].y, minmax[2]);
                minmax[3] = max(pointarray[i].y, minmax[3]);
            }
        }
        maxdist = pointDistance(addPoint(minmax[0], minmax[2]), addPoint(minmax[1], minmax[3]));
        return true;
    }



    // gets the sides of a polygon
    int get_sides(vector<line>* newlines) {
        return newlines->size();
    }

    // sets the lines for the polygon
    int setlines(vector<line> newlines) {
        // clears the boundary array
        while (boundaries.size() > 0) {
            boundaries.pop_back();
        }

        // updates the boundary array
        for (int i = 0; i < newlines.size(); i++) {
            boundaries.push_back(newlines.at(i));
        }
        boundaries.shrink_to_fit();

        validateshape(&boundaries);
        return 0;
    }

    void debugInfo() {
        cout << "-----" << endl
            << "Address: " << this << endl
            << "Sides: " << sides << endl
            << "Center: " << printPoint(center) << endl
            << "Maxdist: " << maxdist << endl
            << "Min Point: (" << minmax[0] << "," << minmax[1] << ")" << endl
            << "Max Point: (" << minmax[2] << "," << minmax[3] << ")" << endl
            << minmax[3] << endl;
        for (int i = 0; i < boundaries.size(); i++) {
            cout << "L" << i << " " << printLine(boundaries.at(i)) << endl;
        }
        cout << "-----" << endl;
    }
};

vector<line> createrect(point startpoint, float length, float height) {
    vector<line> outrect;
    outrect.push_back(addLine(addPoint(startpoint.x, startpoint.y), addPoint(startpoint.x + length, startpoint.y)));
    outrect.push_back(addLine(addPoint(startpoint.x + length, startpoint.y), addPoint(startpoint.x + length, startpoint.y + height)));
    outrect.push_back(addLine(addPoint(startpoint.x + length, startpoint.y + height), addPoint(startpoint.x, startpoint.y + height)));
    outrect.push_back(addLine(addPoint(startpoint.x, startpoint.y + height), addPoint(startpoint.x, startpoint.y)));
    return outrect;
}

struct collisionReport {
    bool collided = false;
    time_t timestamp;
    int entity1;
    int entity2;
    vector<line> collided_line1;
    vector<line> collided_line2;
    vector<point> intersection_point;
};
int printReport(collisionReport *report) {
    cout << "COLLISION REPORT\n"
        << "timestamp: " << report->timestamp
        << "entity1: " << report->entity1 << "| entity2: " << report->entity2 << "\n"
        << "entity1 collision lines:\n";
    for (int i = 0; i < report->collided_line1.size(); i++) {
        cout << printLine(report->collided_line1.at(i)) << "\n";
    }
    cout << "entity1 collision lines:\n";
    for (int i = 0; i < report->collided_line2.size(); i++) {
        cout << printLine(report->collided_line2.at(i)) << "\n";
    }
    cout << "intersection points:\n";
    for (int i = 0; i < report->intersection_point.size(); i++) {
        cout << printPoint(report->intersection_point.at(i));
    }
    cout << "----------" << endl;
    return 0;
}

vector<collisionReport> collisionLog;

// checks collision between two polygons and outputs a single intersection point if a collision occured
int checkcollision(polygon poly1, polygon poly2) {

    // short collision check
    if (max(poly1.maxdist, poly2.maxdist) * 2 < pointDistance(poly1.center, poly2.center)) {
        return NULL;
    }

    // begins long check for collisions
    bool collided = false;
    int line1 = 0;
    int line2 = 0;

    vector<int> line1collision;
    vector<int> line2collision;

    while (line1 < poly1.boundaries.size()) {
        line2 = 0;
        while (line2 < poly1.boundaries.size()) {
            cout << "poly1.l" << line1 << ": " << printLine(poly1.boundaries.at(line1)) << ", poly2.l" << line2 << ": " << printLine(poly2.boundaries.at(line2)) << endl;
            if (doIntersect(poly1.boundaries.at(line1), poly2.boundaries.at(line2))) {
                collided = true;
                line1collision.push_back(line1);
                line2collision.push_back(line2);
            }
            line2++;
        }

        line1++;
    }

    if (!collided) {
        return NULL;
    }

    // creates collision report
    collisionReport report;

    // populates general details in collision report
    report.entity1 = poly1.id;
    report.entity2 = poly2.id;
    report.collided = true;

    // writes collision lines to report
    for (int i = 0; i < line1collision.size(); i++) {
        report.collided_line1.push_back(poly1.boundaries.at(line1collision.at(i)));
        report.collided_line2.push_back(poly2.boundaries.at(line1collision.at(2)));
    }

    // finds intersection points
    for (int i = 0; i < line1collision.size(); i++) {

        // check for duplicate points which can cause unexpected behavior
        if (comparepoint(poly1.boundaries.at(line1collision.at(i)).p1, poly2.boundaries.at(line2collision.at(i)).p2) ||
            comparepoint(poly1.boundaries.at(line1collision.at(i)).p1, poly2.boundaries.at(line2collision.at(i)).p1)) {
            report.intersection_point.push_back(poly1.boundaries.at(line1collision.at(i)).p1);
        }
        else if (comparepoint(poly1.boundaries.at(line1collision.at(i)).p2, poly2.boundaries.at(line2collision.at(i)).p1) ||
            comparepoint(poly1.boundaries.at(line1collision.at(i)).p2, poly2.boundaries.at(line2collision.at(i)).p2)) {
            report.intersection_point.push_back(poly2.boundaries.at(line1collision.at(i)).p2);
        }

        // determines if lines are parallel and overlap
        else if (orientation(poly1.boundaries.at(line1collision.at(i)).p1, poly1.boundaries.at(line1collision.at(i)).p2, poly2.boundaries.at(line2collision.at(i)).p1) == 1 &&
                 orientation(poly1.boundaries.at(line1collision.at(i)).p1, poly1.boundaries.at(line1collision.at(i)).p2, poly2.boundaries.at(line2collision.at(i)).p2) == 1) {
            // report if lines are parallel and overlap
        }

        // calculate the line intersection using the findIntersect function
        else {
            report.intersection_point.push_back(findIntersect(poly1.boundaries.at(line1 - 1), poly2.boundaries.at(line2)));
        }
    }
}

int main() {
    
    polygon poly1;
    poly1.setlines(createrect(addPoint(0, 0), 5, 10));
    poly1.debugInfo();
    polygon poly2;
    poly2.setlines(createrect(addPoint(1, 1), 2, 14));
    poly2.debugInfo();
    checkcollision(poly1, poly2);
    for (int i = 0; i < collisionLog.size(); i++) {
        printReport(&collisionLog.at(i));
    }
    return 0;
}
