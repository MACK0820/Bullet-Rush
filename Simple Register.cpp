//Created by AUSAN / CPE11S1
//CPE102 / 12-11-2024 / FINAL

#include <iostream>
using namespace std;

const int maxRec = 3;
string names[maxRec];
int record = 0;

// function to register name
void registerName()
{
    if (record < maxRec)
    {
        cout << "\nEnter Name to Register: ";
        cin.ignore();
        getline(cin, names[record]);
        record++;
        cout << "Successfully Registered.";
    }
    else
        cout << "\nRegistration Limit Reached!!";
}

// function to display the names
void displayRec()
{
    if (record == 0)
        cout << "\nNo Records Available";
    else
    {
        cout << "\nRegistered Names: " << endl;
        for (int i = 0; i < record; i++)
            cout << i + 1 << ". " << " " << names[i] << endl;
    }
}

int main()
{
    int choice;
    do
    {
        cout << "\nMenu" << endl;
        cout << "1. Register" << endl;
        cout << "2. Display All Records" << endl;
        cout << "3. Exit" << endl;
        cout << "\nEnter Choice: ";
        cin >> choice;

        switch(choice)
        {
            case 1:
                registerName();
                break;
            case 2:
                displayRec();
                break;
            case 3:
                cout << "\nExiting Program....";
                break;
            default:
                cout << "\nInvalid Input!!!";
                break;
        }
    }
    while (choice != 3);
    return 0;
}