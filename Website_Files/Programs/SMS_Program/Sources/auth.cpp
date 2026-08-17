
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
 --- FILE: auth.cpp ---
============================================================================

 Summary:
 This file contains the implementation for user authentication (login)
 for all three user types: Student, Teacher, and Admin.

 It performs the following steps:
 1. Prompts the user for a username and password.
 2. Uses a helper function (`findByUsername`) to locate the user in the
    appropriate global list (students, teachers, or admins).
 3. Verifies the password.
 4. If successful, redirects the program flow to the corresponding user
    dashboard function (`showStudentDashboard`, etc.), passing necessary
    global data lists.

============================================================================
*/

#include "../headers/auth.h"            // Authentication function prototypes
// Auth.cpp needs the full definitions to call the dashboard functions
#include "../headers/student.h"         // Student structure and dashboard prototype
#include "../headers/teacher.h"         // Teacher structure and dashboard prototype
#include "../headers/admin.h"           // Admin structure and dashboard prototype
#include "../headers/utility.h"         // For utility functions like getCurrentDate
#include "../headers/attendance.h"      // <-- ADD INCLUDE (AttendanceRecord structure)
#include "../headers/schedule.h"        // ScheduleEntry structure (for dashboard calls)
#include "../headers/course.h"          // Course structure (for dashboard calls)


#include <iostream>                     // For I/O operations
#include <vector>                       // For std::vector
#include <string>                       // For std::string

// --- Helper function to find by Username (Templated) ---

/*
    This is a generic search function. It looks through a list (vector) of users
    (whether they are Students, Teachers, or Admins) and tries to find one
    that matches the given username.
    
    If it finds a match, it returns the index (position in the list).
    If not, it returns -1.
*/
// Base template (used for Student by default)
template<typename T>
int findByUsername(const std::vector<T>& vec, const std::string& username) {
    for (size_t i = 0; i < vec.size(); ++i) {
        // This checks the 'username' property of the object
        if (vec[i].username == username) return static_cast<int>(i);
    }
    return -1; // Not found
}

// Special version for finding Teachers
template<>
int findByUsername(const std::vector<Teacher>& vec, const std::string& username) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i].username == username) return static_cast<int>(i);
    }
    return -1;
}

// Special version for finding Admins
template<>
int findByUsername(const std::vector<Admin>& vec, const std::string& username) {
    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i].username == username) return static_cast<int>(i);
    }
    return -1;
}
// --- End findByUsername ---


/*
    Clears the console screen and prints the fancy header.
    We use this at the start of every login screen.
*/
void displayMainHeader(const std::string& title) {
    system("cls"); // Clear the console screen
    std::cout << "COMSATS University Islamabad, Wah Campus\n";
    std::cout << "Date: " << getCurrentDate() << "\n";
    std::cout << "========================================\n";
    std::cout << "    " << title << "\n";
    std::cout << "========================================\n\n";
}


// --- Login Function Implementations ---

/*
    Handles the Student Login flow.
    1. Asks for credentials.
    2. Checks the 'allStudents' list.
    3. If correct, launches 'showStudentDashboard'.
*/
void loginAsStudent(std::vector<Student>& allStudents, const std::vector<ScheduleEntry>& allSchedule,
                    const std::vector<Course>& allCourses, const std::vector<Teacher>& allTeachers,
                    const std::vector<AttendanceRecord>& allAttendance,
                    const std::vector<SubmissionDefinition>& allSubmissions,
                    const std::vector<StudentSubmission>& studentSubmissions) { 
    
    displayMainHeader("Student Login");
    
    std::string username, pass;
    std::cout << "Enter Username (e.g., FA25-BCS-109): ";
    std::cin >> username;
    std::cout << "Enter Password: ";
    std::cin >> pass;

    // Search for the student
    int studentIndex = findByUsername(allStudents, username);

    // Verify credentials
    if (studentIndex != -1 && allStudents[studentIndex].password == pass) {
        // Success! Go to dashboard.
        showStudentDashboard(allStudents[studentIndex], allSchedule, allCourses, allTeachers, allAttendance,
                             allSubmissions, studentSubmissions); 
    } else {
        std::cout << "\nError: Invalid Username or Password.\n";
        system("pause");
    }
}

/*
    Handles the Teacher Login flow.
    Similar to student login, but checks the 'allTeachers' list
    and launches 'showTeacherDashboard'.
*/
void loginAsTeacher(std::vector<Teacher>& allTeachers, std::vector<Student>& allStudents,
                    const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                    std::vector<AttendanceRecord>& allAttendance,
                    std::vector<SubmissionDefinition>& allSubmissions,
                    std::vector<StudentSubmission>& studentSubmissions) {
    
    displayMainHeader("Teacher Login");

    std::string username, pass;
    std::cout << "Enter Username (e.g., SamiaRiaz): "; 
    std::cin >> username;
    std::cout << "Enter Password: ";
    std::cin >> pass;

    // Search for the teacher
    int teacherIndex = findByUsername(allTeachers, username);

    if (teacherIndex != -1 && allTeachers[teacherIndex].password == pass) {
        // Success! Go to dashboard.
        showTeacherDashboard(allTeachers[teacherIndex], allStudents, allSchedule, allCourses, allTeachers, allAttendance,
                             allSubmissions, studentSubmissions);
    } else {
        std::cout << "\nError: Invalid Username or Password.\n";
        system("pause");
    }
}

/*
    Handles the Admin Login flow.
    Checks the 'allAdmins' list and launches 'showAdminDashboard'.
    Admins need access to almost every piece of data in the system to manage it.
*/
void loginAsAdmin(std::vector<Admin>& allAdmins, std::vector<Student>& allStudents,
                  std::vector<Teacher>& allTeachers, std::vector<Course>& allCourses,
                  std::vector<ScheduleEntry>& allSchedule,
                  std::vector<AttendanceRecord>& allAttendance,
                  std::vector<SubmissionDefinition>& allSubmissions,
                  std::vector<StudentSubmission>& studentSubmissions) {
    
    displayMainHeader("Admin Login");

    std::string username, pass;
    std::cout << "Enter Username: ";
    std::cin >> username;
    std::cout << "Enter Password: ";
    std::cin >> pass;

    // Search for the admin
    int adminIndex = findByUsername(allAdmins, username);
    
    if (adminIndex != -1 && allAdmins[adminIndex].password == pass) {
        // Success! Go to dashboard.
         showAdminDashboard(allAdmins[adminIndex], allStudents, allTeachers, allCourses, allSchedule, allAttendance,
                            allSubmissions, studentSubmissions); 
    } else {
        std::cout << "\nError: Invalid Username or Password.\n";
        system("pause");
    }
}
