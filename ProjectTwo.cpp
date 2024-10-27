//============================================================================
// Name        : HashTable.cpp
// Author      : Felix Guzman
// Version     : 7.0 (yes actually 6 versions before this one!)
// Description : 7-1 Project Two
//============================================================================


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>


/*
* For this assignment I decided to look at both my latest pseudocode (final project) and my intial pseudocode for Hash Tables
* I decided to go with a Hash Table for the reasons I outlined in the previous assignment and decided to combine my pseudocodes into one
* This was not easy despite having an idea for the structure a lot of the things I previously planned were harder to implement.
* Overall, I tried to maintain and stay faithful to my previous pseudocodes which I do quite well, and add some extra functionality
* and checks. Stuff to make it easier for the user that wouldn't understand the requirements or my thinking when designing this.
* To use this just place the csv file (make sure it's named appropriately (it is here) in the same folder or project in visual studio
* and then run. It should work perfectly. I made sure this satisfies all requirements, follows my previous pseudocode, kept the
* code simple and readable, and added my own touch. That's all!

*/

using namespace std;

// we have 8 courses just doubled it but could be changed based on efficiency or course size needs

const int HASH_TABLE_SIZE = 16;

struct Course { // just setting up our code, struct makes sense in this context more than classes or anything else
    string courseNum;
    string title;
    vector<string> prerequisites;
    Course *next;
    Course() : next(nullptr) {} // here is our next pointer set to nullptr will play a part later
};

Course *hashTable[HASH_TABLE_SIZE] = {nullptr};

bool isDataLoaded = false; // let's approach this with flagging like always...

// here is our hash function to compute index 

int hashFunction(const string &courseNum) {
    int hashValue = 0;
    for (char ch : courseNum) {
        hashValue = (hashValue + ch) % HASH_TABLE_SIZE; // this is based on the file ascii codes so we sum / add mod
    }
    return hashValue;
}

// here we create our course object with the desired parameters

Course *createCourse(string courseNum, string courseTitle, vector<string> prerequisites) {
    Course *course = new Course;
    course->courseNum = courseNum;
    course->title = courseTitle;
    course->prerequisites = prerequisites;
    return course;
}

// here is our insertCourse function to handle collisions/chaining

void insertCourse(Course *course) {
    int key = hashFunction(course->courseNum);
    if (!hashTable[key]) {
        hashTable[key] = course;
    }
    else {
        Course *current = hashTable[key];
        while (current->next) {
            current = current->next;
        }
        current->next = course;
    }
}

// now we load the courses from file in the loadCourses function

void loadCourses(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error! Could not open file!" << endl;
        return;
    }

    string line;
    while (getline(file, line)) {
        vector<string> courseInfo;
        istringstream iss(line); // very helpful built in function to avoid complicating the structure
        string data;

        while (getline(iss, data, ',')) {
            courseInfo.push_back(data);
        }

        if (courseInfo.size() < 2) {
            cout << "Error! Wrong data format!" << endl;
            continue; // this actually simplifies the process and organizes the coding structure further
        }

        string courseNum = courseInfo[0];
        string courseTitle = courseInfo[1];
        vector<string> prerequisites(courseInfo.begin() + 2, courseInfo.end());

        transform(courseNum.begin(), courseNum.end(), courseNum.begin(), ::toupper);
        Course *course = createCourse(courseNum, courseTitle, prerequisites);

        insertCourse(course);  // will use for collisions
    }

    file.close();
    isDataLoaded = true;
    cout << "Courses have been loaded successfully!" << endl;
}

// our search function, works just like desired, will display details if course exists 

void searchCourse(const string &courseNum) {
    string searchKey = courseNum;
    transform(searchKey.begin(), searchKey.end(), searchKey.begin(), ::toupper);
    int key = hashFunction(searchKey);
    Course *current = hashTable[key];

    while (current) {
        if (current->courseNum == searchKey) {
            cout << "Course: " << current->courseNum << ", " << current->title << endl;
            cout << "Prerequisites: ";
            if (current->prerequisites.empty()) {
                cout << "None";
            }
            else {
                for (size_t i = 0; i < current->prerequisites.size(); ++i) {
                    cout << current->prerequisites[i];
                    if (i < current->prerequisites.size() - 1) cout << ", ";
                }
            }
            cout << endl;
            return;
        }
        current = current->next;
    }
    cout << "Course " << courseNum << " was not found." << endl;
}

// displaying all courses in the desired alphanumeric order

void printCourseList() {
    if (!isDataLoaded) { // added an extra check that makes the program more logical and easier for the user
        cout << "You haven't loaded the file, load it and try again." << endl;
        return;
    }

    vector<string> keys;

    for (int i = 0; i < HASH_TABLE_SIZE; ++i) {
        Course *current = hashTable[i];
        while (current) {
            keys.push_back(current->courseNum);
            current = current->next;
        }
    }

    sort(keys.begin(), keys.end());

    for (const string &key : keys) {
        Course *course = hashTable[hashFunction(key)];
        while (course && course->courseNum != key) {
            course = course->next;
        }
        if (course) {
            cout << "Course: " << course->courseNum << ", " << course->title << endl;
        }
    }
}

// here is our menu display function, displays everything needed and outlined in our requirements

void displayMenu() {
    bool whileRunning = true;
    while (whileRunning) {

        cout << "1: Load course data" << endl;
        cout << "2: Print course list" << endl;
        cout << "3: Search course by number" << endl;
        cout << "9: Exit" << endl;

        cout << "What would you like to do? ";

        int choice;
        cin >> choice;

        // input validation for menu choice with checks, clears, and so on for user needs and possible errors

        while (cin.fail()) {
            cout << "Invalid input! Please enter a valid option: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // this fixes the inf loop and format issue
            cin >> choice;
        }

        // our switch statement, has some slight modifications but similar to previous assignments

        switch (choice) {
        case 1:
            loadCourses("CS 300 ABCU_Advising_Program_Input.csv");
            break;
        case 2:
            printCourseList();
            break;
        case 3: {
            cout << "Enter course number: ";
            string courseNum;
            cin >> courseNum;
            searchCourse(courseNum);
            cout << "Remember if prerequisites are blank or not displayed, there are none!" << endl; // for user, won't change code struc for this
            break;
        }
        case 9:
            cout << "Thank you for using the course planner!" << endl;
            whileRunning = false;
            cin.get();
            break;
        default:
            cout << choice << " is not a valid option. Please try again." << endl;
            break;
        }
    }
}

int main() {
    cout << "Welcome to the course planner." << endl; // FINALLY DONE! UPDATE!!!
    displayMenu();
    return 0;
}
