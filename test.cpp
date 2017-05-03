#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <windows.h>
#include <iomanip>

#include "jsoncons/json.hpp"

#include "libtracpp.hpp"

using namespace std;
using namespace jsoncons;

class gnsspoint {
public:
  double lat, lon;
  int timestamp;
};

int main(int argc, char **argv){
  cout << "Testing LIBTRACPP" << endl;

  // Parsing command line
  string input_name;
  if(argc > 1){
    input_name = argv[1];
  }
  else{
    cout << "Wrong command line" << endl;
    exit(-1);
  }

  // Parsing input JSON
  json jin;
  try {
    jin = json::parse_file(input_name);
  }
  catch (parse_exception &e) {
    cerr << "JSON PARSE EXC : " << e.what() << endl;
    exit(-1);
  }
  catch (json_exception_1<runtime_error> &e) {
    cerr << "JSON FILE EXC : " << e.what() << endl;
    exit(-1);
  }
  catch (std::exception &e) {
    cerr << "GENERIC EXC : " << e.what() << endl;
    exit(-1);
  }

  // Converting to std container
  vector<gnsspoint> trip;
  for(const auto &r : jin.array_range()){
    gnsspoint temp;
    temp.lat = r["lat"].as<double>();
    temp.lon = r["lon"].as<double>();
    temp.timestamp = r["timestamp"].as<int>();
    trip.push_back(temp);
  }

  // Call rdp algorithm
  rdp_algo rdpalgo;
  auto rdp = rdpalgo.reduce(trip);

  // Jsonize and dump reduced trajectory
  json jout = json::array();
  for(const auto &r : rdp){
    json temp;
    temp["lat"] = r.lat;
    temp["lon"] = r.lon;
    temp["timestamp"] = r.timestamp;
    jout.add(temp);
  }
  ofstream out(input_name.substr(0, input_name.size() - 5) + "_reduced.json");
  out << pretty_print(jout) << endl;
  out.close();

  // Courtesy output
  cout << "Original points : " << trip.size() << endl;
  cout << "Reduced points  : " << rdp.size() << endl;
  cout << "Rank parameter  : " << rdp.size() / (double) trip.size() << endl;

  return 0;
}

