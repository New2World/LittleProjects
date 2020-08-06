#pragma once

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include <cstring>

#define DBFILE "dbrecord.bin"
#define MAXSIZE 100
#define WIDTHFACTOR1 5
#define WIDTHFACTOR2 2

using namespace std;

class DBM{
    enum primary {ID=0, F_NAME, L_NAME, SSN, AGE, SALARY};

    #ifdef MSVC
    #pragma pack(push,1)
    #endif
    struct dbinfo{
        int emptyBlockChain[MAXSIZE];
        int n_record, empty_block, cur_id;
        primary pkey;
        bool reverse;
    #ifdef GCC
    }__attribute__((packed));
    #else
    };
    #endif
    typedef struct dbinfo dbinfo;

    #ifdef MSVC
    #pragma pack(push,1)
    #endif
    struct employee{
        int id, age;
        float salary;
        char lastName[21], firstName[21];
        char ssn[12];
        bool operator < (const employee& e){
            return id < e.id;
        }
    #ifdef GCC
    }__attribute__((packed));
    #else
    };
    #endif
    typedef struct employee employee;

    vector<int> _Index;
    dbinfo dbInfo;

    void addDB(const employee&);
    bool isEmptyBlock(int);
	int setWidth(string);
    vector<int> _index(primary, bool=false);
    employee makeRecord(string, string, string, int, float, int);

public:
    DBM();
    ~DBM();

    void index(string, bool=false);
    void add(string, string, string, int, float);
    void del(string);
    void display();
    void search(string);
};

DBM::DBM(){
    fstream _dbFile(DBFILE, std::ios::in | std::ios::out | std::ios::binary);
    if(!_dbFile.is_open()){
        cout << "ERROR: database open failed" << endl;
        return;
    }
    dbInfo.n_record = 0;
	dbInfo.cur_id = 1;
    dbInfo.empty_block = 0;
    dbInfo.pkey = ID;
    dbInfo.reverse = false;
    dbInfo.emptyBlockChain[0] = 0;
    if(_dbFile.eof() || !_dbFile.read((char*)&dbInfo, sizeof(dbinfo))){
        _dbFile.seekp(0);
        if(_dbFile.write((char*)&dbInfo, sizeof(dbinfo)))
            cout << "Write" << endl;
        _dbFile.flush();
    }
	employee record;
	while (!_dbFile.eof() && _dbFile.read((char*)&record, sizeof(employee)))
		_Index.push_back(record.id);
    _Index = _index(dbInfo.pkey, dbInfo.reverse);
}

DBM::~DBM(){
    fstream _dbFile(DBFILE, std::ios::in | std::ios::out | std::ios::binary);
    _dbFile.write((char*)&dbInfo, sizeof(dbinfo));
    _dbFile.flush();
    _dbFile.close();
}

bool DBM::isEmptyBlock(int pos){
    if(!dbInfo.empty_block)
        return false;
    for(int i = 0;i < dbInfo.empty_block;i++)
        if(pos == dbInfo.emptyBlockChain[i])
            return true;
    return false;
}

DBM::employee DBM::makeRecord(  string firstName,
                                string lastName,
                                string ssn,
                                int age,
                                float salary,
                                int id){
    employee sample;
    if(firstName.length() > 20 || lastName.length() > 20 || ssn.length() > 11 || age <= 0 || salary < 0)
        throw "invalid input";
    strncpy(sample.firstName, firstName.c_str(), 21);
    strncpy(sample.lastName, lastName.c_str(), 21);
    strncpy(sample.ssn, ssn.c_str(), 12);
    sample.age = age;
    sample.salary = salary;
    sample.id = id;
    return sample;
}

vector<int> DBM::_index(primary field, bool r){
    vector<int> _Id;
    vector<string> pvec_s;
	vector<int> pvec_i;
	vector<float> pvec_f;
    employee record;
    int cnt = 0, pos = 0;
    string key_s;
	int key_i;
	float key_f;
    ifstream _dbFile(DBFILE, std::ios::in | std::ios::binary);
    _dbFile.seekg(sizeof(dbinfo));
	while (!_dbFile.eof() && _dbFile.read((char*)&record, sizeof(employee))) {
		if (isEmptyBlock(cnt)) {
			cnt++;
			continue;
		}
		if (field == ID)
			key_i = record.id;
		else if (field == AGE)
			key_i = record.age;
		else if (field == SALARY)
			key_f = record.salary;
		else if (field == F_NAME)
			key_s = string(record.firstName);
		else if (field == L_NAME)
			key_s = string(record.lastName);
		else if (field == SSN)
			key_s = string(record.ssn);
		if (field == ID || field == AGE) {
			pos = lower_bound(pvec_i.begin(), pvec_i.end(), key_i) - pvec_i.begin();
			pvec_i.insert(pvec_i.begin() + pos, key_i);
		}
		else if (field == SALARY) {
			pos = lower_bound(pvec_f.begin(), pvec_f.end(), key_f) - pvec_f.begin();
			pvec_f.insert(pvec_f.begin() + pos, key_f);
		}
		else {
			pos = lower_bound(pvec_s.begin(), pvec_s.end(), key_s) - pvec_s.begin();
			pvec_s.insert(pvec_s.begin() + pos, key_s);
		}
        _Id.insert(_Id.begin() + pos, record.id);
        cnt++;
    }
    _dbFile.close();
    if(r)
        reverse(_Id.begin(), _Id.end());
    return _Id;
}

void DBM::addDB(const employee& sample){
    int sample_pos;
    sort(dbInfo.emptyBlockChain,
         dbInfo.emptyBlockChain + dbInfo.empty_block,
         std::greater<int>());
    if(dbInfo.empty_block > 0)
        sample_pos = dbInfo.emptyBlockChain[--dbInfo.empty_block];
    else
        sample_pos = dbInfo.n_record;
    fstream _dbFile(DBFILE, std::ios::in | std::ios::out | std::ios::binary);
    _dbFile.seekp(sample_pos * sizeof(employee) + sizeof(dbinfo));
    _dbFile.write((char*)&sample, sizeof(employee));
    _dbFile.flush();
    _dbFile.close();
}

void DBM::add(string firstName, string lastName, string ssn, int age, float salary){
    employee sample;
    try{
        sample = makeRecord(firstName, lastName, ssn, age, salary, dbInfo.cur_id);
    }
    catch(char* error){
        dbInfo.n_record--;
        cout << error << endl;
        return;
    }
    addDB(sample);
	dbInfo.cur_id++;
    dbInfo.n_record++;
    _Index.push_back(sample.id);
    _Index = _index(dbInfo.pkey, dbInfo.reverse);
}

void DBM::del(string ssn){
    employee record;
    ifstream _dbFile(DBFILE, std::ios::in | std::ios::binary);
    _dbFile.seekg(sizeof(dbinfo));
    for(int i = 0;!_dbFile.eof() && _dbFile.read((char*)&record, sizeof(employee));i++){
        if(isEmptyBlock(i))
            continue;
        if(strcmp(record.ssn, ssn.c_str()) == 0){
            dbInfo.emptyBlockChain[dbInfo.empty_block++] = i;
            dbInfo.n_record--;
            break;
        }
    }
    _dbFile.close();
    _Index.erase(find(_Index.begin(), _Index.end(), record.id));
    _Index = _index(dbInfo.pkey, dbInfo.reverse);
}

void DBM::index(string field, bool r){
    primary key = ID;
    transform(field.begin(), field.end(), field.begin(), ::tolower);
    if(field == "age")
        key = AGE;
    else if(field == "salary")
        key = SALARY;
    else if(field == "firstname" || field == "first name")
        key = F_NAME;
    else if(field == "lastname" || field == "last name")
        key = L_NAME;
    else if(field == "ssn")
        key = SSN;
    _Index = _index(key, r);
    dbInfo.pkey = key;
    dbInfo.reverse = r;
}

void DBM::search(string v){
    ifstream _dbFile(DBFILE, std::ios::in | std::ios::binary);
    employee record;
    int cnt = 0;
    _dbFile.seekg(sizeof(dbinfo));
    while(!_dbFile.eof() && _dbFile.read((char*)&record, sizeof(employee))){
        if(isEmptyBlock(cnt)){
            cnt++;
            continue;
        }
        if(string(record.ssn) == v)
            break;
        cnt++;
    }
    cout << endl;
    cout << "------------------------------" << endl;
    if(string(record.ssn) != v)
        cout << "[x] There's no satisfying record" << endl;
    else{
        cout << "  ID: " << record.id << endl;
        cout << "  First Name: " << record.firstName << endl;
        cout << "  Last Name: " << record.lastName << endl;
        cout << "  SSN: " << record.ssn << endl;
        cout << "  Age: " << record.age << endl;
        cout << "  Salary: " << record.salary << endl;
    }
    cout << "------------------------------" << endl;
}

int DBM::setWidth(string field) {
	return field.length() * (field.length() > 4 ? WIDTHFACTOR2 : WIDTHFACTOR1);
}

void DBM::display(){
    int cnt = 0;
    employee record;
    vector<employee> allRecord;
    ifstream _dbFile(DBFILE, std::ios::in | std::ios::binary);
    _dbFile.seekg(sizeof(dbinfo));
    while(!_dbFile.eof() && _dbFile.read((char*)&record, sizeof(employee))){
        if(isEmptyBlock(cnt)){
            cnt++;
            continue;
        }
        allRecord.push_back(record);
        cnt++;
    }
    _dbFile.close();
    sort(allRecord.begin(), allRecord.end());
    vector<string> allField = {"ID", "FirstName", "LastName", "SSN", "Age", "Salary"};
    vector<string> field = {allField[dbInfo.pkey]};
	cout.width(setWidth(allField[dbInfo.pkey]));
    cout << std::right << allField[dbInfo.pkey];
    for(int i = 0;i < 6;i++){
        if(i == dbInfo.pkey)
            continue;
        field.push_back(allField[i]);
		cout.width(setWidth(allField[i]));
        cout << std::right << allField[i];
    }
    cout << endl;
    for(int i = 0;i < dbInfo.n_record;i++){
		for (int j = 0; j < dbInfo.n_record; j++) {
			if (allRecord[j].id == _Index[i]) {
				record = allRecord[j];
				break;
			}
		}
        for(int j = 0;j < 6;j++){
			cout.width(setWidth(field[j]));
            if(field[j] == "ID")
                cout << std::right << record.id;
            else if(field[j] == "FirstName")
                cout << std::right << record.firstName;
            else if(field[j] == "LastName")
                cout << std::right << record.lastName;
            else if(field[j] == "SSN")
                cout << std::right << record.ssn;
            else if(field[j] == "Age")
                cout << std::right << record.age;
            else if(field[j] == "Salary")
                cout << std::right << record.salary;
        }
        cout << endl;
    }
}
