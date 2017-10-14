#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "AISpaceFileReader.h"

using namespace std;
int outputDim;
unordered_map<string, set<string>> values;
vector<string> tags;



class Node{

public:
	Node(){}
	string id;
	string split;
	string decision;
	float entropi;
	int instances;
	string color;
	unordered_map<string, Node*> childs;
	vector<string> decisions;
	vector<vector<string>> data;
	unordered_map<string, int> counts;
	bool isLeaf;
	void addChild(Node* c,string v){

		pair<string, Node*> cpair(v, c);
		childs.insert(cpair);

	}

	void print( int depth){

		cout << split<<"  " << entropi<< "  "<<instances<<"  "<<decision;
		/*for (int i = 0; i < decisions.size(); i++)
			cout << decisions[i] << " ,";*/
		cout << endl;
		for (auto it = childs.begin(); it != childs.end(); ++it){

			for (int i = 0; i < depth+1; i++)
				cout << "\t";
			cout << it->first << " : ";
			it->second->print(depth + 1);
		
		}

	}

	void graphViz(ostream &f = cout){

		f <<"  "<< id << " [shape=box, color=" << this->color << ", label=\"Split: " << split << "\\nEntropy = " << entropi << "\\nInstances = " << instances << "\\nDecision = " << decision << "\n\\n Value  Count\\n" ;


		for (auto it = counts.begin(); it != counts.end(); ++it){


			f << it->first << " :  " << it->second << endl;

		}

		f << "\"];" << endl;

		for (auto it = childs.begin(); it != childs.end(); ++it){

			f <<"  "<< id << " -> " << it->second->id << "[label=\"" << it->first << "\"] ;" << endl;
		}
		f << endl;


		for (auto it = childs.begin(); it != childs.end(); ++it)
			it->second->graphViz(f);
	}


	void decide(){


		unordered_map<string, int> counts;

		for (auto it = values[tags[tags.size() - 1]].begin(); it != values[tags[tags.size() - 1]].end(); ++it){

			pair<string, int> temp(*it, 0);
			counts.insert(temp);
		}


		for (int i = 0; i < decisions.size(); i++){

			string d = decisions[i];
			counts[d]++;

		}


		string decision;
		int maxCount = 0;
		
		for (auto it = decisions.begin(); it != decisions.end(); ++it){

			if (counts[*it] > maxCount){

				maxCount = counts[*it];
				decision = *it;
			}


		}

		this->decision = decision;
	}

	void amILeaf(){

		
		bool allSame = true;
		for (int i = 1; i < decisions.size(); i++)
			if (decisions[i] != decisions[0])
				allSame = false;

		if (allSame)
			this->isLeaf = true;
		else 
			this->isLeaf = false;
	}

	int countNodes(){

		int count = childs.size();

		for (auto it = childs.begin(); it != childs.end(); ++it)
			count += it->second->countNodes();

		return count;

	}

	int countSplits(){

		int count = 0;


		for (auto it = childs.begin(); it != childs.end(); ++it)
			if (!it->second->isLeaf)
				count++;


		for (auto it = childs.begin(); it != childs.end(); ++it)
			count += it->second->countSplits();

		return count;

	}


	int maxDepth(){

		if (isLeaf)
			return 0;

		int mdepth = 0;
		for (auto it = childs.begin(); it != childs.end(); ++it){

			int depth = it->second->maxDepth();

			if (depth > mdepth)
				mdepth = depth;
		}

		return mdepth + 1;



	}


	void setColor(){

		//cout << isLeaf << "	" << entropi;

		if (this->isLeaf){

			if (entropi > 0){

				color = "blue";
			}
			else
				color = "green";

		}

		else
			color = "red";
	}

	void histogramInstances(){

			
			counts.clear();
			int index = tags.size() - 1;
			for (auto it = values[tags[index]].begin(); it != values[tags[index]].end(); ++it){

					pair<string, int> att(*it, 0);
					counts.insert(att);
			}


			for (int i = 0; i < this->data.size(); i++){

				counts[this->data[i][index]]++;


			}



			/*cout << "--------------------------------------------------" << endl;
			for (auto it = counts.begin(); it != counts.end(); ++it){


				cout << it->first << " :  " << it->second << endl;

			}*/
			


	}


};



float calcInitialEntropi(vector<string> data, set<string> values){


	unordered_map<string, int> counts;
	for (auto it = values.begin(); it != values.end(); ++it){

		pair<string, int> value(*it, 0);

	}
		

	int totalSize = data.size();
	for (int i = 0; i < totalSize; i++){

		string tagValue = data[i];
		counts[tagValue]++;
	}

	float entropi = 0;


	for (auto it = values.begin(); it != values.end(); ++it){
	
		string v = *it;
		if (counts[v] == 0)
			continue;
		float p = (float)counts[v] / totalSize;
		
		//cout << v<<"		"<<counts[v] << endl;
		entropi += (-p*log2(p));

	}

	return entropi;




}


float calculateAfterSplitEntropi(vector<vector<string>> data, vector<string> decisions, int tagNo, set<string> values, set<string> desicionTypes){

	float totalEnt = 0;
	unordered_map<string, unordered_map<string, int>> counts;
	
	for (auto it = values.begin(); it != values.end(); ++it){


		unordered_map<string, int> temp;
		for (auto d = desicionTypes.begin(); d != desicionTypes.end(); ++d){

			pair<string, int> att(*d, 0);
			temp.insert(att);
		}
			

		pair<string, unordered_map<string, int>> att(*it, temp);
		counts.insert(att);

	}

	for (auto it = values.begin(); it != values.end(); ++it){

		string att = *it;
		

		for (int i = 0; i < data.size(); i++){
			string v = data[i][tagNo];

			//cout << v << "	" << att << endl;
			if (v == att){

				string desicion = decisions[i];
				counts[att][desicion]++;
			}


		}
	}


	for (auto it = counts.begin(); it != counts.end(); ++it){

		float ent = 0;
		string att = it->first;
		unordered_map<string,int> c = it->second;


		int total=0;
		for (auto it2 = c.begin(); it2 != c.end(); ++it2){
			total += it2->second;
		}

		/*if (total == 0)
			return 0;*/
		//cout << endl;


		for (auto it2 = c.begin(); it2 != c.end(); ++it2){


			if (it2->second == 0)
				continue;

			float p = (float)it2->second / total;
			//cout << it2->second << "	" << total << endl;
			
		    ent += (-p*log2(p));
		}
		
		//cout << ent << endl;
		
		totalEnt += ((float)total/ data.size())*ent;

	}



	return totalEnt;


}


void split(Node* node, int tagNo, set<string> v){


	vector<vector<string>> data = node->data;
	int id = 1;
	for (auto it = v.begin(); it != v.end(); ++it){

		

		vector<vector<string>> temp;
		vector<string> dtemp;
		string att = *it;


		for (int i = 0; i < data.size(); i++){
			string v = data[i][tagNo];
			if (v == att){

				vector<string> d = data[i];
				//d.erase(d.begin()+tagNo);
				temp.push_back(d);
				dtemp.push_back(data[i][data[i].size() - 1]);
			}


		}


		Node* childNode = new Node();
		childNode->decisions = dtemp;
		childNode->data = temp;
		childNode->amILeaf();
		childNode->instances = temp.size();
		childNode->decide();
		childNode->id = node->id +to_string(id);
		childNode->histogramInstances();
		node->addChild(childNode, att);
		id++;
	}
}

void splitNode(Node* node){


	if (node->isLeaf){
		node->entropi = calcInitialEntropi(node->decisions, values[tags[tags.size() - 1]]);
		node->split = "Leaf";
		node->setColor();
		return;
	}
		
	float totalEnt = calcInitialEntropi(node->decisions, values[tags[tags.size() - 1]]);
	//cout << totalEnt << endl;

	int imax = -1;
	float maxGain = 0;
	string bestT;
	vector<float> gains;
	for (int j = 0; j < tags.size() - 1; j++){

		string t = tags[j];
		float e = calculateAfterSplitEntropi(node->data, node->decisions, j, values[t], values[tags[tags.size() - 1]]);
		float gain = totalEnt - e;
		gains.push_back(e);
		if (maxGain < gain){
			maxGain = gain;
			imax = j;
			bestT = t;

		}

		// cout << t << "	" << e<<endl;
	}

	node->entropi = totalEnt;

	if (imax == -1){

		node->split = "Leaf";
		node->isLeaf = true;
		node->setColor();
		return;

	}


	node->setColor();


	node->split = tags[imax];

	//cout << node->split << "	" << node->entropi << endl;

	split(node, imax, values[tags[imax]]);


	for (auto it = node->childs.begin(); it != node->childs.end(); ++it){

		Node* p = it->second;
		splitNode(p);
	}

}

string whatAreYou(Node* root,vector<string> input){


	int index = -1;
	for (int i = 0; i < tags.size(); i++)
		if (tags[i] == root->split)
			index = i;

	if (root->isLeaf){


		return root->decision;

	}

	else {

		if (input[index] == " ")
		{


			unordered_map<string, int> counts;

			for (auto it = values[tags[index]].begin(); it != values[tags[index]].end(); ++it){

					pair<string, int> att(*it, 0);
					counts.insert(att);
			}


			for (int i = 0; i < root->data.size(); i++){

				counts[root->data[i][index]]++;


			}

			int max = 0;
			string att;
			for (auto it = counts.begin(); it != counts.end(); ++it){
				if (it->second > max)
					att = it->first;

			}





			//cout << "missing		" << att<< endl;
			input[index] = att;

		}
		return whatAreYou(root->childs[input[index]], input);

	}
}


int main(char* argc,char** argv){

	string input1 = "Animals";
	string input2 = "Cars";
	string input3 = "Emails";
	string input4 = "Purchase";
	string input5 = "Superheroes";
	string input6 = "TVShows";
	string input7 = "Wow";

	string input = input1;
	AISpaceFileReader fr(argv[1]);
	//AISpaceFileReader fr("Input_" + input + ".txt");


	vector<string> decisions;
	vector<vector<string>> test, training;
	
	tags = fr.getTags();
	test = fr.getTestData();
	training = fr.getTrainingData();
	decisions = fr.getDecisions();
	values = fr.getValues();

	/*outputDim = (*(--values.end())).second.size();

	cout << outputDim << endl;

	for (auto it = values.begin(); it != values.end(); ++it){
		std::cout << it->first << endl << "		";

		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			cout << *it2 << ",  ";

		cout << endl;

	}

	for (int i = 0; i < decisions.size(); i++)
		cout << decisions[i] << "--";
	cout << endl << endl;

	for (int i = 0; i < training.size(); i++){
		for (int j = 0; j < training[i].size(); j++){

			cout << training[i][j] << "--";

		}
		cout << endl;
	}

	cout << endl;


	for (int i = 0; i < test.size(); i++){
		for (int j = 0; j < test[i].size(); j++){

			cout << test[i][j] << "--";

		}
		cout << endl;
	}
	cout << endl;*/





	Node* root = new Node();
	root->data = training;
	root->decisions = decisions;
	root->id = "1";
	root->amILeaf();
	root->instances = training.size();
	root->decide();
	root->histogramInstances();
	splitNode(root);

	
	for (int i = 0; i < test.size(); i++){
		string result = whatAreYou(root, test[i]);
		cout <<  result<<"	"<<  ((result == test[i][test[i].size()-1]) ? "true" : "false") << endl;
	}

	
	ofstream of;
	of.open("Output.dot");

	of << "digraph G\n {" << endl;

	of << "graph [label=\"Decision Tree\\n\\nNumber of Nodes = " << root->countNodes() + 1 << "\\nNumber of Splits = " << root->countSplits() + 1 << "\\nMaximum Depth = "<<root->maxDepth()<<"\\n\", labelloc = t];" << endl;


	root->graphViz(of);

	of << "}" << endl;

	of.close();

	cout << "done" << endl;

	//string command = "createGIF Output";
	//system(command.c_str());

	//getchar();
	return 5;



}