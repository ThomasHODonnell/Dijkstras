//PARTNER: MALHAR PANDYA

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <climits>
#include <queue>
#include <sstream>
#include <algorithm>

using namespace std;

class Edge; 

class Vertex {
public:
    string code;
    int dvalue;
    Edge* parentedge;
    vector<Edge*> adjlist;
   Vertex(string code) : code(code), dvalue(2401), parentedge(nullptr) {} 
};

class Edge {
public:
    int depttime, arrtime;
    Vertex* origin;
    Vertex* dest;
    string airline, flno;

    Edge(const string& airline, const string& flno, Vertex* origin, Vertex* dest, int depttime, int arrtime)
        : airline(airline), flno(flno), origin(origin), dest(dest), depttime(depttime), arrtime(arrtime) {}
};

class CompareVertices {
public:
    bool operator()(const Vertex* v1, const Vertex* v2) {
        return v1->dvalue > v2->dvalue; // Min-heap based on dvalue
    }
};

class Graph {
public:
    map<string, Vertex*> vmap;
    vector<Edge*> elist;
    priority_queue<Vertex*, vector<Vertex*>, CompareVertices> heap;
    vector<Vertex*> N;

    void readAirports(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            exit(EXIT_FAILURE);
        }

        string line;
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty()) 
                continue;

            Vertex* v = new Vertex(line);
            vmap[line] = v;
            heap.push(v);
        }

        file.close(); 
    }

    void readFlights(const string& filename) {

        ifstream file(filename);
        if (!file.is_open()) {
            exit(EXIT_FAILURE);
        }

        string line;
        getline(file, line);
        while (getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);

            if (line.empty()) 
                continue;

            istringstream iss(line);
            string airline, flno, originCode, destCode;
            int depttime, arrtime;

            if (!(iss >> airline >> flno >> originCode >> destCode >> depttime >> arrtime)) {
                continue;
            }

            if (vmap.find(originCode) == vmap.end() || vmap.find(destCode) == vmap.end()) {
                continue;
            }

            Vertex* origin = vmap[originCode];
            Vertex* dest = vmap[destCode];
            Edge* e = new Edge(airline, flno, origin, dest, depttime, arrtime);
            elist.push_back(e);
            origin->adjlist.push_back(e);
        }

        file.close(); 
    }

    int dijkstra(const string& srcCode, const string& sinkCode, int starttime) {

        Vertex* v = vmap.at(srcCode);
        v->dvalue = starttime;
        decreaseKey(v,starttime);

        while(!heap.empty()) {
            Vertex* u = heap.top();
            heap.pop();
            Edge* e; 
            for (int i = 0;i < u->adjlist.size(); i++){
                e = u->adjlist.at(i); 
                relax(e);
            }
        }
        return vmap.at(sinkCode)->dvalue;
    }

    void decreaseKey(Vertex* v, int key) {
        //if needed first remove and then change value and re-insert
        v->dvalue = key;
        heap.push(v);
    }

    void relax(Edge* e) {
        //relax will need to change based on temporal situation here
        int waitTime = 0; 
        if (e->depttime < e->origin->dvalue) waitTime = 2400 - e->origin->dvalue + e->depttime;
        else waitTime = e->depttime - e->origin->dvalue;

        int newD = e->origin->dvalue + waitTime + (e->arrtime - e->depttime);
        if (e->dest->dvalue > newD) {
            e->dest->dvalue = newD;
            e->dest->parentedge = e;
            decreaseKey(e->dest,e->dest->dvalue);
        }
    }

    void printPath(const string& code) {
        Edge* e = vmap[code]->parentedge;
        if (!e) return;
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        return EXIT_FAILURE;
    }

    string airportsFile = "airports.txt";
    string flightsFile = "flights.txt";

    Graph graph;
    graph.readAirports(airportsFile);
    graph.readFlights(flightsFile);

    int startTime = stoi(argv[3]);
    int shortestPath = graph.dijkstra(argv[1], argv[2], startTime);
    cout << shortestPath << endl;

    return 0;
}