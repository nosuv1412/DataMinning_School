#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>

using namespace std;

//khai bao thuoc tinh cho lop labels
class labels {
	public:
		int numOflabel;
		vector<int> values;
		vector<int> numOfvalues;
		vector<double> probability;
	public:
		labels(int numOflabel, vector<int> num) {
			this->numOflabel = numOflabel;
			this->values.resize(numOflabel);
			this->numOfvalues = num;
			this->probability.resize(numOflabel);
		}
};

//khai bao thuoc tinh cho lop colums
class column{
	public:
		string name;
		int numOfValue;
		int index;
		vector<int> values;
		vector<vector<double> > probabilityOfClass;
	public:
		column(string name, int index,int numOfValue) {
			this->name = name;
			this->index = index;
			this->numOfValue = numOfValue;
			this->values.resize(numOfValue);
		}
};

// lay ra ten cua tat ca cac cot
vector<string> getColumnNames( string str) {
	string delimiter = ",";
	size_t pos = 0;
	string name_str;
	vector<string>names;
	while ((pos = str.find(delimiter)) != string::npos) {
	    name_str = str.substr(0, pos);
	    names.push_back(name_str);
	    str.erase(0, pos + delimiter.length());
	}
	names.push_back(name_str);
	return names;
}

//chia tap train va test
void splitTrainTest(vector<vector<int> > inputdata,vector<vector<int> > &trainset, vector<vector<int> > &testset, int trainsize) {
    for(int i = 0;i< inputdata.size();i++) {
    	(i < trainsize) ? trainset.push_back(inputdata[i]) : testset.push_back(inputdata[i]);
	}
}

void splitTrainTestRandomly(vector<vector<int> > inputdata,vector<vector<int> > &trainset, vector<vector<int> > &testset, double splitRatio) {
    // Xáo trộn dữ liệu ban đầu
    random_shuffle(inputdata.begin(), inputdata.end());

    // Tính số lượng mục cho tập huấn luyện dựa trên tỷ lệ trainRatio
    int trainSize = static_cast<int>(inputdata.size() * splitRatio);

    // Chia dữ liệu thành tập huấn luyện và tập kiểm tra
    trainset.assign(inputdata.begin(), inputdata.begin() + trainSize);
    testset.assign(inputdata.begin() + trainSize, inputdata.end());
}


// Lay ra tat ca cac gia tri cua mot thuoc tinh
vector<int> getAllValuesOfColumn(int index, vector< vector<int> > a) {
	set<int> s;
	for(int i=0;i<a.size();i++) {
		s.insert(a[i][index]);
	}
	vector<int> result (s.begin(), s.end());
	return result;
}

// tinh xac suat co dieu kien cua mot thuoc tinh
void conditional_probability(column &columns, labels labels, vector< vector<int> > trainset) {
	for(int k = 0; k < columns.values.size(); k++) {
		vector<double> prob;
		for(int m = 0; m < labels.values.size();m++) {
			int count_value_with_label = 0;
			for(int i=0;i<trainset.size();i++) {
				if(trainset[i][columns.index] == columns.values[k] && trainset[i][trainset[i].size()-1] == labels.values[m]) {
					count_value_with_label++;
				}
			}
			prob.push_back(count_value_with_label * 1.0 / labels.numOfvalues[m]);
		}
		columns.probabilityOfClass.push_back(prob);
	}
}

// du doan lop
int predict(vector<column> columns, labels labels, vector<int> x) {
	vector<double> prob;
	prob = labels.probability;

	for(int k=0; k<labels.values.size();k++) {
		for(int i = 0; i < x.size(); i++) {
			for(int j = 0 ;j < columns[i].values.size();j++) {
				if(columns[i].values[j] == x[i]) {
					prob[k] *= columns[i].probabilityOfClass[j][k];
				}

			}
		}
	}
	
	double max_prob = 0, label;
	for(int i=0;i<prob.size();i++) {
		if(prob[i] > max_prob) {
			max_prob = prob[i];
			label = i;
		}
	}

	return label;
}


// Hàm đọc dữ liệu từ file CSV
vector<vector<int>> read_csv(string filename) {
	vector<vector<int>> data;
   	ifstream file(filename.c_str());
    if (file.is_open()) {
        string line;
        getline(file, line);
        while (getline(file, line)) {
            stringstream ss(line);
            string cell;
            vector<int> row;
		    while (getline(ss, cell, ',')) {
				row.push_back(stoi(cell));
		    }
    		data.push_back(row);
        }
        file.close();
    } else {
        cout << "Unable to open file" << endl;
    }
    return data;
}

// goi ra cac colum name tu viec doc file
vector<string> getColumnNames_out(string fileName) {
    vector<string> columnNames;
    ifstream file(fileName.c_str());
    if (file.is_open()) {
        string line;
        if (getline(file, line)) { // Đọc dòng đầu tiên
            columnNames = getColumnNames(line);
        }
        file.close();
    } else {
        cout << "Unable to open file" << endl;
    }
    return columnNames;
}


int main() {

	//ten files can xu ly
	string fileName = "heart_xuly_final.csv";

	// doc tap du lieu luu vao inputdata
    vector<vector<int> > inputdata = read_csv(fileName);

	//doc ten cac cot luu vao colum_names
    vector<string> column_names = getColumnNames_out(fileName);


    // Khai bao tap train va tap test
    vector<vector<int> > trainset;
    vector<vector<int> > testset;

    // chon ty le chia theo du lieu dau vao (80%)
    double splitRatio = 0.8;
    // thuc hien chia
    int trainsize = round(inputdata.size() * splitRatio);
    // splitTrainTest(inputdata,trainset,testset,trainsize);
	splitTrainTestRandomly(inputdata, trainset, testset, splitRatio);

    // vector luu cac nhan va lu bien vao vector nay
	vector<int> labelValues;
	labelValues = getAllValuesOfColumn(column_names.size()-1,trainset);

	// vector luu xac suat mot ban ghi thuoc ve mot nhan nao do
	vector<double> probability_D(labelValues.size());

	// vector luu so luong cac ban ghi thuoc cac nhan
    vector<int> numOfValues(labelValues);

    // thuc hien viec gan so luong cac ban ghi ban dau cua cac nhan = 0
    for(int i=0;i<numOfValues.size();i++){
    	numOfValues[i] = 0;
	}

    // dem cac ban ghi thuoc cac nhan
	for(int k = 0; k < labelValues.size(); k++){
		for(int i=0; i < trainset.size(); i++) {
			if(trainset[i][trainset[i].size()-1] == labelValues[k]) {
				numOfValues[k]++;
			}
		}
	}

	// tinh xac suat cua tung nhan
	for(int k = 0; k< numOfValues.size();k++){
		probability_D[k] = numOfValues[k] * 1.0 / trainsize;
	}

	// khai bao doi tuong heart_lbs thuoc class labels
	labels heart_lbs(labelValues.size(), numOfValues);
	heart_lbs.values = labelValues;
	heart_lbs.probability = probability_D;

	//in ra thong tin xac suat cua nhan
	cout<<"Xac suat gia tri vao tung nhan: "<<endl;
	for(int i=0;i<probability_D.size();i++) {
		cout <<"Nhan "<<labelValues[i] << ": ";
		cout << probability_D[i]*100.0<<" %"<<endl;
	}
	cout << endl;

	// vector luu cac doi tuong cot;
    vector<column> columns;
	// them cac cot vao vector
	for(int i=0;i<column_names.size()-1;i++) {
		vector<int>columnValues;
		// lay ra tat ca cac gia tri cua cot
		columnValues = getAllValuesOfColumn(i,trainset);
		// khai bao doi tuong cot
		column c(column_names[i],i,columnValues.size());
		// gan vector chua cac gia tri vao cot
		c.values = columnValues;
		// tinh xac suat co dieu kien cua cac gia tri cua cot
		conditional_probability(c,heart_lbs,trainset);
		// them vao vector
		columns.push_back(c);
	}


	//in ra thong tin xac suat cua cac cot
	int count = 0;
	for(auto column : columns){
		cout <<endl<< column.name <<": gom "<< column.numOfValue <<" gia tri"<<endl;

		for(int i=0;i<heart_lbs.values.size();i++){
			cout<<"\t "<<heart_lbs.values[i]<<"   ";
		}
		cout<<endl;
		for(int i = 0; i < column.values.size(); i++){
			cout << column.values[i] << ":\t";
				vector<double> temp = column.probabilityOfClass[i];
				for(int k = 0; k < temp.size(); k++){
					cout << temp[k]*100.0<<"%" << "    ";
				}
				cout << endl;
		}
	}

	// test
	int countCorrect = 0;
	double accuracy;

	for(int i=0;i< testset.size();i++) {
		vector<int> x;
		x = testset[i];
		x.pop_back();
		int label = predict(columns,heart_lbs,x);
		if(testset[i][testset[i].size()-1] == label) {
			countCorrect++;
		}
	}

	accuracy = countCorrect*1.0/testset.size()* 100;
	cout<<endl<<"Ti le du doan dung: "<<accuracy<<"%"<<endl;


	int x1,x3,x4,x5,x8,x10;

	//nhap tuoi
	cout<<"=========================";
	cout<<endl<<"Option age: ";
	cout<<endl<<"1:0-37"<<endl<<"2:38-45"<<endl<<"3:46-53"<<endl<<"4:54-61"<<endl<<"5:62-69"<<endl<<"6:69-100"<<endl;
	while (x1 <= 0 || x1 > 6)
	{
		cout<<"Nhap age :";cin>>x1;
	}

	//nhap gioi tinh4
	int x2;
	cout<<"=========================";
	cout<<endl<<"Option sex";
	cout<<endl<<"0: nu"<<endl<<"1:nam"<<endl;
	cout<<"Nhap sex :";cin>>x2;
	(x2!=0) ? x2 = 1 : x2 = 0;

	// nhap cp
	cout<<"=========================";
	cout<<endl<<"Option cp";
	cout<<endl<<"0:Dau that nguc dien hinh"<<endl<<"1:Dau that nguc khong dien hinh"<<endl<<"2:Dau nhung khong dau that nguc nhung khong"<<endl<<"3:Khong co trieu chung"<<endl;
	cout<<"Nhap cp:";cin>>x3;

	//nhap trtbps
	cout<<"=========================";
	cout<<endl<<"Option trtbps";
	cout<<endl<<"1:<120"<<endl<<"2:121-147"<<endl<<"3:148-173"<<endl<<"4:>174"<<endl;
	while (x4<1 || x4>4)
	{
		cout<<"Nhap trtbps:";cin>>x4;
	}

	//nhap chol
	cout<<"=========================";
	cout<<endl<<"Option chol";
	cout<<endl<<"1:<235"<<endl<<"2:236-345"<<endl<<"3:346-454"<<endl<<"4:>455"<<endl;
	while (x5<1 || x5>4)
	{
		cout<<"Nhap chol:";cin>>x5;
	}

	//nhap fbs
	int x6;
	cout<<"=========================";
	cout<<endl<<"Option fbs";
	cout<<endl<<"0:sai"<<endl<<"1:dung"<<endl;
	cout<<"Nhap fbs:";cin>>x6;
	

	//nhap restecg
	int x7;
	cout<<"=========================";
	cout<<endl<<"Option restecg";
	cout<<endl<<"0:Binh thuong"<<endl<<"1:Co song S-T-T bat thuong"<<endl<<"2:Phi dai that trai co the xay ra"<<endl;
	cout<<"Nhap restecg:";cin>>x7;

	//nhap thalachh
	cout<<"=========================";
	cout<<endl<<"Option thalachh";
	cout<<endl<<"1:<103"<<endl<<"2:104-137"<<endl<<"3:138-169"<<endl<<"4:>170"<<endl;
	while (x8<=0 || x8>4)
	{
		cout<<"Nhap thalachh:";cin>>x8;
	}

	//nhap exng
	int x9;
	cout<<"=========================";
	cout<<endl<<"Option exng";
	cout<<endl<<"0:khong"<<endl<<"1:co"<<endl;
	cout<<"Nhap exng:";cin>>x9;

	//nhap oldpeak
	cout<<"=========================";
	cout<<endl<<"Option oldpeak";
	cout<<endl<<"1:<1.55"<<endl<<"2:1.55-3.1"<<endl<<"3:3.2-4.65"<<endl<<"4:>4.65"<<endl;
	while (x10<=0 || x10>4)
	{
		cout<<"Nhap oldpeak:";cin>>x10;
	}

	//nhap slp
	int x11;
	cout<<"=========================";
	cout<<endl<<"Option slp(0-1-2)"<<endl;
	cout<<"Nhap slp:";cin>>x11;


	//nhap caa
	int x12;
	cout<<"=========================";
	cout<<endl<<"Option caa(0-1-2)"<<endl;
	cout<<"Nhap caa:";cin>>x12;
	

	//nhap thal
	int x13;
	cout<<"=========================";
	cout<<endl<<"Option thal(0-1-2-3)"<<endl;
	cout<<"Nhap thal:";cin>>x13;

	vector<int> allPoint;
	allPoint.push_back(x1);
	allPoint.push_back(x2);
	allPoint.push_back(x3);
	allPoint.push_back(x4);
	allPoint.push_back(x5);
	allPoint.push_back(x6);
	allPoint.push_back(x7);
	allPoint.push_back(x8);
	allPoint.push_back(x9);
	allPoint.push_back(x10);
	allPoint.push_back(x11);
	allPoint.push_back(x12);
	allPoint.push_back(x13);

	int lables = predict(columns,heart_lbs,allPoint);
	
	string resultPredict;

	(lables==0) ? resultPredict = "It nguy co dau tim" : resultPredict = "Nhieu nguy co dau tim";

	cout<<endl<<"==============KET QUA=================="<<endl;
	cout << "\t" << resultPredict;
	cout<<endl<<"==============CAM ON=================="<<endl;
    return 0;
}