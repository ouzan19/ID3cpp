#include "AISpaceFileReader.h"

AISpaceFileReader::AISpaceFileReader(){

}

AISpaceFileReader::~AISpaceFileReader(){

}

AISpaceFileReader::AISpaceFileReader(string filename){

	tags.clear();
	testData.clear();
	trainingData.clear();
	decisions.clear();
	values.clear();
	string line;
	ifstream f;
	f.open(filename);

	while (getline(f, line)){

		//cout << line << endl;

		//! if tag
		if (line[0] == 'T'){

			line.erase(0, 3);
			size_t pos = 0;
			std::string token;
			while ((pos = line.find(", ")) != std::string::npos) {
				token = line.substr(0, pos);
				tags.push_back(token);

				set<string> s;
				pair<string, set<string>> value(token, s);
				values.insert(value);

				line.erase(0, pos + 2);
			}
			line.erase(line.size() - 1, line.size());
			tags.push_back(line);
			
			set<string> s;
			pair<string, set<string>> value(line, s);
			values.insert(value);
			

		}


		//! if training data
		else if (line[0] == 'A'){
			vector<string> data;
			line.erase(0, 3);
			size_t pos = 0;
			std::string token;
			int tagNo = 0;
			while ((pos = line.find(", ")) != std::string::npos) {
				token = line.substr(0, pos);
				data.push_back(token);
				line.erase(0, pos + 2);

				values[tags[tagNo]].insert(token);
				tagNo++;
			}
			line.erase(line.size() - 1, line.size());
			data.push_back(line);
			decisions.push_back(line);
			trainingData.push_back(data);
			values[tags[tagNo]].insert(line);
		}

		//! if test data
		else if (line[0] == 'B'){
			vector<string> data;
			line.erase(0, 3);
			size_t pos = 0;
			std::string token;
			while ((pos = line.find(", ")) != std::string::npos) {
				token = line.substr(0, pos);
				data.push_back(token);
				line.erase(0, pos + 2);
			}
			line.erase(line.size() - 1, line.size());
			data.push_back(line);
			testData.push_back(data);
		}


	}

	f.close();
}


vector<string> AISpaceFileReader::getTags(){

	return tags;
}

vector<vector<string>> AISpaceFileReader::getTestData(){

	return testData;
}


vector<vector<string>> AISpaceFileReader::getTrainingData(){

	return trainingData;
}

vector<string> AISpaceFileReader::getDecisions(){

	return decisions;
}


unordered_map<string, set<string>> AISpaceFileReader::getValues(){

	return values;
}