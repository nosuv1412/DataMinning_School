#include <bits/stdc++.h>

using namespace std;

//khai bao thuoc tinh cho lop labels
class labels {
	public:
		int numOflabel; 									//số lượng nhãn
		vector<int> values;									//giá trị nhãn
		vector<int> numOfvalues;							//số lượng giá trị ứng với mỗi nhãn
		vector<double> probability;							//xác xuất tương ứng của nhãn
	public:
		labels(int numOflabel, vector<int> num) {			
			this->numOflabel = numOflabel;					//thiết lập số lượng nhãn
			this->values.resize(numOflabel);				//đặt kích thước cho nhãn
			this->numOfvalues = num;						//gán số lượng giá trị tương ứng
			this->probability.resize(numOflabel);			//đặt kích thước cho vector xác xuất
		}
};

//khai bao thuoc tinh cho lop colums
class column{
	public:
		string name;										//Tên của cột
		int numOfValue;										//Số lượng giá trị khác nhau trong cột	
		int index;											//Chỉ số của cột
		vector<int> values;									//Gia tri cot
		vector<vector<double> > probabilityOfClass;			//vector 2D chứa mỗi giá trị cho từng nhãn
	public:
		column(string name, int index,int numOfValue) {
			this->name = name;
			this->index = index;
			this->numOfValue = numOfValue;
			this->values.resize(numOfValue);				//đặt kích thước cho vector giá trị
		}
};

// lay ra ten cua tat ca cac cot
vector<string> getColumnNames( string str) {				
	string delimiter = ",";									//Dấu phẩy là ký tự phân tách giữa các tên cột
	size_t pos = 0;											//Vị trí của ký tự phân tách trong chuỗi
	string name_str;
	vector<string>names;
	while ((pos = str.find(delimiter)) != string::npos) {	//Duyệt qua chuỗi để tìm và lấy tên cột
	    name_str = str.substr(0, pos);						//Lấy tên cột từ đầu đến vị trí của ký tự phân tách
	    names.push_back(name_str);							//Thêm tên cột vào vector
	    str.erase(0, pos + delimiter.length());				//Thêm tên cột vào vector
	}
	names.push_back(name_str);								//Thêm tên cột cuối cùng vào vector
	return names;
}

//chia tap train va test
void splitTrainTest(vector<vector<int> > inputdata,vector<vector<int> > &trainset, vector<vector<int> > &testset, int trainsize) {
	random_shuffle(inputdata.begin(), inputdata.end());
	random_shuffle(inputdata.begin(), inputdata.end());
	for(int i = 0;i< inputdata.size();i++) {
		// Nếu chỉ số dòng nhỏ hơn trainsize, thêm vào tập huấn luyện, ngược lại thêm vào tập kiểm tra
    	(i < trainsize) ? trainset.push_back(inputdata[i]) : testset.push_back(inputdata[i]);
	}
}


// Lay ra tat ca cac gia tri cua mot thuoc tinh
vector<int> getAllValuesOfColumn(int index, vector< vector<int> > a) {
	set<int> s;
	for(int i=0;i<a.size();i++) {
		// Chèn giá trị từ cột thứ 'index' vào set
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
			// Đếm số lần xuất hiện của giá trị cot'columns.values[k]' và nhãn 'labels.values[m]'
			for(int i=0;i<trainset.size();i++) {
				if(trainset[i][columns.index] == columns.values[k] && trainset[i][trainset[i].size()-1] == labels.values[m]) {
					count_value_with_label++;
				}
			}
			// Tính xác suất có điều kiện và thêm vào vector 'prob'
			prob.push_back(count_value_with_label * 1.0 / labels.numOfvalues[m]);
		}
		columns.probabilityOfClass.push_back(prob);
	}
}

// du doan lop
int predict(vector<column> columns, labels labels, vector<int> x) {
	vector<double> prob;
	prob = labels.probability;

	for(int k=0; k<labels.values.size();k++) {							//Duyệt qua từng nhãn
		for(int i = 0; i < x.size(); i++) {								//Duyệt qua từng đặc trưng của mẫu dữ liệu mới
			for(int j = 0 ;j < columns[i].values.size();j++) {			//Duyệt qua từng giá trị của đặc trưng trong cột tương ứng
				if(columns[i].values[j] == x[i]) {						//Nếu giá trị của đặc trưng trùng với giá trị của mẫu dữ liệu mới
					prob[k] *= columns[i].probabilityOfClass[j][k];		//Cập nhật xác suất bằng cách nhân với xác suất có điều kiện tương ứng
				}

			}
		}
	}
	// Tìm nhãn có xác suất cao nhất
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
    splitTrainTest(inputdata,trainset,testset,trainsize);

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


	//nhap tuoi
	int x1;
	cout<<"=========================";
	cout<<endl<<"Option age: ";
	cout<<endl<<"1:0-37"<<endl<<"2:38-45"<<endl<<"3:46-53"<<endl<<"4:54-61"<<endl<<"5:62-69"<<endl<<"6:69-100"<<endl;
	while (x1 <= 0 || x1 > 6)
	{
		cout<<"Enter your age group: ";cin>>x1;
	}

	//nhap gioi tinh
	int x2;
	cout<<"=========================";
	cout<<endl<<"Option sex";
	cout<<endl<<"0: nu"<<endl<<"1: nam"<<endl;
	cout<<"Enter your sex group: ";cin>>x2;
	(x2!=0) ? x2 = 1 : x2 = 0;

	// nhap cp
	int x3;	
	cout<<"=========================";
	cout<<endl<<"Option cp";
	cout<<endl<<"0: Dau that nguc dien hinh"<<endl<<"1: Dau that nguc khong dien hinh"<<endl<<"2: Dau nhung khong dau that nguc nhung khong"<<endl<<"3: Khong co trieu chung"<<endl;
	cout<<"Enter your cp group: ";cin>>x3;

	//nhap trtbps
	int x4;
	cout<<"=========================";
	cout<<endl<<"Option trtbps";
	cout<<endl<<"1: <120"<<endl<<"2: 121-147"<<endl<<"3: 148-173"<<endl<<"4: >174"<<endl;
	while (x4<1 || x4>4)
	{
		cout<<"Enter your trtbps group: ";cin>>x4;
	}

	//nhap chol
	int x5;
	cout<<"=========================";
	cout<<endl<<"Option chol";
	cout<<endl<<"1: <235"<<endl<<"2: 236-345"<<endl<<"3: 346-454"<<endl<<"4: >455"<<endl;
	while (x5<1 || x5>4)
	{
		cout<<"Enter your chol group: ";cin>>x5;
	}

	//nhap fbs
	int x6;
	cout<<"=========================";
	cout<<endl<<"Option fbs";
	cout<<endl<<"0: sai"<<endl<<"1: dung"<<endl;
	cout<<"Enter your fbs group: ";cin>>x6;
	

	//nhap restecg
	int x7;
	cout<<"=========================";
	cout<<endl<<"Option restecg";
	cout<<endl<<"0: Binh thuong"<<endl<<"1: Co song S-T-T bat thuong"<<endl<<"2: Phi dai that trai co the xay ra"<<endl;
	cout<<"Enter your restecg group: ";cin>>x7;

	//nhap thalachh
	int x8;
	cout<<"=========================";
	cout<<endl<<"Option thalachh";
	cout<<endl<<"1: <103"<<endl<<"2: 104-137"<<endl<<"3: 138-169"<<endl<<"4: >170"<<endl;
	while (x8<=0 || x8>4)
	{
		cout<<"Enter your thalachh group: ";cin>>x8;
	}

	//nhap exng
	int x9;
	cout<<"=========================";
	cout<<endl<<"Option exng";
	cout<<endl<<"0: khong"<<endl<<"1: co"<<endl;
	cout<<"Enter your exng group: ";cin>>x9;

	//nhap oldpeak
	int x10;
	cout<<"=========================";
	cout<<endl<<"Option oldpeak";
	cout<<endl<<"1: <1.55"<<endl<<"2: 1.55-3.1"<<endl<<"3: 3.2-4.65"<<endl<<"4: >4.65"<<endl;
	while (x10<=0 || x10>4)
	{
		cout<<"Enter your oldpeak group: ";cin>>x10;
	}

	//nhap slp
	int x11;
	cout<<"=========================";
	cout<<endl<<"Option slp(0-1-2)"<<endl;
	cout<<endl<<"0: 0"<<endl<<"1: 1"<<endl<<"2: 2"<<endl;
	cout<<"Enter your slp: ";cin>>x11;


	//nhap caa
	int x12;
	cout<<"=========================";
	cout<<endl<<"Option caa(0-1-2)"<<endl;
	cout<<endl<<"0: 0"<<endl<<"1: 1"<<endl<<"2: 2"<<endl;
	cout<<"Enter your caa: ";cin>>x12;
	

	//nhap thal
	int x13;
	cout<<"=========================";
	cout<<endl<<"Option thal(0-1-2-3)"<<endl;
	cout<<endl<<"0: 0"<<endl<<"1: 1"<<endl<<"2: 2"<<endl;
	cout<<"Enter your thal: ";cin>>x13;

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

	cout<<endl<<"==============KET QUA=============="<<endl;
	cout << "|\t" << resultPredict<< "\t   |";
	cout<<endl<<"===================================";

    return 0;
}