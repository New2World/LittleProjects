#include "dbm.hpp"

#define NUM_OP 6
#define INPUT(x) {cout << x; cout.flush();}

using namespace std;

string OPERATION[NUM_OP] = {
    "0. Exit",
    "1. Add record",
    "2. Delete record",
    "3. Find record",
    "4. Index",
    "5. Display all"
};

string outputTitle(){
    cout << endl;
    cout << "##############################" << endl;
    cout << "#                            #" << endl;
    cout << "#      Database Manager      #" << endl;
    cout << "#                            #" << endl;
    cout << "##############################" << endl;
    cout << endl;
    cout << "Operations:" << endl;
    for(int i = 1;i < NUM_OP;i++)
        cout << "  " << OPERATION[i] << endl;
    cout << "  " << OPERATION[0] << endl;
    cout << endl;
    cout << "Operation sequence: ";
    cout.flush();
    string op;
    cin >> op;
    cout << endl;
    return op;
}

int main(){
    DBM db;
    char field[12];
    string firstName, lastName, ssn;
    float salary;
    int age;
    int o;
    bool flag = true;
    string op = outputTitle();
    for(int i = 0;flag && i < op.length();i++){
        o = op[i] - '0';
        cout << OPERATION[o] << endl;
        cout << endl;
        switch(o){
        case 1:
            INPUT("  First Name: ");
            cin >> firstName;
            INPUT("  Last Name: ");
            cin >> lastName;
            INPUT("  SSN: ");
            cin >> ssn;
            INPUT("  Age: ");
            cin >> age;
            INPUT("  Salary: ");
            cin >> salary;
            db.add(firstName, lastName, ssn, age, salary);
            break;
        case 2:
            INPUT("  SSN: ");
            cin >> ssn;
            db.del(ssn);
            break;
        case 3:
            INPUT("  SSN to be search: ");
            cin >> ssn;
            db.search(ssn);
            break;
        case 4:
            INPUT("  Index Field: ");
            getchar();
            cin.getline(field, 12);
            INPUT("  Reverse? [y/N]: ");
            ssn = string("n");
            cin >> ssn;
            if(ssn == "N" || ssn == "n")
                flag = false;
            db.index(string(field), flag);
            flag = true;
            break;
        case 5:
            db.display();
            break;
        case 0:
            flag = false;
            break;
        default:
            cout << "[x] invalid option" << endl;
        }
        if(flag)
            cout << endl;
    }
    return 0;
}
