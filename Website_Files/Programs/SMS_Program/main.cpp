
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
//   Last Modified: November 19, 2025                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

/*
============================================================================
 --- FILE: main.cpp ---
============================================================================

 Summary:
 This is the main entry point for the entire application.

 It handles the overall program lifecycle:
 1. Initialization: Loading all data (students, teachers, courses, etc.)
    from text files into memory when the program starts.
 2. Execution: Displaying the main menu and routing the user to the
    appropriate login screen (Student, Teacher, or Admin).
 3. Termination: Saving all in-memory data back to text files when the
    user chooses to exit, ensuring no work is lost.

============================================================================
*/


// --- Standard Library Includes ---
#include <iostream>     // For console input/output (std::cout, std::cin)
#include <vector>       // For using std::vector containers
#include <string>       // For using std::string class
#include <limits>       // For std::numeric_limits (used in input validation)
#include <algorithm>    // For std::find (used in populateSectionIDs)

// --- Project Header Includes ---
#include "headers/student.h"
#include "headers/teacher.h"
#include "headers/admin.h"
#include "headers/course.h"
#include "headers/schedule.h"
#include "headers/file_handler.h"
#include "headers/auth.h"
#include "headers/utility.h"        // For helper functions like getCurrentDate
#include "headers/attendance.h"     // For attendance records


// --- Global Data Store ---
// These vectors act as the application's in-memory database.
// Instead of reading from files every time we need data, we load everything
// here at the start and pass references to these vectors around.
std::vector<Student> g_allStudents;
std::vector<Teacher> g_allTeachers;
std::vector<Admin> g_allAdmins;
std::vector<Course> g_allCourses;
std::vector<ScheduleEntry> g_weeklySchedule;
std::vector<AttendanceRecord> g_attendanceRecords;
std::vector<std::string> g_allSectionIDs;
std::vector<SubmissionDefinition> g_allSubmissions;
std::vector<StudentSubmission> g_studentSubmissions;


// --- File Paths ---
// Constants defining where our data lives on the disk.
const std::string STUDENT_FILE = "data/students.txt";
const std::string TEACHER_FILE = "data/teachers.txt";
const std::string ADMIN_FILE = "data/admins.txt";
const std::string COURSE_FILE = "data/courses.txt";
const std::string SCHEDULE_FILE = "data/schedule.txt";
const std::string ATTENDANCE_FILE = "data/attendance.txt";
const std::string SUBMISSION_FILE = "data/submissions.txt";
const std::string STUDENT_MARKS_FILE = "data/student_marks.txt";


// --- Function Prototypes ---
// We declare these up here so 'main' knows they exist.
void showMainMenu();        // The main loop
void loadAllData();         // The startup loader
void saveAllData();         // The shutdown saver
void populateSectionIDs();  // Helper to find unique sections


/*
    Main entry point.
    The logic flow is simple: Load -> Run Menu -> Save -> Exit.
*/
int main() {
    // 1. Startup: Load everything from disk
    loadAllData();

    // 2. Verification: Print a summary so we know data loaded correctly
    std::cout << "--- Data Load Summary ---\n";
    std::cout << "Loaded " << g_allStudents.size() << " students.\n";
    std::cout << "Loaded " << g_allTeachers.size() << " teachers.\n";
    std::cout << "Loaded " << g_allAdmins.size() << " admins.\n";
    std::cout << "Loaded " << g_allCourses.size() << " courses.\n";
    std::cout << "Loaded " << g_weeklySchedule.size() << " schedule entries.\n";
    std::cout << "Loaded " << g_attendanceRecords.size() << " attendance records.\n";
    std::cout << "Loaded " << g_allSubmissions.size() << " submissions definitions.\n";
    std::cout << "Loaded " << g_studentSubmissions.size() << " student marks.\n";
    std::cout << "Loaded " << g_allSectionIDs.size() << " sections.\n";
    std::cout << "-------------------------\n";
    
    system("pause");

    // 3. Execution: Run the main menu loop
    // This function won't return until the user selects "Exit".
    showMainMenu();
    
    // 4. Termination: The program exited cleanly.
    return 0; 
}


/*
    Calls the file handlers to populate all our global vectors.
*/
void loadAllData() {
    g_allStudents = loadStudentsFromFile(STUDENT_FILE);
    g_allTeachers = loadTeachersFromFile(TEACHER_FILE);
    g_allAdmins = loadAdminsFromFile(ADMIN_FILE);
    g_allCourses = loadCoursesFromFile(COURSE_FILE);
    g_weeklySchedule = loadScheduleFromFile(SCHEDULE_FILE);
    g_attendanceRecords = loadAttendanceFromFile(ATTENDANCE_FILE);
    g_allSubmissions = loadSubmissionsFromFile(SUBMISSION_FILE);
    g_studentSubmissions = loadStudentSubmissionsFromFile(STUDENT_MARKS_FILE);
    
    // Once the raw data is loaded, we need to figure out what Sections exist
    populateSectionIDs();
}

/*
    Writes the current state of all global vectors back to the text files.
    This overwrites the old files with the new data.
*/
void saveAllData() {
    saveStudentsToFile(STUDENT_FILE, g_allStudents);
    saveTeachersToFile(TEACHER_FILE, g_allTeachers);
    saveScheduleToFile(SCHEDULE_FILE, g_weeklySchedule);
    saveCoursesToFile(COURSE_FILE, g_allCourses);
    saveAdminsToFile(ADMIN_FILE, g_allAdmins);
    saveAttendanceToFile(ATTENDANCE_FILE, g_attendanceRecords);
    saveSubmissionsToFile(SUBMISSION_FILE, g_allSubmissions);
    saveStudentSubmissionsToFile(STUDENT_MARKS_FILE, g_studentSubmissions);
}

/*
    This helper looks through all students and all schedule entries to find
    every unique Section ID (like "BCS-2A", "BCS-6B").
    We need this list so the Admin has a dropdown menu of sections to pick from.
*/
void populateSectionIDs() {
     g_allSectionIDs.clear(); 

     // Scan students for section IDs
     for(const auto& s : g_allStudents) {
         bool isValid = !s.sectionID.empty() && s.sectionID != "Unassigned";
         
         // Check if we've already added this ID to our list
         bool isDuplicate = std::find(g_allSectionIDs.begin(), g_allSectionIDs.end(), s.sectionID) != g_allSectionIDs.end();

         if (isValid && !isDuplicate) 
         {
             g_allSectionIDs.push_back(s.sectionID);
         }
     }
     
     // Scan the schedule too (in case a section has classes but no students yet)
     for(const auto& entry : g_weeklySchedule) {
         bool isValid = !entry.sectionID.empty() && entry.sectionID != "Unassigned";

         bool isDuplicate = std::find(g_allSectionIDs.begin(), g_allSectionIDs.end(), entry.sectionID) != g_allSectionIDs.end();

         if (isValid && !isDuplicate) 
         {
             g_allSectionIDs.push_back(entry.sectionID);
         }
     }
}

/*
    The top-level menu loop.
    This is where the user decides their role (Student, Teacher, Admin).
*/
void showMainMenu() {
    int choice;

    do {
        system("cls");
        std::cout << "COMSATS University Islamabad, Wah Campus\n";
        std::cout << "Date: " << getCurrentDate() << "\n";
        std::cout << "========================================\n";
        std::cout << "    Student & Teacher Academic Portal\n";
        std::cout << "========================================\n\n";
        std::cout << "1. Login as Student\n";
        std::cout << "2. Login as Teacher\n";
        std::cout << "3. Login as Admin\n";
        std::cout << "4. Exit\n\n";
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear(); 
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
            
            system("pause");
            choice = 0; 
            continue;   
        }

        switch (choice) {
            case 1:
                // Pass the massive data lists to the Student Login handler
                loginAsStudent(g_allStudents, g_weeklySchedule, g_allCourses, g_allTeachers, g_attendanceRecords,
                               g_allSubmissions, g_studentSubmissions);
                break;
            case 2:
                // Pass the data lists to the Teacher Login handler
                loginAsTeacher(g_allTeachers, g_allStudents, g_weeklySchedule, g_allCourses, g_attendanceRecords,
                               g_allSubmissions, g_studentSubmissions);
                break;
            case 3:
                // Pass the data lists to the Admin Login handler
                loginAsAdmin(g_allAdmins, g_allStudents, g_allTeachers, g_allCourses, g_weeklySchedule, g_attendanceRecords,
                             g_allSubmissions, g_studentSubmissions);
                break;
            case 4:
                // CRITICAL: Save everything before quitting!
                std::cout << "Saving all data...\n";
                saveAllData();
                std::cout << "Exiting. Goodbye!\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                system("pause");
                break;
        }
    } while (choice != 4);
}
