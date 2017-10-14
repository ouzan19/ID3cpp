#ifndef AI_SPACE_FILE_HPP
#define AI_SPACE_FILE_HPP

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>

using namespace std;
class AISpaceFileReader{

public:
	AISpaceFileReader(string filename);
	~AISpaceFileReader();
	vector<string> getTags();
	vector<string> getDecisions();
	vector<vector<string>> getTrainingData();
	vector<vector<string>> getTestData();
	unordered_map<string, set<string>> getValues();
private:
	AISpaceFileReader(); 
	vector<string> tags;
	vector<string> decisions;
	unordered_map<string, set<string>> values;
	vector<vector<string>> trainingData;
	vector<vector<string>> testData;









};

#endif /AI_SPACE_FILE_HPP