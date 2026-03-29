#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <string>
#include <limits>
#include <ctime> 
using namespace std;

bool isValidStudentID(const string &id) {
    if (id.length() < 7 || id.length() > 12) {
        return false;  
    }
    
   
    for (char c : id) {
        if (!isalnum(c)) { 
            return false;
        }
    }
    
    return true;
}

bool isNumberOnly(const string &s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit(c)) return false;
    }
    return true;
}

bool isValidMenuChoice(const string &input, int min, int max) {
    if (!isNumberOnly(input)) return false;
    if (input.size() > 1 && input[0] == '0') return false;
    int value = stoi(input);
    return (value >= min && value <= max);
}

bool isValidCalendarDate(int year, int month, int day) {
    if (month < 1 || month > 12 || day < 1) return false;
    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    // Leap year check
    if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
        daysInMonth[1] = 29;
    return day <= daysInMonth[month - 1];
}

struct DateTime {
    string date;
    int hour;
    string str() const {
        return date + " " + to_string(hour) + ":00";
    }
};

struct Appointment {
    string studentName;
    string studentID;
    string reason;
    string counselorName;
    DateTime dt;
    int room;
    string status = "pending"; 
};

struct Counselor {
    string name;
    int id;
    int room;
    vector<Appointment> appointment;
};

struct UndoAction {
    string studentID;
    string actionType;
    Appointment appointment;
};

// Global variables
// --- ARRAY / VECTOR
vector<Counselor> counselors = {
    {"Dr. Reyes", 1, 101},
    {"Ms. Santos", 2, 102},
    {"Mr. Cruz", 3, 103}
};

// --- ARRAY / VECTOR
vector<Appointment> appointments;
// --- QUEUE
queue<Appointment> waitQueue;
// --- STACK
stack<UndoAction> undoStack;

bool isCounselorAvailable(const string &counselorName, const string &date, int hour) {
    for (auto &a : appointments) {
        if (a.counselorName == counselorName &&
            a.dt.date == date &&
            a.dt.hour == hour) {
            return false; // counselor already booked at this date & hour
        }
    }
    return true;
}

void showCounselorSchedule(const Counselor &c) {
    cout << "\n" << c.name << " (Room " << c.room << ")\n";
    cout << "Scheduled:\n";

    bool found = false;
    for (auto &a : appointments) {
        if (a.counselorName == c.name) {
            cout << "   • " << a.dt.date << " at " << a.dt.hour << ":00\n";
            found = true;
        }
    }
    if (!found)
        cout << "   • No appointments\n";
}

// --- LINKED LIST 
struct ListNode {
    Appointment data;
    ListNode* next;
};

ListNode* head = nullptr;

void addAppointmentToLinkedList(const Appointment &a) {
    ListNode* node = new ListNode{a, nullptr};
    if (!head) head = node;
    else {
        ListNode* temp = head;
        while (temp->next) temp = temp->next;
        temp->next = node;
    }
}

void printLinkedListAppointments() {
    ListNode* current = head;
    while (current) {
        cout << current->data.studentName << " " << current->data.dt.str() << "\n";
        current = current->next;
    }
}

// --- New function to remove a node from linked list ---
void removeAppointmentFromLinkedList(const Appointment &ap) {
    ListNode* prev = nullptr;
    ListNode* current = head;
    while (current) {
        if (current->data.studentID == ap.studentID &&
            current->data.dt.date == ap.dt.date &&
            current->data.dt.hour == ap.dt.hour) {
            if (!prev) head = current->next;
            else prev->next = current->next;
            delete current;
            return;
        }
        prev = current;
        current = current->next;
    }
}

// --- TREE / SORTING / SEARCHING
struct BSTNode {
    Appointment data;
    BSTNode* left;
    BSTNode* right;
};

BSTNode* insertBST(BSTNode* root, const Appointment &a) {
    if (!root) return new BSTNode{a, nullptr, nullptr};
    if (a.dt.date < root->data.dt.date || (a.dt.date == root->data.dt.date && a.dt.hour < root->data.dt.hour))
        root->left = insertBST(root->left, a);
    else
        root->right = insertBST(root->right, a);
    return root;
}

void inorderBST(BSTNode* node) {
    if (!node) return;
    inorderBST(node->left);
    cout << "Student: " << node->data.studentName << " | ID: " << node->data.studentID
         << " | Counselor: " << node->data.counselorName << " | Date: " << node->data.dt.str()
         << " | Room: " << node->data.room << " | Reason: " << node->data.reason << "\n";
    inorderBST(node->right);
}

// --- GRAPH 
vector<vector<int>> adj;
int n = 0;

void addEdge(int u, int v) {
    if (u >= adj.size()) adj.resize(u+1);
    if (v >= adj.size()) adj.resize(v+1);
    adj[u].push_back(v);
    adj[v].push_back(u);
}

void buildConflictGraph(const vector<Appointment> &list) {
    n = list.size();
    adj.clear();
    adj.resize(n);
    for (int i = 0; i < n; i++)
        for (int j = i+1; j < n; j++)
            if (list[i].dt.date == list[j].dt.date && list[i].dt.hour == list[j].dt.hour)
                addEdge(i,j);
}

void printConflictGraph(const vector<Appointment> &list) {
    bool conflictFound = false;
    for (int i = 0; i < n; i++) {
        for (int v : adj[i]) {
            if (i < v) {
                cout << "Conflict between:\n";
                cout << list[i].studentName << " at " << list[i].dt.str() << " and "
                     << list[v].studentName << " at " << list[v].dt.str() << "\n";
                conflictFound = true;
            }
        }
    }
    if (!conflictFound) cout << "No overlapping schedules.\n";
}

void printAppointment(const Appointment &a) {
    cout << "Student: " << a.studentName
         << " (" << a.studentID << ") | Counselor: " << a.counselorName
         << " | Time: " << a.dt.str() << " | Room: " << a.room
         << " | Reason: " << a.reason << "\n";
}

bool isSlotTaken(const string &counselor, const DateTime &dt, int room) {
    for (auto &a : appointments)
        if ((a.counselorName == counselor || a.room == room) && a.dt.date == dt.date && a.dt.hour == dt.hour)
            return true;
    return false;
}

bool isValidFutureDate(const string &date) {
    int y, m, d;
    if (sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d) != 3) return false;

    time_t t = time(nullptr);
    t += 8 * 3600;  
    tm* now = gmtime(&t);


    int currY = now->tm_year + 1900;
    int currM = now->tm_mon + 1;
    int currD = now->tm_mday;

    if (y < currY) return false;
    if (y == currY && m < currM) return false;
    if (y == currY && m == currM && d < currD) return false;

    return true;
}

bool hasExistingAppointment(const string &studentID, const DateTime &dt) {
    for (auto &a : appointments) {
        if (a.studentID == studentID && a.dt.date == dt.date && a.dt.hour == dt.hour)
            return true;
    }
    return false;
}

// --- Reason selection 
int chooseReason() {
    int r;
    do {
        cout << "\nChoose reason for counseling:\n";
        cout << "1. Academic\n2. Personal\n3. Career\n4. Others\nChoice: ";
        string input;
        getline(cin, input);
        if (!isNumberOnly(input)) {
            cout << "Invalid input. Enter a number from 1-4.\n";
            continue;
        }
        r = stoi(input);
        if (r < 1 || r > 4) {
            cout << "Invalid choice. Select a number from 1-4.\n";
            continue;
        }
        break;
    } while (true);
    return r;
}

string getReasonString(int r) {
    switch (r) {
        case 1: return "Academic";
        case 2: return "Personal";
        case 3: return "Career";
        case 4: return "Others";
    }
    return "Others";
}

// --- Book Appointment ---
void bookAppointment(string name, string id) {
    cout << "\nChoose counselor:\n";
    for (int i = 0; i < (int)counselors.size(); i++)
        cout << i + 1 << ") " << counselors[i].name << " (Room " << counselors[i].room << ")\n";

    int c;
    while (true) {
        cout << "Enter your choice: ";
        string choiceInput;
        getline(cin, choiceInput);
        if (!isValidMenuChoice(choiceInput, 1, (int)counselors.size())) {
            cout << "Invalid choice. Enter a number between 1 and " << counselors.size() << " without leading zeros.\n";
            continue;
        }
        c = stoi(choiceInput);
        break;
    }

    // --- Get current time ---
    time_t t = time(nullptr);
    t += 8 * 3600;  
    tm* now = gmtime(&t);
    int currentHour = now->tm_hour;
    int currentMin = now->tm_min;

    // --- Valid session slots ---
struct TimeSlot {
    int startHour;
    int endHour;
    string display; // for user-friendly menu
};

vector<TimeSlot> timeSlots = {
    {8, 10, "8:00 AM - 10:00 AM"},
    {10, 12,"10:00 AM - 12:00 PM"},
    {13, 15, "1:00 PM - 3:00 PM"},
    {15, 17, "3:00 PM - 5:00 PM"}
};

    string date;
    int hour;

    while (true) {
        cout << "Enter date (YYYY-MM-DD): ";
        getline(cin, date);
        
        int y, m, d;
        if (sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d) != 3) {
            cout << "Invalid format.\n";
            cout << "Please use: YYYY-MM-DD\n";
            cout << "Example: 2025-11-19\n";
            continue;
        }
        
        if (!isValidCalendarDate(y, m, d)) {
            cout << "That date does not exist.\n";
            cout << "Please enter a valid calendar date.\n";
            cout << "Example: 2025-11-19\n";
            continue;
        }
        
        if (!isValidFutureDate(date)) {
            cout << "You cannot book past dates.\n";
            cout << "Enter a date today or in the future.\n";
            continue;
        }

        // --- Reject past dates ---
        if (y < now->tm_year + 1900 ||
            (y == now->tm_year + 1900 && m < now->tm_mon + 1) ||
            (y == now->tm_year + 1900 && m == now->tm_mon + 1 && d < now->tm_mday)) {
            cout << "Cannot book past date.\n";
            continue;
        }

        cout << "\nChoose session time:\n";
        for (int i = 0; i < timeSlots.size(); i++) {
            cout << i + 1 << ". " << timeSlots[i].display << "\n";
        }

        int slotChoice;
        while (true) {
            cout << "Enter your choice (1-" << timeSlots.size() << "): ";
            string input;
            getline(cin, input);
            if (!isNumberOnly(input)) { 
                cout << "Numbers only.\n"; 
                continue; 
            }
            slotChoice = stoi(input);
            if (slotChoice < 1 || slotChoice > timeSlots.size()) {
                cout << "Invalid choice.\n"; 
                continue;
            }
            break;
        }

// Set the selected slot
hour = timeSlots[slotChoice - 1].startHour;
int endHour = timeSlots[slotChoice - 1].endHour;


        // --- Check if the hour is valid slot ---
        bool validSlot = false;
        for (auto &slot : timeSlots) {
            if (hour == slot.startHour) {
                validSlot = true;
                break;
            }
        }
        if (!validSlot) {
            cout << "Invalid start hour. Must be 8, 10, 13, or 15.\n";
            continue;
        }

        // --- Reject past/ongoing slots today ---
        if (y == now->tm_year + 1900 && m == now->tm_mon + 1 && d == now->tm_mday) {
            for (auto &slot : timeSlots) {
                if (slot.startHour == hour && slot.endHour <= currentHour) {
                    cout << "Cannot book this slot today - time already passed.\n";
                    validSlot = false;
                    break;
                }
                if (slot.startHour == hour && slot.startHour <= currentHour && currentHour < slot.endHour) {
                    cout << "Cannot book this slot today - slot already started.\n";
                    validSlot = false;
                    break;
                }
            }
            if (!validSlot) continue;
        }

        DateTime dtTemp{date, hour};
        if (hasExistingAppointment(id, dtTemp)) {
            cout << "You already have an appointment at this date and hour.\n";
            continue;
        }

        break; 
    }

    int reasonChoice = chooseReason();
    string reason = getReasonString(reasonChoice);

    Appointment ap;
    ap.studentName = name;
    ap.studentID = id;
    ap.reason = reason;
    ap.counselorName = counselors[c-1].name;
    ap.dt = {date, hour};
    ap.room = counselors[c-1].room;
    ap.status = "pending";


    // --- Check if slot is taken ---
    bool slotTaken = false;
    for (auto &slot : timeSlots) {
        if (slot.startHour == hour) {
            for (int h = slot.startHour; h < slot.endHour; h++) {
                DateTime dtCheck{date, h};
                if (isSlotTaken(ap.counselorName, dtCheck, ap.room)) {
                    slotTaken = true;
                    break;
                }
            }
        }
    }

    if (slotTaken) {
        cout << "Slot already taken. Added to waiting queue.\n";
        waitQueue.push(ap);
    } else {
        appointments.push_back(ap);
        addAppointmentToLinkedList(ap);
        undoStack.push({id, "book", ap});
        cout << "Appointment Booked!\n";
        printAppointment(ap);

        // Print session duration
        string sessionDisplay;
        for (auto &slot : timeSlots) {
            if (slot.startHour == hour) {
                sessionDisplay = slot.display;
                break;
            }
        }
        cout << "Session duration: " << sessionDisplay << "\n";
    }
}

void viewAppointments(string id) {
    bool found = false;
    for (auto &a : appointments)
        if (a.studentID == id) {
            printAppointment(a);
            found = true;
        }
    if (!found)
        cout << "No appointments found.\n";
}

void cancelAppointment(string id) {
    string date;
    int hour;

    cout << "Enter date (YYYY-MM-DD): ";
    getline(cin, date);
    
    int y, m, d;
    if (sscanf(date.c_str(), "%d-%d-%d", &y, &m, &d) != 3) {
        cout << "Invalid format.\n";
        cout << "Please use: YYYY-MM-DD\n";
        cout << "Example: 2025-11-19\n";
        return;
    }
    
    if (!isValidCalendarDate(y, m, d)) {
        cout << "That date does not exist.\n";
        cout << "Please enter a valid calendar date.\n";
        cout << "Example: 2025-11-19\n";
        return;
    }
    
    if (!isValidFutureDate(date)) {
        cout << "You cannot book past dates.\n";
        cout << "Enter a date today or in the future.\n";
        return;
    }


    cout << "Enter session start hour (8, 10, 13, 15): ";
    string hourInput;
    getline(cin, hourInput);
    if (!isNumberOnly(hourInput)) {
        cout << "Invalid hour. Numbers only.\n";
        return;
    }
    hour = stoi(hourInput);
    if (hour != 8 && hour != 10 && hour != 13 && hour != 15) {
        cout << "Invalid session hour. Must be 8, 10, 13, or 15.\n";
        return;
    }

    bool found = false;
    for (auto it = appointments.begin(); it != appointments.end(); ++it) {
        if (it->studentID == id && it->dt.date == date && it->dt.hour == hour) {
            undoStack.push({id, "cancel", *it});
            removeAppointmentFromLinkedList(*it);
            appointments.erase(it);
            cout << "Appointment canceled.\n";
            found = true;
            break;
        }
    }
    if (!found)
        cout << "No matching appointment found.\n";
}

void undoLastAction(const string &studentID) {
    if (undoStack.empty()) {
        cout << "Nothing to undo.\n";
        return;
    }

    // Find last action for this student
    stack<UndoAction> tempStack;
    UndoAction target;
    bool found = false;

    while (!undoStack.empty()) {
        UndoAction top = undoStack.top();
        undoStack.pop();
        if (top.studentID == studentID && !found) {
            target = top;
            found = true;
            break;
        } else {
            tempStack.push(top);
        }
    }

    while (!tempStack.empty()) {
        undoStack.push(tempStack.top());
        tempStack.pop();
    }

    if (!found) {
        cout << "No actions to undo for this student.\n";
        return;
    }

    if (target.actionType == "book") {
        auto it = remove_if(appointments.begin(), appointments.end(),
            [&](const Appointment &a) {
                return a.studentID == target.appointment.studentID &&
                       a.dt.date == target.appointment.dt.date &&
                       a.dt.hour == target.appointment.dt.hour;
            });
        if (it != appointments.end()) {
            removeAppointmentFromLinkedList(target.appointment); // remove from linked list
            appointments.erase(it, appointments.end());
            cout << "Undo: booking removed.\n";
        } else {
            cout << "Nothing to remove - maybe it was already canceled.\n";
        }
    } else if (target.actionType == "cancel") {
        appointments.push_back(target.appointment);
        addAppointmentToLinkedList(target.appointment); // restore to linked list
        cout << "Undo: canceled appointment restored.\n";
    }
}

Appointment autoAssignAppointment(string name, string id, string reason);

bool isStudentIDUsed(const string &id) {
    for (auto &a : appointments) {
        if (a.studentID == id)
            return true;
    }
    return false;
}

bool isValidStudentName(const string &name) {
    if (name.empty()) return false;
    
    for (char c : name) {
        if (!isalpha(c) && c != ' ' && c != '.') {
                return false;
        }
    }
    
    if (name.length() < 5) return false;
    
    bool hasVowel = false;
    for (char c : name) {
        char lower = tolower(c);
        if (lower=='a'||lower=='e'||lower=='i'||lower=='o'||lower=='u') {
                hasVowel = true;
            break;
        }
    }
    if (!hasVowel) return false;
    
    string lower;
    for (char c : name) lower += tolower(c);
    
    int repeatCount = 1;
    for (int i = 1; i < lower.size(); i++) {
        if (lower[i] == lower[i - 1]) repeatCount++;
        else repeatCount = 1;
    
        if (repeatCount >= 3) return false;
    }
    
    if (name.find(' ') == string::npos) return false;
    
    vector<string> words;
    string word;
    for (char c : lower) {
        if (c == ' ') {
            if (!word.empty()) words.push_back(word);
            word.clear();
        } else word += c;
    }
    if (!word.empty()) words.push_back(word);
    
    bool allSame = true;
    for (int i = 1; i < words.size(); i++) {
        if (words[i] != words[0]) {
                allSame = false;
            break;
        }
    }
    if (allSame) return false;
    
    return true;
}

// --- Student Dashboard ---
void studentDashboard() {
    string name, id;
    
    do {
        cout << "Enter your full name: ";
        getline(cin, name);
    
        if (!isValidStudentName(name)) {
            cout << "Invalid name. Please enter a real full name (letters, spaces, periods only).\n";
        } else {
            break;
        }
    } while (true);

    do {
        cout << "Enter your student ID (7-12 letters or numbers): ";
        string idInput;
        getline(cin, idInput);
    
        if (!isValidStudentID(idInput)) {
            cout << "Invalid ID. It must be 7–12 characters long and contain only letters and numbers.\n";
        }
        else if (isStudentIDUsed(idInput)) {
            cout << "This student ID already exists. Please enter a unique ID.\n";
        }
        else {
            id = idInput;
            break;
        }

    } while (true);


    char ch;
    do {
        cout << "Is this your first time? (y/n): ";
        cin >> ch;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (ch == 'y' || ch == 'Y' || ch == 'n' || ch == 'N')
            break;
        cout << "Invalid input. Please enter 'y' or 'n'.\n";
    } while (true);

    if (ch == 'y' || ch == 'Y') {
    int reasonChoice = chooseReason();
    string reason = getReasonString(reasonChoice);
    cout << "Auto-assigning your appointment...\n";
    autoAssignAppointment(name, id, reason); 
    return;
}

    int choice;
    do {
        cout << "\n--- Student Menu ---\n";
        cout << "1. Book Appointment\n2. View My Appointments\n3. Cancel Appointment\n4. Undo Last Action\n5. Back\nChoice: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Enter a number from 1-5.\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (choice == 1) bookAppointment(name, id);
        else if (choice == 2) viewAppointments(id);
        else if (choice == 3) cancelAppointment(id);
        else if (choice == 4) undoLastAction(id);
        else if (choice != 5)
            cout << "Invalid choice. Select from 1-5.\n";
    } while (choice != 5);
}

Appointment autoAssignAppointment(string name, string id, string reason) {
    Appointment ap;
    ap.studentName = name;
    ap.studentID = id;
    ap.reason = reason;

    time_t t = time(nullptr); 
    t += 8 * 3600;  

    tm* now = gmtime(&t);  


    int currentHour = now->tm_hour; 
    int currentMin = now->tm_min;

    // Format date (YYYY-MM-DD)
    char buffer[11];
    sprintf(buffer, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
    string today = buffer;

    // --- Define working time slots (4 sessions/day, 2 hours each) ---
    struct TimeSlot {
        int startHour;
        int endHour;
        string display;
    };
    
    vector<TimeSlot> timeSlots = {
        {8, 10, "8:00 AM - 10:00 AM"},
        {10, 12, "10:00 AM - 12:00 PM"},
        {13, 15, "1:00 PM - 3:00 PM"},
        {15, 17, "3:00 PM - 5:00 PM"}
    };


    // --- Assigning the next valid real-time slot ---
    for (auto &c : counselors) {
        for (auto &slot : timeSlots) {
            int startHour = slot.startHour;
            int endHour = slot.endHour;

            // Skip past or ongoing slots
            if (endHour <= currentHour) continue;
            if (startHour <= currentHour && currentHour < endHour) continue;

            // Check if both hours are free
            bool slotFree = true;
            for (int h = startHour; h < endHour; h++) {
                DateTime dtCheck{today, h};
                if (isSlotTaken(c.name, dtCheck, c.room)) {
                    slotFree = false;
                    break;
                }
            }

            if (slotFree) {
                // Assign appointment
                DateTime dt{today, startHour};
                ap.counselorName = c.name;
                ap.room = c.room;
                ap.dt = dt;

                appointments.push_back(ap);
                addAppointmentToLinkedList(ap);
                undoStack.push({id, "book", ap});

                cout << "\nAuto-assigned appointment:\n";
                printAppointment(ap);
                cout << "Session duration: " << startHour << ":00-" << endHour << ":00\n";
                cout << "(True local time now: " << currentHour << ":" 
                     << (currentMin < 10 ? "0" : "") << currentMin << ")\n";
                return ap;
            }
        }
    }

    // --- If no slot today ---
    now->tm_mday += 1; // move to next day
    mktime(now);
    sprintf(buffer, "%04d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
    string tomorrow = buffer;

    for (auto &c : counselors) {
        for (auto &slot : timeSlots) {
            bool slotFree = true;
            for (int h = slot.startHour; h < slot.endHour; h++) {
                DateTime dtCheck{tomorrow, h};
                if (isSlotTaken(c.name, dtCheck, c.room)) {
                    slotFree = false;
                    break;
                }
            }
            if (slotFree) {
                DateTime dt{tomorrow, slot.startHour};
                ap.counselorName = c.name;
                ap.room = c.room;
                ap.dt = dt;

                appointments.push_back(ap);
                addAppointmentToLinkedList(ap);
                undoStack.push({id, "book", ap});

                cout << "\nNo slots today - scheduled for tomorrow:\n";
                printAppointment(ap);
                cout << "Session duration: " << slot.startHour << ":00-" << slot.endHour << ":00\n";
                cout << "(True local time now: " << currentHour << ":" 
                     << (currentMin < 10 ? "0" : "") << currentMin << ")\n";
                return ap;
            }
        }
    }

    ap.counselorName = "Pending";
    ap.room = 0;
    ap.dt = {"Pending", 0};
    waitQueue.push(ap);

    cout << "\n No available slots today or tomorrow. Added to waiting queue.\n";
    printAppointment(ap);
    return ap;
}

// --- Check Conflicts ---
void checkConflicts(const string &counselor) {
    vector<Appointment> list;
    for (auto &a : appointments)
        if (a.counselorName == counselor)
            list.push_back(a);

    if (list.empty()) {
        cout << "No appointments for " << counselor << ".\n";
        return;
    }

    cout << "\nAppointments for " << counselor << ":\n";
    for (auto &a : list)
        printAppointment(a);

    buildConflictGraph(list);
    printConflictGraph(list);
}

// --- Manage Pending Appointments ---
void manageAppointments(const string &counselorName) {
    while (true) {
        vector<int> pendingIndexes;

        // List all pending appointments for this counselor
        for (int i = 0; i < appointments.size(); i++) {
            if (appointments[i].counselorName == counselorName &&
                appointments[i].status == "pending") {
                pendingIndexes.push_back(i);
            }
        }

        if (pendingIndexes.empty()) {
            cout << "No pending appointments to manage.\n";
            return;
        }

        // Print pending appointments
        for (int j = 0; j < pendingIndexes.size(); j++) {
            int i = pendingIndexes[j];
            cout << j + 1 << ". ";
            printAppointment(appointments[i]);
        }

        cout << "Enter the number of the appointment to manage (0 to exit): ";
        string s;
        getline(cin, s);
        if (!isNumberOnly(s)) { cout << "Numbers only.\n"; continue; }

        int choice = stoi(s);
        if (choice == 0) return;
        if (choice < 1 || choice > pendingIndexes.size()) {
            cout << "Invalid choice.\n"; 
            continue;
        }

        int idx = pendingIndexes[choice - 1];

        cout << "1. Accept appointment\n2. Transfer appointment\n3. Back\nChoice: ";
        string actionInput;
        getline(cin, actionInput);
        if (!isNumberOnly(actionInput)) { cout << "Invalid input.\n"; continue; }

        int action = stoi(actionInput);

        if (action == 1) {
            appointments[idx].status = "accepted";
            cout << "Appointment accepted.\n";
        }

        
        else if (action == 2) {
            cout << "\nChoose counselor to transfer to:\n";

            vector<int> options;
            int display = 1;

            for (int i = 0; i < counselors.size(); i++) {
                if (counselors[i].name != counselorName) {
                    options.push_back(i);
                    cout << display << ") ";
                    showCounselorSchedule(counselors[i]);
                    display++;
                }
            }

            // Choose counselor
            int transferChoice;
            while (true) {
                cout << "\nEnter your choice: ";
                string inp;
                getline(cin, inp);
                if (!isNumberOnly(inp)) { cout << "Numbers only.\n"; continue; }

                transferChoice = stoi(inp);
                if (transferChoice < 1 || transferChoice > options.size()) {
                    cout << "Invalid choice.\n"; 
                    continue;
                }
                break;
            }

            // Selected counselor index
            int counselorIndex = options[transferChoice - 1];
            string newCounselor = counselors[counselorIndex].name;
            int newRoom = counselors[counselorIndex].room;

            string date = appointments[idx].dt.date;
            int hour = appointments[idx].dt.hour;

            // Check availability
            if (!isCounselorAvailable(newCounselor, date, hour)) {
                cout << "\nCannot transfer appointment.\n";
                cout << newCounselor << " is NOT available on " << date 
                     << " at " << hour << ":00.\n";
                cout << "Please pick another counselor or schedule.\n\n";
                return;
            }

            // Transfer appointment
            appointments[idx].counselorName = newCounselor;
            appointments[idx].room = newRoom;
            appointments[idx].status = "pending";

            cout << "Appointment successfully transferred to " 
                 << newCounselor << ".\n";
        }

        else if (action == 3) return;
        else cout << "Invalid choice.\n";
    }
}

void manageQueue(queue<Appointment>& q, vector<Counselor>& counselors) {
    if (q.empty()) {
        cout << "--- Waiting Queue is empty ---\n";
        return;
    }

    cout << "--- Waiting Queue ---\n";
    int idx = 1;
    vector<Appointment> tempQueue;
    while (!q.empty()) {
        Appointment a = q.front(); q.pop();
        cout << idx << ". Student: " << a.studentName
             << " (" << a.studentID << ") | Counselor: " << a.counselorName
             << " | Time: " << a.dt.date << " " << a.dt.hour << ":00"
             << " | Room: " << a.room
             << " | Reason: " << a.reason << "\n";
        tempQueue.push_back(a);
        idx++;
    }

    for (auto& a : tempQueue) q.push(a);

    int selection;
    while (true) {
        cout << "Enter the number of the appointment to manage (0 to exit): ";
        string s;
        getline(cin, s);
        if (!isNumberOnly(s)) { cout << "Numbers only.\n"; continue; }
        selection = stoi(s);
        if (selection < 0 || selection > tempQueue.size()) {
            cout << "Invalid choice.\n";
            continue;
        }
        break;
    }

    if (selection == 0) return;

    Appointment current = tempQueue[selection - 1];

    cout << "1. Accept appointment\n";
    cout << "2. Transfer to another counselor\n";
    cout << "3. Back\n";

    int action;
    while (true) {
        cout << "Choice: ";
        string sa;
        getline(cin, sa);
        if (!isNumberOnly(sa)) { cout << "Numbers only.\n"; continue; }
        action = stoi(sa);
        if (action < 1 || action > 3) { cout << "Invalid choice.\n"; continue; }
        break;
    }
    if (action == 1) { 
    
        for (auto& c : counselors) {
            if (c.name == current.counselorName) {
                c.appointment.push_back(current); 
                break;
            }
        }
    
        queue<Appointment> newQueue;
        for (int i = 0; i < tempQueue.size(); i++) {
            if (i != selection - 1) newQueue.push(tempQueue[i]);
        }
        q = newQueue;

    
        cout << "Appointment accepted and moved to confirmed appointments.\n\n";
        return; 
    }


    if (action == 2) {
        cout << "Choose counselor to transfer to:\n";
        vector<int> options;
        for (int i = 0; i < counselors.size(); i++) {
            if (counselors[i].name != current.counselorName) {
                options.push_back(i);
                cout << options.size() << ") " << counselors[i].name
                     << " (Room " << counselors[i].room << ")\n";
            }
        }

        if (options.empty()) {
            cout << "No other counselors available.\n";
            return;
        }

        int cChoice;
        while (true) {
            cout << "Enter choice: ";
            string s;
            getline(cin, s);
            if (!isNumberOnly(s)) { cout << "Numbers only.\n"; continue; }
            cChoice = stoi(s);
            if (cChoice < 1 || cChoice > options.size()) {
                cout << "Invalid choice.\n";
                continue;
            }
            break;
        }

        int selectedIndex = options[cChoice - 1];
        string newCounselor = counselors[selectedIndex].name;

        // Check if counselor has any appointment
        bool hasAppointments = false;
        for (auto& a : tempQueue) {
            if (a.counselorName == newCounselor) {
                hasAppointments = true;
                break;
            }
        }

        if (!hasAppointments) {
            cout << newCounselor << " has no appointments yet.\n";
        } else if (!isCounselorAvailable(newCounselor, current.dt.date, current.dt.hour)) {
            cout << "\nCannot transfer.\n";
            cout << newCounselor << " already has an appointment on "
                 << current.dt.date << " at " << current.dt.hour << ":00.\n";
            cout << "Transfer canceled.\n\n";
            return;
        }

        // Transfer normally
        current.counselorName = newCounselor;
        current.room = counselors[selectedIndex].room;

        // Remove old entry and push new one to queue
        queue<Appointment> newQueue;
        for (int i = 0; i < tempQueue.size(); i++) {
            if (i == selection - 1) newQueue.push(current);
            else newQueue.push(tempQueue[i]);
        }
        q = newQueue;

        cout << "Appointment transferred in the queue to "
             << newCounselor << ".\n\n";
    }
}

// --- Counselor Dashboard ---
void counselorDashboard() {
    int menuChoice;
    string name;
    do {
        cout << "\nLogging in as:\n";
        for (int i = 0; i < (int)counselors.size(); i++)
            cout << i + 1 << ") " << counselors[i].name << " (Room " << counselors[i].room << ")\n";
        cout << "Enter your choice: ";
        string choiceInput;
        getline(cin, choiceInput);
        if (!isNumberOnly(choiceInput)) { cout << "Invalid input. Numbers only.\n"; continue; }
        menuChoice = stoi(choiceInput);
        if (menuChoice < 1 || menuChoice > (int)counselors.size()) {
            cout << "Invalid choice. Select a valid number.\n"; continue;
        }
        name = counselors[menuChoice - 1].name;
        break;
    } while (true);

    do {
        cout << "\nCounselor Menu (" << name << ")\n";
        cout << "1. View My Appointments \n";
        cout << "2. View All Appointments\n";
        cout << "3. Check Conflicts\n";
        cout << "4. Manage Pending Appointments\n";
        if (!waitQueue.empty()) cout << "5. Manage Waiting Queue\n6. Back\nChoice: ";
        else cout << "5. Back\nChoice: ";

        string input;
        getline(cin, input);
        if (!isNumberOnly(input)) { cout << "Numbers only.\n"; continue; }
        menuChoice = stoi(input);

        if (menuChoice == 1) {
            BSTNode* root = nullptr;
            bool found = false;
            for (auto &a : appointments) {
                if (a.counselorName == name) {
                    root = insertBST(root, a);
                    found = true;
                }
            }
            if (found) inorderBST(root);
            else cout << "No appointments found for " << name << ".\n";

        } else if (menuChoice == 2) {
            if (appointments.empty()) cout << "No appointments.\n";
            else for (auto &a : appointments) printAppointment(a);

        } else if (menuChoice == 3) checkConflicts(name);
        else if (menuChoice == 4) manageAppointments(name);
        else if (menuChoice == 5 && !waitQueue.empty()) 
            manageQueue(waitQueue, counselors); 
        else if ((menuChoice == 5 && waitQueue.empty()) || menuChoice == 6) break;
        else cout << "Invalid choice.\n";
    } while (true);
}

// --- MAIN ---
int main() {
    cout << "GUIDANCE COUNSELING APPOINTMENT SYSTEM\n";
    int menuChoice;
    while (true) {
        cout << "\nAre you a:\n1. Student\n2. Counselor\n3. Exit\nChoice: ";
        string menuInput;
        getline(cin, menuInput);
        if (!isValidMenuChoice(menuInput, 1, 3)) {
            cout << "Invalid input. Please enter a number between 1 and 3 without leading zeros.\n";
            continue;
        }
        menuChoice = stoi(menuInput);


        if (menuChoice == 1) studentDashboard();
        else if (menuChoice == 2) counselorDashboard();
        else if (menuChoice == 3) {
            cout << "Thank you! Come back anytime.\n";
            break;
        } else cout << "Invalid choice.\n";
    }
    return 0;
}
