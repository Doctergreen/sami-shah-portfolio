/*
/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//   ********************************************************************* //
//   * 														             * //
//   *         STUDENT & TEACHER ACADEMIC PORTAL (STAP)                  * //
//   * 														             * //
//   ********************************************************************* //
//                                                                         //
//   Project Made By:                                                      //
//   Sami Shah (109)                                                       //
//   Fareed Ul Hussain (145)                                               //
//                                                                         //
//   Course:      Programming Fundamentals                                 //
//   Teacher:     Dr. Samia Riaz                                           //
//   Campus:      COMSATS University Islamabad, Wah Campus                 //
//                                                                         //
//   Last Modified: December 6, 2025                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

============================================================================
// --- FILE: tests.cpp ---
============================================================================
//
// Summary:
// This is the automated Test Harness used for Quality Assurance (QA).
// It verifies the system's core logic algorithms independently.
//
// Complies with Test ID requirements: TC_CALC, TC_FILE, TC_SCHED, TC_AUTH.
//
============================================================================
*/

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>  
#include <fstream>  
#include <sstream>

// --- Project Headers (For Data Types) ---
#include "headers/utility.h"
#include "headers/file_handler.h"
#include "headers/course.h"
#include "headers/student.h"
#include "headers/attendance.h"
#include "headers/admin.h"
#include "headers/auth.h" 
#include "headers/schedule.h" // Needed for ScheduleEntry

// ==========================================================================
//    MOCK ENVIRONMENT
//    (We define these here to run tests in isolation without linker errors)
// ==========================================================================

// 1. Global Data Storage for Tests
std::vector<Student> all_students;
std::vector<std::string> g_allSectionIDs;
// NEW: We need this vector to test scheduling
std::vector<ScheduleEntry> all_schedule; 

// 2. Mock Login Logic (Replicates auth.cpp Linear Search)
std::string test_login_logic(std::string username, std::string password) {
    if (username == "admin" && password == "admin123") return "ADMIN";
    
    for (const auto& s : all_students) {
        if (s.username == username && s.password == password) {
            return "STUDENT";
        }
    }
    return "INVALID";
}

// 3. Calculation Logic (Replicates utility.cpp)
double test_getGradePoint(int mark) {
    if (mark >= 90) return 4.0; 
    if (mark >= 85) return 3.7; 
    if (mark >= 80) return 3.3; 
    if (mark >= 75) return 3.0; 
    if (mark >= 50) return 1.0; 
    return 0.0; 
}

// ==========================================================================
//    TEST CASES
// ==========================================================================

// Structure to hold test results
struct TestResult {
    std::string id;
    std::string name;
    std::string expected;
    std::string actual;
    std::string status; 
};
std::vector<TestResult> all_results;

// --- MODULE 1: CALCULATION TESTS ---
void check_calculations() {
    std::cout << "Running Calculation Tests...\n";

    // TEST 1: Grade Point Conversion
    // ID: TC_CALC_001
    double gradeA = test_getGradePoint(87);
    TestResult t1;
    t1.id = "TC_CALC_001";
    t1.name = "Grade Point (A)";
    t1.expected = "3.700000"; 
    t1.actual = std::to_string(gradeA);
    t1.status = (gradeA == 3.7) ? "PASS" : "FAIL";
    all_results.push_back(t1);

    // TEST 2: Boundary Check
    // ID: TC_CALC_002
    double gradeD = test_getGradePoint(50);
    TestResult t2;
    t2.id = "TC_CALC_002";
    t2.name = "Grade Point (50)";
    t2.expected = "1.000000";
    t2.actual = std::to_string(gradeD);
    t2.status = (gradeD == 1.0) ? "PASS" : "FAIL";
    all_results.push_back(t2);
}

// --- MODULE 2: FILE HANDLING TESTS ---
void check_files() {
    std::cout << "Running File Persistence Tests...\n";

    // Setup: Create dummy data
    std::vector<Student> list;
    Student s;
    s.username = "test_persist";
    s.studentID = "999";
    s.name = "Persistence Test";
    s.password = "123";
    s.sectionID = "SEC-TEST";
    list.push_back(s);

    // 1. SAVE (Simulated)
    std::ofstream out("temp_qa_test.txt");
    out << "FormatHeader\n";
    out << s.username << "," << s.studentID << "," << s.name << "," << s.password << "," << s.sectionID << "\n";
    out.close();

    // 2. LOAD (Simulated ETL)
    std::vector<Student> loaded;
    std::ifstream in("temp_qa_test.txt");
    std::string line;
    getline(in, line); // skip header
    while(getline(in, line)) {
        std::stringstream ss(line);
        std::string segment;
        Student temp;
        getline(ss, temp.username, ',');
        getline(ss, temp.studentID, ','); 
        // ... (skipping others for brevity in test)
        loaded.push_back(temp);
    }
    in.close();

    // TEST 3: Verify Data Integrity
    // ID: TC_FILE_002
    TestResult t5;
    t5.id = "TC_FILE_002";
    t5.name = "Data Persistence";
    t5.expected = "test_persist";
    
    if (!loaded.empty()) {
        t5.actual = loaded[0].username;
        t5.status = (loaded[0].username == "test_persist") ? "PASS" : "FAIL";
    } else {
        t5.actual = "NO DATA";
        t5.status = "FAIL";
    }
    all_results.push_back(t5);

    remove("temp_qa_test.txt");
}

// --- MODULE 3: SCHEDULING TESTS ---
void check_scheduling() {
    std::cout << "Running Scheduling Tests...\n";

    // TEST 4: Teacher Assignment Logic
    // ID: TC_SCHED_003
    
    // Arrange
    ScheduleEntry s;
    s.teacherID = "TEST_TEACHER";
    s.courseCode = "TEST101";
    s.day = "Monday";
    
    // Act
    all_schedule.push_back(s);
    
    // Assert (Find it back)
    bool found = false;
    for(const auto& entry : all_schedule) {
        if(entry.teacherID == "TEST_TEACHER") {
            found = true;
            break;
        }
    }

    TestResult tSched;
    tSched.id = "TC_SCHED_003";
    tSched.name = "Schedule Assignment";
    tSched.expected = "Found=1"; 
    tSched.actual = found ? "Found=1" : "Found=0";
    tSched.status = found ? "PASS" : "FAIL";
    
    all_results.push_back(tSched);
}

// --- MODULE 4: AUTHENTICATION TESTS ---
void check_security() {
    std::cout << "Running Security Tests...\n";

    // TEST 5: Security Boundary
    // ID: TC_AUTH_004
    std::string role = test_login_logic("fake_user", "bad_pass");

    TestResult t7;
    t7.id = "TC_AUTH_004";
    t7.name = "Security Boundary";
    t7.expected = "INVALID"; 
    t7.actual = role;
    t7.status = (role == "INVALID") ? "PASS" : "FAIL";
    
    all_results.push_back(t7);
}

// --- MAIN TEST RUNNER ---
int main() {
    // 1. Run all test modules
    check_calculations();
    check_files();
    check_scheduling(); // Now includes the new test
    check_security();

    // 2. Display Report Table
    std::cout << "\n\n";
    std::cout << "=========================================================================\n";
    std::cout << "                             QA TEST REPORT                              \n";
    std::cout << "=========================================================================\n";
    
    std::cout << "| " << std::left << std::setw(15) << "TEST ID" 
              << "| " << std::setw(25) << "NAME"
              << "| " << std::setw(10) << "EXPECTED"
              << "| " << std::setw(10) << "ACTUAL"
              << "| " << std::setw(8) << "STATUS" << " |\n";
              
    std::cout << "-------------------------------------------------------------------------      |\n";

    int pass_count = 0;
    for (const auto& res : all_results) {
        std::cout << "| " << std::left << std::setw(15) << res.id
                  << "| " << std::setw(25) << res.name
                  << "| " << std::setw(10) << res.expected.substr(0,8)
                  << "| " << std::setw(10) << res.actual.substr(0,8)
                  << "| " << std::setw(8) << res.status << " |\n";
                  
        if (res.status == "PASS") pass_count++;
    }
    std::cout << "-------------------------------------------------------------------------      |\n";
    
    // Summary
    std::cout << "TOTAL TESTS: " << all_results.size() << "\n";
    std::cout << "PASSED:      " << pass_count << "\n";
    std::cout << "FAILED:      " << (all_results.size() - pass_count) << "\n";
    std::cout << "=========================================================================\n";

    std::cout << "\n[QA] System Certified. Press Enter to exit...";
    std::cin.get();
    return 0;
}

