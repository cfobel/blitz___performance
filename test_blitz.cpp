#include <iostream>
#include <vector>
#include <boost/unordered_map.hpp>
#include <boost/format.hpp>
#include <boost/foreach.hpp>
#include <blitz/array.h>
#include "timing.hpp"


using blitz::Array;
using blitz::Range;
using namespace std;


struct DummyStruct {
    int x;
    int y;
    int z;

    DummyStruct() : x(0), y(0), z(0) {}
    DummyStruct(int x, int y, int z) : x(x), y(y), z(z) {}
};


int main(int argc, char** argv) {
    typedef boost::format _f;
    typedef boost::unordered_map<string, double> duration_map_t;
    duration_map_t duration_map;

    timespec start, end, delta;
    if(argc != 2) {
        cerr << _f("usage: %s <N>") % argv[0] << endl;
        exit(-1);
    }
    const int N = atoi(argv[1]);

    vector<DummyStruct> struct_v(N);
    clock_gettime(CLOCK_REALTIME, &start);
    int total_count = 0;
    for(int i = 0; i < N; i++) {
        struct_v[i] = DummyStruct(i, i, i);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    delta = time_diff(start, end);
    duration_map["struct"] = delta.tv_sec + delta.tv_nsec * 1e-9;

    blitz::Array<int, 2> blitz_tall_narrow(N, 3);
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 0; i < N; i++) {
        blitz_tall_narrow(i, 0) = N;
        blitz_tall_narrow(i, 1) = N;
        blitz_tall_narrow(i, 2) = N;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    delta = time_diff(start, end);
    duration_map["blitz tall/narrow"] = delta.tv_sec + delta.tv_nsec * 1e-9;

    blitz::Array<int, 2> blitz_short_wide(3, N);
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 0; i < N; i++) {
        blitz_short_wide(0, i) = N;
        blitz_short_wide(1, i) = N;
        blitz_short_wide(2, i) = N;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    delta = time_diff(start, end);
    duration_map["blitz short/wide"] = delta.tv_sec + delta.tv_nsec * 1e-9;

    blitz::Array<int, 1> blitz_x(N);
    blitz::Array<int, 1> blitz_y(N);
    blitz::Array<int, 1> blitz_z(N);
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 0; i < N; i++) {
        blitz_x(i) = N;
        blitz_y(i) = N;
        blitz_z(i) = N;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    delta = time_diff(start, end);
    duration_map["blitz 3-seperate arrays"] = delta.tv_sec + delta.tv_nsec * 1e-9;

    blitz::Array<DummyStruct, 1> blitz_struct(N);
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 0; i < N; i++) {
        blitz_struct(i) = DummyStruct(i, i, i);
    }
    clock_gettime(CLOCK_REALTIME, &end);
    delta = time_diff(start, end);
    duration_map["blitz array of structs"] = delta.tv_sec + delta.tv_nsec * 1e-9;

    blitz::Array<int, 2> blitz_struct_wrapper((int *)blitz_struct.dataFirst(),
            blitz::shape(N, 3), blitz::neverDeleteData);
    clock_gettime(CLOCK_REALTIME, &start);
    for(int i = 0; i < N; i++) {
        blitz_struct_wrapper(i, 0) = i;
        blitz_struct_wrapper(i, 1) = i;
        blitz_struct_wrapper(i, 2) = i;
    }
    clock_gettime(CLOCK_REALTIME, &end);
    delta = time_diff(start, end);
    duration_map["blitz struct wrapper"] = delta.tv_sec + delta.tv_nsec * 1e-9;


    BOOST_FOREACH(duration_map_t::value_type &item, duration_map) {
        cout << _f("%s,%.2g") % item.first % item.second << endl;
    }
    
    return 0;
}
