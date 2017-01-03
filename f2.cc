// /mnt/c/Users/Nika/Desktop
#include <omp.h>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
// #include "rdtsc.h"

//#define LINE_GRANULARITY 1000000
#define LINE_GRANULARITY 1000
#define CHUNK_SIZE 1000

using namespace std;
//unsigned long long t1, t2;
unordered_multimap<string, string> mymap;

class Barcode_st {
public:
	Barcode_st() {
		lock_ptr = NULL;
	};
	
	~Barcode_st() {
		if (lock_ptr!=NULL) {
			omp_destroy_lock(lock_ptr);
			delete lock_ptr;
			lock_ptr = NULL;
		}
	}

	omp_lock_t *lock_ptr;
	//////////////////////////////////////////////////////////////////////////////////////////////////

	void write_line(const string& key, const string& line) {
		mymap.insert(make_pair(key, line));
		
		///////////////////////
		
		
	};
/////////////////////////////////////////////////////////////////////////////////////////////////////////
	
private:
	const int dir_extract_len = 6;

	string extract_dir_name (const string& key) {
		return key.substr(0, dir_extract_len) + '/';
	}
};

int find_pattern_start(const string& line, string pattern) {
	for (int k = 1; k < line.size() - pattern.size(); k++) {
		if (line.substr(k, pattern.length()) == pattern && (line[k-1] == ' ' || line[k-1] == '\t') ) {
			return k + pattern.length();
		}
	}
	return -1;
}

int find_length(const string& line, int pos) {
	if (pos < 0)
		return -1;
	for (int l = pos; l < line.length(); l++) {
		if (line[l] != 'A' && line[l] != 'C' && line[l] != 'G' && line[l] != 'N' && line[l] != 'T') {
			return l - pos;
		}
	}
	return line.length() - pos;
}

void process_lines(string *lines, int num_lines, map<string, Barcode_st>& barcode_map, omp_lock_t *read_locks, int num_locks, string output_dir = "./") {
	int i;
	
	#pragma omp parallel for private(i) schedule(dynamic, CHUNK_SIZE) 
	for (i = 0; i < num_lines; i++) {
		// get thread id
		int thread_id = omp_get_thread_num();

		const string &line = lines[i];
		if (line[0] == '@')
			continue;
		// init
		string BX = "";
	       	//string BC = "";

		// finding BX
		int BX_start = find_pattern_start(line, "BX:Z:");
		
		if (BX_start != -1) {
			int BX_length = find_length(line, BX_start);
			BX = line.substr(BX_start, BX_length);
		}

		
		Barcode_st *barcode_ptr = NULL;
		omp_lock_t *lock_ptr = NULL;
		string barcode_key;

		// OBTAIN PTRS
		// valid barcode read
		if (BX_start != -1) { // && BC_start != -1) {
			barcode_key = BX; // + "_" + BC;
			bool key_exist = false;

			omp_set_lock(&read_locks[thread_id]);
			key_exist = barcode_map.find(barcode_key) != barcode_map.end();
			omp_unset_lock(&read_locks[thread_id]);

			if (key_exist) {
				omp_set_lock(&read_locks[thread_id]);

				lock_ptr = barcode_map.at(barcode_key).lock_ptr;
				barcode_ptr = &barcode_map.at(barcode_key);

				// WRITE LINE
				omp_set_lock(lock_ptr);
				barcode_ptr->write_line(barcode_key, line);
				omp_unset_lock(lock_ptr);

				omp_unset_lock(&read_locks[thread_id]);
			}
			else {
				// to write we need to grab all the read locks
				for (int j = 0; j < num_locks; j++)
					omp_set_lock(&read_locks[j]);

				// test again to make sure no race
				key_exist = barcode_map.find(barcode_key) != barcode_map.end();
				if (key_exist) {
					lock_ptr = barcode_map.at(barcode_key).lock_ptr;
					barcode_ptr = &barcode_map.at(barcode_key);
				}
				else {
					lock_ptr = new omp_lock_t;
					omp_init_lock(lock_ptr);
					barcode_ptr = &barcode_map[barcode_key];
					barcode_ptr->lock_ptr = lock_ptr;
					//barcode_ptr->check_dir(output_dir, barcode_key);
				}

				// WRITE LINE
				barcode_ptr->write_line(barcode_key, line);

				// release all the read locks
				for (int j = num_locks - 1; j >= 0; j--)
					omp_unset_lock(&read_locks[j]);
			}
		}
		// broken barcode read
		else {
			barcode_key = "no_barcode";

			omp_set_lock(&read_locks[thread_id]);

			barcode_ptr = &barcode_map.at("no_barcode");
			lock_ptr = barcode_map.at("no_barcode").lock_ptr;

			// WRITE LINE
			omp_set_lock(lock_ptr);
			barcode_ptr->write_line(barcode_key, line);
			omp_unset_lock(lock_ptr);

			omp_unset_lock(&read_locks[thread_id]);
		}

	}

	string file_total_name = "sorted.sam";

	ofstream output_file(file_total_name, fstream::app);


	for(auto& kv : mymap) {

		output_file << kv.second.c_str() << endl;

	}

	output_file.close();
}

int main (int argc, char *argv[]) {

	if (argc != 2 && argc != 3) {
		cerr << "Usage: ./bin input_SAM output_dir" << endl;
		exit(0);
	}
	int fdin, fdout;
	char *src, *dst;
	struct stat statbuf;
//t1=rdtsc();
	// default output_dir
	string output_dir = "./";
	if (argc == 3) {
		output_dir = argv[2];
		if (output_dir[output_dir.length() - 1] != '/')
			output_dir += '/';
	}

	// allocate lines
	string *lines;
	lines = new string[LINE_GRANULARITY];

	// initialize the barcode_map
	map<string, Barcode_st> barcode_map;
	// init the no_barcode entry
	barcode_map["no_barcode"].lock_ptr = new omp_lock_t;
	barcode_map["no_barcode"].write_line("no_barcode", "");
	omp_init_lock(barcode_map["no_barcode"].lock_ptr);

	int num_lines = 0;

	ifstream input_file(argv[1]);

	if (!input_file.is_open() ) {
		cerr << "File " << argv[1] << " doesn't not exist. Exiting" << endl;
		exit(0);
	}

	// initiate read locks
	omp_lock_t *read_locks;
	int num_locks = omp_get_max_threads();
	read_locks = new omp_lock_t[num_locks];

	cout << "num of threads: " << omp_get_max_threads() << endl;

	for (int i = 0; i < num_locks; i++)
		omp_init_lock(&read_locks[i]);

	// process file
	int process_counter = 0;
	while (!input_file.eof()) {
		getline(input_file, lines[num_lines]);
		num_lines++;

		// remove the extra line after file is exhausted
		if (input_file.eof())
			num_lines--;

		if (num_lines == LINE_GRANULARITY) {
			process_lines(lines, num_lines, barcode_map, read_locks, num_locks, output_dir);
			num_lines = 0;
			process_counter++;
			cout << "processed " << process_counter << "*" << LINE_GRANULARITY << " reads" << endl;
		}
	}

	if (num_lines != 0) {
		process_lines(lines, num_lines, barcode_map, read_locks, num_locks, output_dir);
		num_lines = 0;
	}

	//t2=rdtsc();
//	printf("%llu", t2-t1);
	// destroy read locks
	for (int i = 0; i < num_locks; i++)
		omp_destroy_lock(&read_locks[i]);
	/////////////////////////////////////////////////////////////////////////

//moved to processes lines

/////////////////////////////////////////////////////////////////////////
	delete [] lines;
	delete [] read_locks;

	return 0;
}
