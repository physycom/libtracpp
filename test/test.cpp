#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>

#include "jsoncons/json.hpp"

#include "libtracpp.hpp"

using namespace std;
using namespace jsoncons;

// sample class to instantiate
// the template library
class gnsspoint {
public:
  double lat, lon;
  int timestamp;

  // json constructor
  gnsspoint(){}
  gnsspoint(const json &j){
    lat = j["lat"].as<double>();
    lon = j["lon"].as<double>();
    timestamp = j["timestamp"].as<int>();    
  }

  // json converter
  json to_json() const {
    json j;
    j["lat"] = lat;
    j["lon"] = lon;
    j["timestamp"] = timestamp;
    return j;
  }
};

// auxiliary function to 
// (de)serialize json into class
// (pretty unsafe)
template<class T>
vector<T> deserialize(const json &j){
  vector<T> trip(0);
  T point;
  if( j.is_array() ){
    for(const auto &r : j.array_range()){
      trip.emplace_back(r);
    }
  }
  else if ( j.is_object() ){
    for(const auto &r : j.object_range()){
      trip.emplace_back(r.value());
    }
  }
  return trip;
}

template<class T>
json serialize(const vector<T> &vec, char mode = 'a'){
  json j;
  json r;
  if ( mode == 'a'){     // array
    j = json::array();
    for(const auto &r : vec){
      j.add(r.to_json());
    } 
  }
  else if ( mode ==  'o'){
    size_t cnt = 0;
    string prefix = "record_";
    for(const auto &r : vec){
      stringstream ss;
      ss << prefix << setw(5) << setfill('0') << endl;
      j[ss.str()] = r.to_json();
    }
  }
  return j;
}


// test
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

  // instantiate rdp object 
  rdp_algo rdpalgo;

  // Converting json to std container
  auto trip = deserialize<gnsspoint>(jin);

  // Reduce with rdp
  auto rdp = rdpalgo.reduce(trip);

  // Convert std container to json and dump
  auto jrdp = serialize(rdp);
  ofstream out_(input_name.substr(0, input_name.size() - 5) + "_reduced.json");
  out_ << pretty_print(jrdp) << endl;
  out_.close();

  // Courtesy output
  cout << "Original points : " << trip.size() << endl;
  cout << "Reduced points  : " << rdp.size() << endl;
  cout << "Rank parameter  : " << rdp.size() / (double) trip.size() << endl;

  return 0;
}

