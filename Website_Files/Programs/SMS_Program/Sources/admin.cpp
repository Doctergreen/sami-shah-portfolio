
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
//   Fareed Ul Hussain (145)                                                //
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
 --- FILE: admin.cpp ---
============================================================================

 Summary:
 This file contains all the logic for the Admin's side of the application.

 It handles:
 1. The main Admin Dashboard and menu navigation.
 2. Functions to Add, Remove, and Edit Students, Teachers, and Courses.
 3. Managing Sections and viewing/editing Schedules.
 4. Viewing detailed student records (attendance, grades, and submissions).

============================================================================
*/

// --- Project Header Includes ---
#include "../headers/admin.h"           // Admin-specific structures and management functions
#include "../headers/schedule.h"        // Schedule structures and display/manipulation functions
#include "../headers/student.h"         // Student-specific structures and helper functions
#include "../headers/teacher.h"         // Teacher-specific structures
#include "../headers/course.h"          // Course structures
#include "../headers/utility.h"         // For changePassword and getCurrentDate helper functions
#include "../headers/attendance.h"      // Attendance record structures

// --- Standard Library Includes ---
#include <iostream>                     // For console input/output (std::cout, std::cin)
#include <iomanip>                      // For output formatting (std::setw, std::setprecision)
#include <limits>                       // For std::numeric_limits (used in input validation)
#include <vector>                       // For using std::vector containers
#include <string>                       // For using std::string class
#include <algorithm>                    // For std::find, std::remove_if, std::sort
#include <map>                          // For std::map (used in attendance grouping)
#include <numeric>                      // For std::accumulate (not explicitly used here, but kept if needed)

// --- Global Variable Declaration ---
// This vector holds all the unique section IDs used across the app.
// We declare it extern here because it's actually defined in main.cpp.
extern std::vector<std::string> g_allSectionIDs;

/*
    This is the main dashboard for the Admin. It runs a loop that shows the menu
    and lets the admin pick what they want to manage (students, teachers, courses, etc.)
    until they decide to logout.
    
    It takes references to all the main data vectors so it can pass them down
    to the specific management functions.
*/
void showAdminDashboard(Admin& loggedInAdmin, std::vector<Student>& allStudents, std::vector<Teacher>& allTeachers,
                        std::vector<Course>& allCourses, std::vector<ScheduleEntry>& allSchedule,
                        const std::vector<AttendanceRecord>& allAttendance,
                        std::vector<SubmissionDefinition>& allSubmissions,
                        std::vector<StudentSubmission>& studentSubmissions) { 

    // First, we need to rebuild the global list of Section IDs.
    // We look at every student and schedule entry to find unique section names
    // and add them to our list if they aren't there already.
    g_allSectionIDs.clear();
    for(const auto& s : allStudents) {
        if (!s.sectionID.empty() && s.sectionID != "Unassigned" &&
            std::find(g_allSectionIDs.begin(), g_allSectionIDs.end(), s.sectionID) == g_allSectionIDs.end()) {
            g_allSectionIDs.push_back(s.sectionID);
        }
    }
    for(const auto& entry : allSchedule) {
        if (!entry.sectionID.empty() && entry.sectionID != "Unassigned" &&
            std::find(g_allSectionIDs.begin(), g_allSectionIDs.end(), entry.sectionID) == g_allSectionIDs.end()) {
            g_allSectionIDs.push_back(entry.sectionID);
        }
    }

    int choice;
    do {
        system("cls"); // Clear screen for a clean menu display
        
        // Display the header with the current date and admin info
        std::cout << "COMSATS University Islamabad, Wah Campus\n";
        std::cout << "Date: " << getCurrentDate() << "\n";
        std::cout << "------------------------------------------------------------------\n";
        std::cout << "Admin: " << loggedInAdmin.name << " | Position: " << loggedInAdmin.position << "\n";
        std::cout << "==================================================================\n\n";

        // Main Admin Menu Options
        std::cout << "1. Manage Students\n";
        std::cout << "2. Manage Teachers\n";
        std::cout << "3. Manage Courses\n";
        std::cout << "4. Manage Sections\n";
        std::cout << "5. View/Edit Schedules\n";
        std::cout << "6. Change My Password\n"; 
        std::cout << "9. Logout\n\n";
        std::cout << "Enter your choice: ";

        // Handle input and basic validation
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            system("pause");
            choice = 0;
            continue;
        }

        // Route the user to the correct function based on their choice
        switch (choice) {
            case 1:
                // Go to Student Management (passing all the data needed including submissions)
                admin_ManageStudents(allStudents, allCourses, g_allSectionIDs, allAttendance,
                                     allSubmissions, studentSubmissions);
                break;
            case 2:
                // Go to Teacher Management
                admin_ManageTeachers(allTeachers, allCourses);
                break;
            case 3:
                // Go to Course Management
                admin_ManageCourses(allCourses);
                break;
            case 4:
                // Go to Section Management
                admin_ManageSections(g_allSectionIDs, allStudents, allSchedule, allCourses, allTeachers);
                break;
            case 5:
                // Go to Schedule Management
                showAdminScheduleMenu(allSchedule, allCourses, allTeachers, g_allSectionIDs);
                break;
            case 6: 
                // Helper function to change the admin's own password
                changePassword(loggedInAdmin);
                break;
            case 9: 
                std::cout << "Logging out...\n";
                break;
            default:
                std::cout << "Invalid choice.\n";
                system("pause");
                break;
        } 
    
    } while (choice != 9); // Keep looping until they pick Logout
}

/*
    Handles everything related to students. 
    This menu lets the admin add new students, remove them, view their details,
    enroll them in courses, or change their sections.
*/
void admin_ManageStudents(std::vector<Student>& allStudents, const std::vector<Course>& allCourses, 
                          const std::vector<std::string>& allSectionIDs, const std::vector<AttendanceRecord>& allAttendance,
                          std::vector<SubmissionDefinition>& allSubmissions,
                          std::vector<StudentSubmission>& studentSubmissions) {
    int choice;
    do {
        system("cls");
        std::cout << "--- Student Management ---\n";
        std::cout << "1. Add New Student (& Assign Section)\n";
        std::cout << "2. Remove Student\n";
        std::cout << "3. View All Student Details...\n"; 
        std::cout << "4. Enroll Student in Course\n";
        std::cout << "5. Assign/Change Student Section\n";
        std::cout << "6. Transfer Student to Another Section\n"; // Same as option 5
        std::cout << "9. Back to Admin Dashboard\n\n";
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
                // Add a student. We need courses to set up their default grades and sections to assign them one.
                admin_AddStudent(allStudents, allCourses, allSectionIDs);
                break;
            case 2:
                admin_RemoveStudent(allStudents);
                break;
            case 3: 
                // Go to the sub-menu for viewing student details
                admin_ViewStudentDetailsMenu(allStudents, allCourses, allAttendance, 
                                             allSubmissions, studentSubmissions);
                break;
            case 4:
            {    // First find the student by ID, then enroll them
                std::cout << "Enter Student ID (Roll No) to enroll: ";
                std::string id;
                std::cin >> id;
                Student* studentToEnroll = nullptr;
                for(auto& s : allStudents) { if (s.studentID == id) { studentToEnroll = &s; break; } }
                if (studentToEnroll) { admin_EnrollStudentInCourse(*studentToEnroll, allCourses); }
                else { std::cout << "Student not found.\n"; system("pause"); }
            }
                break;
            case 5:
                admin_AssignStudentToSection(allStudents, allSectionIDs);
                break;
            case 6:
                // Just calls the same function as case 5
                admin_TransferStudentSection(allStudents, allSectionIDs);
                break;
            case 9:
                break; // Go back to dashboard
            default:
                std::cout << "Invalid choice.\n"; system("pause"); break;
        }
    } while (choice != 9);
}

/*
    Creates a new student record.
    We ask for their username, roll number, name, and password.
    We also check if the username is taken, assign them to a section, 
    and enroll them in all current courses by default (mark starts at 0).
*/
void admin_AddStudent(std::vector<Student>& allStudents, const std::vector<Course>& allCourses, const std::vector<std::string>& allSectionIDs) {
    system("cls");
    std::cout << "--- Add New Student ---\n";
    Student newStudent;

    std::cout << "Enter Student Username (e.g., FA25-BCS-109): ";
    std::cin >> newStudent.username;
    
    // Make sure username doesn't exist already
    for(const auto& s : allStudents) { if(s.username == newStudent.username) { std::cout << "Username exists!\n"; system("pause"); return; }}

    std::cout << "Enter Student Roll Number (e.g., 109): ";
    std::cin >> newStudent.studentID;
    
    std::cout << "Enter Student Name (use_underscores): ";
    std::cin >> newStudent.name;
    std::cout << "Enter Student Password: ";
    std::cin >> newStudent.password;

    // Logic to handle section assignment
    if (allSectionIDs.empty()) {
        std::cout << "No sections exist. Assigning as 'Unassigned'.\n";
        newStudent.sectionID = "Unassigned";
    } else {
        std::cout << "\nAvailable Sections:\n";
        for (size_t i = 0; i < allSectionIDs.size(); ++i) {
            std::cout << i + 1 << ". " << allSectionIDs[i] << "\n";
        }
        std::cout << "Assign student to section (enter number, 0 for Unassigned): ";
        int sectionChoice;
        if (!(std::cin >> sectionChoice)) {
            std::cout << "Invalid input. Assigning as 'Unassigned'.\n";
            std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            newStudent.sectionID = "Unassigned";
        } else if (sectionChoice > 0 && sectionChoice <= static_cast<int>(allSectionIDs.size())) {
            newStudent.sectionID = allSectionIDs[sectionChoice - 1];
        } else {
            std::cout << "Assigning as 'Unassigned'.\n";
            newStudent.sectionID = "Unassigned";
        }
    }

    // Automatically enroll them in every course available
    std::cout << "\nStudent added. Enrolling in default courses...\n";
    for (const auto& course : allCourses) {
        newStudent.grades.push_back(Grade(course.courseCode, 0)); // Initial mark is 0
    }
    std::cout << "Enrolled in " << newStudent.grades.size() << " courses.\n";

    // Finally, add them to the main list
    allStudents.push_back(newStudent);
    std::cout << "\nStudent '" << newStudent.name << "' added successfully!\n";
    system("pause");
}

/*
    Just a helper that calls assignStudentToSection.
    Matches the "Transfer" option in the menu.
*/
void admin_TransferStudentSection(std::vector<Student>& allStudents, const std::vector<std::string>& allSectionIDs) {
    admin_AssignStudentToSection(allStudents, allSectionIDs);
}

/*
    Finds a student by their Roll Number and deletes them from the list.
*/
void admin_RemoveStudent(std::vector<Student>& allStudents) {
    system("cls");
    std::cout << "--- Remove Student ---\n";
    std::string id;
    std::cout << "Enter Student ID (Roll No) to remove: ";
    std::cin >> id;

    int studentIndex = -1;
    // Find the student's index
    for (size_t i = 0; i < allStudents.size(); ++i) {
        if (allStudents[i].studentID == id) {
            studentIndex = i;
            break;
        }
    }

    if (studentIndex == -1) {
        std::cout << "Error: Student not found.\n";
    } else {
        std::cout << "Removing student: " << allStudents[studentIndex].name << "\n";
        // Actually remove them from the vector
        allStudents.erase(allStudents.begin() + studentIndex);
        std::cout << "Student removed successfully.\n";
    }
    system("pause");
}

/*
    Shows a simple table of all students.
    I sort them by Roll Number first so the list is easier to read.
*/
void admin_ViewAllStudents(const std::vector<Student>& allStudents) {
    system("cls");
    std::cout << "--- All Student Records (Sorted by Roll No.) ---\n\n";

    // --- SORTING LOGIC ---
    // We make a copy because we can't modify the original 'const' list.
    std::vector<Student> sortedStudents = allStudents;

    // Sort using a lambda function to compare Roll IDs as integers
    std::sort(sortedStudents.begin(), sortedStudents.end(),
        [](const Student& a, const Student& b) {
            try {
                int idA = std::stoi(a.studentID);
                int idB = std::stoi(b.studentID);
                return idA < idB; 
            } catch (const std::invalid_argument& e) {
                // If ID isn't a number, just keep it at the end
                return false; 
            } catch (const std::out_of_range& e) {
                   return false;
            }
        });


    // Print the table header
    std::cout << std::left
              << std::setw(20) << "Username"
              << std::setw(15) << "Roll No."
              << std::setw(30) << "Name"
              << std::setw(15) << "Password"
              << std::setw(12) << "Section" << "\n";
    std::cout << std::string(92, '-') << "\n";

    // Loop through the sorted list and print rows
    if (sortedStudents.empty()) { 
        std::cout << "No student records found.\n";
       } else {
        for (const auto& student : sortedStudents) { 
            std::cout << std::left
                      << std::setw(20) << student.username
                      << std::setw(15) << student.studentID
                      << std::setw(30) << student.name
                      << std::setw(15) << student.password
                      << std::setw(12) << student.sectionID << "\n";
        }
    }
    std::cout << "\n";
    system("pause");
}

/*
    Enrolls a specific student in a new course.
    Checks if they are already enrolled first. If not, adds the course with 0 marks.
*/
void admin_EnrollStudentInCourse(Student& student, const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Enrolling " << student.name << " in a Course ---\n";
    if (allCourses.empty()) { std::cout << "No courses available.\n"; system("pause"); return; }
    
    // Show list of courses
    for(size_t i = 0; i < allCourses.size(); ++i) { std::cout << i+1 << ". " << allCourses[i].courseCode << " - " << allCourses[i].courseName << "\n"; }

    std::cout << "\nEnter course number to enroll in (0 to cancel): ";
    int choice;
    
    if (!(std::cin >> choice)) { std::cout << "Invalid input.\n"; std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); system("pause"); return; }

    if(choice > 0 && choice <= static_cast<int>(allCourses.size())) {
        std::string selectedCode = allCourses[choice - 1].courseCode;
        bool alreadyEnrolled = false;
        
        // Check existing enrollment
        for(const auto& grade : student.grades) { if (grade.courseCode == selectedCode) { alreadyEnrolled = true; break; } }
        
        if (alreadyEnrolled) { std::cout << "Student is already enrolled.\n"; }
        else { 
            // Add the course to their grades vector
            student.grades.push_back(Grade(selectedCode, 0)); 
            std::cout << "Student enrolled (Mark: 0).\n"; 
        }
    } else if (choice != 0) { std::cout << "Invalid choice.\n"; }
    else { std::cout << "Operation cancelled.\n"; }
    system("pause");
}

/*
    Changes a student's section ID.
    We ask for the student's Roll No, then show a list of sections to pick from.
*/
void admin_AssignStudentToSection(std::vector<Student>& allStudents, const std::vector<std::string>& allSectionIDs) {
    system("cls");
    std::cout << "--- Assign/Change Student Section ---\n";
    if (allSectionIDs.empty()) { std::cout << "No sections exist.\n"; system("pause"); return; }
    std::string studentID;
    std::cout << "Enter Student ID: ";
    std::cin >> studentID;
    
    // Locate the student
    Student* student = nullptr;
    for (auto& s : allStudents) { if (s.studentID == studentID) { student = &s; break; } }
    
    if (!student) { std::cout << "Student not found.\n"; system("pause"); return; }
    
    std::cout << "Current Section: " << student->sectionID << "\nAvailable Sections:\n";
    for (size_t i = 0; i < allSectionIDs.size(); ++i) { std::cout << i + 1 << ". " << allSectionIDs[i] << "\n"; }
    std::cout << "Assign to section number (0 to cancel): ";
    int sectionChoice;
    
    if(!(std::cin >> sectionChoice)){ std::cout << "Invalid input.\n"; std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); system("pause"); return;}
    
    if (sectionChoice > 0 && sectionChoice <= static_cast<int>(allSectionIDs.size())) {
        student->sectionID = allSectionIDs[sectionChoice - 1]; // Save the new section
        std::cout << "Student assigned to section '" << student->sectionID << "'.\n";
    } else if (sectionChoice != 0) { std::cout << "Invalid choice.\n"; }
    else { std::cout << "Operation cancelled.\n"; }
    system("pause");
}

// Forward declarations for schedule functions (implemented elsewhere)
void displayScheduleGrid(const std::vector<ScheduleEntry>& schedule, const std::vector<Course>& courses, 
                          const std::vector<Teacher>& teachers, const std::string& title);
void admin_AddScheduleEntry(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                            const std::vector<Teacher>& allTeachers, const std::vector<std::string>& allSectionIDs);
void admin_RemoveScheduleEntry(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses);
void admin_SwapScheduleEntries(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                               const std::vector<std::string>& allSectionIDs);

// Forward declaration to keep the code compiling in order
void admin_ViewSectionSchedule(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                               const std::vector<Teacher>& allTeachers, const std::vector<std::string>& allSectionIDs);

/*
    Shows the Schedule Management menu.
    Admin can choose to look at schedules either by Teacher (e.g., what classes does Dr. Smith have?)
    or by Section (e.g., what is BCS-1A's timetable?).
*/
void showAdminScheduleMenu(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                           const std::vector<Teacher>& allTeachers, const std::vector<std::string>& allSectionIDs) {
    int choice;
    do {
        system("cls");
        std::cout << "--- View/Edit Schedules (Admin) ---\n";
        std::cout << "1. View/Edit Teacher Schedule\n";
        std::cout << "2. View/Edit Section Schedule\n";
        std::cout << "9. Back to Admin Dashboard\n\n";
        std::cout << "Enter your choice: ";
        
        if (!(std::cin >> choice)) {
             std::cout << "Invalid input.\n"; std::cin.clear();
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             system("pause"); choice = 0; continue;
        }
        switch(choice) {
            case 1: 
                admin_ViewTeacherSchedule(allSchedule, allCourses, allTeachers); 
                break;
            case 2: 
                admin_ViewSectionSchedule(allSchedule, allCourses, allTeachers, allSectionIDs); 
                break;
            case 9: 
                break;
            default: 
                std::cout << "Invalid choice.\n"; system("pause"); break;
        }
    } while (choice != 9);
}

/*
    Lets the admin pick a teacher, shows their schedule, and offers options to edit it
    (add class, remove class, swap timings).
*/
void admin_ViewTeacherSchedule(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                               const std::vector<Teacher>& allTeachers) {
    system("cls");
    std::cout << "--- View/Edit Teacher Schedule ---\n\n";
    if (allTeachers.empty()) { std::cout << "No teachers in the system.\n"; system("pause"); return; }
    
    // 1. Pick the teacher
    std::cout << "Select Teacher:\n";
    for(size_t i = 0; i < allTeachers.size(); ++i) {
        std::cout << i + 1 << ". " << allTeachers[i].name << " (" << allTeachers[i].username << ")\n";
    }
    std::cout << "Choice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice)) {
          std::cout << "Invalid input.\n"; std::cin.clear();
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          system("pause"); return;
    }
    if (choice == 0) { std::cout << "Cancelled.\n"; system("pause"); return; }
    if (choice < 1 || choice > static_cast<int>(allTeachers.size())) {
        std::cout << "Invalid choice.\n"; system("pause"); return;
    }
    const Teacher& selectedTeacher = allTeachers[choice - 1];
    
    std::vector<ScheduleEntry> filteredSchedule;
    int editChoice;
    
    // 2. Loop to view and edit
    do {
        // Filter the main schedule list for just this teacher
        filteredSchedule.clear();
        for(const auto& entry : allSchedule) {
            if (entry.teacherID == selectedTeacher.username) {
                filteredSchedule.push_back(entry);
            }
        }
        
        // Draw the grid
        std::string title = "Schedule for " + selectedTeacher.name;
        displayScheduleGrid(filteredSchedule, allCourses, allTeachers, title);
        
        // Show edit commands
        std::cout << "\n--- Options ---\n";
        std::cout << "1. Add Entry (To Master Schedule)\n";
        std::cout << "2. Remove Entry (From Master Schedule)\n";
        std::cout << "3. Swap Entries (Specify Section)\n";
        std::cout << "9. Back to Previous Menu\n\n";
        std::cout << "Enter choice: ";
        
        if (!(std::cin >> editChoice)) {
              std::cout << "Invalid input.\n"; std::cin.clear();
              std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
              system("pause"); editChoice = 0; continue;
        }
        
        switch(editChoice) {
            case 1: admin_AddScheduleEntry(allSchedule, allCourses, allTeachers, g_allSectionIDs); break;
            case 2: admin_RemoveScheduleEntry(allSchedule, allCourses); break;
            case 3: admin_SwapScheduleEntries(allSchedule, allCourses, g_allSectionIDs); break;
            case 9: std::cout << "Returning...\n"; break;
            default: std::cout << "Invalid choice.\n"; system("pause"); break;
        }
    } while (editChoice != 9);
}

/*
    Same as above, but lets the admin pick a Section to view/edit.
*/
void admin_ViewSectionSchedule(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                               const std::vector<Teacher>& allTeachers, const std::vector<std::string>& allSectionIDs) {
    system("cls");
    std::cout << "--- View/Edit Section Schedule ---\n\n";
    if (allSectionIDs.empty()) { std::cout << "No sections defined.\n"; system("pause"); return; }
    
    // 1. Pick the section
    std::cout << "Select Section:\n";
    for (size_t i = 0; i < allSectionIDs.size(); ++i) { std::cout << i + 1 << ". " << allSectionIDs[i] << "\n"; }
    std::cout << "Choice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice)) {
          std::cout << "Invalid input.\n"; std::cin.clear();
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          system("pause"); return;
    }
    if (choice == 0) { std::cout << "Cancelled.\n"; system("pause"); return; }
    if (choice < 1 || choice > static_cast<int>(allSectionIDs.size())) { std::cout << "Invalid choice.\n"; system("pause"); return; }
    std::string selectedSection = allSectionIDs[choice - 1];
    
    std::vector<ScheduleEntry> filteredSchedule;
    int editChoice;
    
    // 2. Loop to view and edit
    do {
        // Filter schedule for this section
        filteredSchedule.clear();
        for(const auto& entry : allSchedule) { 
            if (entry.sectionID == selectedSection) { filteredSchedule.push_back(entry); } 
        }
        
        std::string title = "Schedule for Section " + selectedSection;
        displayScheduleGrid(filteredSchedule, allCourses, allTeachers, title);
        
        std::cout << "\n--- Options ---\n";
        std::cout << "1. Add Entry (To Master Schedule)\n";
        std::cout << "2. Remove Entry (From Master Schedule)\n";
        std::cout << "3. Swap Entries (Within THIS Section)\n";
        std::cout << "9. Back to Previous Menu\n\n";
        std::cout << "Enter choice: ";
        
        if (!(std::cin >> editChoice)) {
              std::cout << "Invalid input.\n"; std::cin.clear();
              std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
              system("pause"); editChoice = 0; continue;
        }
        
        switch(editChoice) {
            case 1: admin_AddScheduleEntry(allSchedule, allCourses, allTeachers, allSectionIDs); break;
            case 2: admin_RemoveScheduleEntry(allSchedule, allCourses); break;
            case 3: admin_SwapScheduleEntries(allSchedule, allCourses, allSectionIDs); break;
            case 9: std::cout << "Returning...\n"; break;
            default: std::cout << "Invalid choice.\n"; system("pause"); break;
        }
    } while (editChoice != 9);
}

// --- Course Management Functions ---

/*
    Menu for managing courses. 
    Add new courses (like CSC101), remove them, or view the list.
*/
void admin_ManageCourses(std::vector<Course>& allCourses) {
   int choice;
    do {
        system("cls");
        std::cout << "--- Course Management ---\n";
        std::cout << "1. View All Courses\n";
        std::cout << "2. Add New Course\n";
        std::cout << "3. Remove Course\n";
        std::cout << "9. Back to Admin Dashboard\n\n";
        std::cout << "Enter your choice: ";
        if (!(std::cin >> choice)) {
              std::cout << "Invalid input.\n"; std::cin.clear();
              std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
              system("pause"); choice = 0; continue;
        }
        switch (choice) {
            case 1: admin_ViewCourses(allCourses); break;
            case 2: admin_AddCourse(allCourses); break;
            case 3: admin_RemoveCourse(allCourses); break;
            case 9: break;
            default: std::cout << "Invalid choice.\n"; system("pause"); break;
        }
    } while (choice != 9);
}

/*
    Simply prints a list of all courses (Code and Name).
*/
void admin_ViewCourses(const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- All Courses ---\n\n";
    std::cout << std::left << std::setw(15) << "Course Code" << std::setw(60) << "Course Name" << "\n";
    std::cout << std::string(75, '-') << "\n";
    if (allCourses.empty()) { std::cout << "No courses found.\n"; }
    else { 
        for (const auto& course : allCourses) { 
            std::cout << std::left << std::setw(15) << course.courseCode << std::setw(60) << course.courseName << "\n"; 
        } 
    }
    std::cout << "\n";
    system("pause");
}

/*
    Adds a new course.
    We make sure the course code doesn't already exist to prevent duplicates.
*/
void admin_AddCourse(std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Add New Course ---\n\n";
    Course newCourse;
    std::cout << "Enter New Course Code (e.g., CSC106): ";
    std::cin >> newCourse.courseCode;
    
    // Check duplicate
    for(const auto& course : allCourses) { if (course.courseCode == newCourse.courseCode) { std::cout << "Error: Course code exists.\n"; system("pause"); return; } }
    
    std::cout << "Enter New Course Name (use_underscores): ";
    std::cin >> newCourse.courseName;
    
    allCourses.push_back(newCourse);
    std::cout << "\nCourse added successfully!\n";
    std::cout << "Remember to save data by exiting.\n";
    system("pause");
}

/*
    Deletes a course by its code.
    Note: This just deletes the course definition. It doesn't automatically delete 
    grades or schedule entries that use this course, so be careful.
*/
void admin_RemoveCourse(std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Remove Course ---\n\n";
    std::string codeToRemove;
    std::cout << "Enter Course Code to remove: ";
    std::cin >> codeToRemove;
    
    int courseIndex = -1;
    for (size_t i = 0; i < allCourses.size(); ++i) { if (allCourses[i].courseCode == codeToRemove) { courseIndex = i; break; } }
    
    if (courseIndex == -1) { 
        std::cout << "Error: Course not found.\n"; 
    }
    else {
        std::cout << "Removing course: " << allCourses[courseIndex].courseName << "\n";
        allCourses.erase(allCourses.begin() + courseIndex);
        std::cout << "Course removed.\n";
        std::cout << "Note: Data referencing this code might be affected (grades, schedule).\n";
        std::cout << "Remember to save data.\n";
    }
    system("pause");
}

// --- Section Management Functions ---

/*
    Menu for managing Sections (classes).
    We can view who is in a section, create new sections, or delete them.
*/
void admin_ManageSections(std::vector<std::string>& allSectionIDs, std::vector<Student>& allStudents,
                          std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                          const std::vector<Teacher>& allTeachers) {
    int choice;
    do {
        system("cls");
        std::cout << "--- Section Management ---\n";
        std::cout << "1. View All Sections\n";
        std::cout << "2. Add New Section\n";
        std::cout << "3. Remove Section\n";
        std::cout << "9. Back to Admin Dashboard\n\n";
        std::cout << "Enter your choice: ";
        
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input.\n"; std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            system("pause"); choice = 0; continue;
        }
        switch (choice) {
            case 1:
                // View sections. Needs all data to show the details.
                admin_ViewSections(allSectionIDs, allStudents, allSchedule, allCourses, allTeachers);
                break;
            case 2:
                admin_AddSection(allSectionIDs);
                break;
            case 3: 
                admin_RemoveSection(allSectionIDs, allStudents, allSchedule); 
                break;
            case 9: break;
            default: std::cout << "Invalid choice.\n"; system("pause"); break;
        }
    } while (choice != 9);
}

/*
    Lists all sections.
    Allows the user to pick one to see detailed info (students and teachers assigned).
*/
void admin_ViewSections(const std::vector<std::string>& allSectionIDs, const std::vector<Student>& allStudents,
                        const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                        const std::vector<Teacher>& allTeachers) { 
    int choice;
    do {
        system("cls");
        std::cout << "--- All Sections ---\n\n";
        if (allSectionIDs.empty()) {
            std::cout << "No sections defined.\n";
        } else {
            for (size_t i = 0; i < allSectionIDs.size(); ++i) {
                 std::cout << i + 1 << ". " << allSectionIDs[i] << "\n"; 
            }
        }
        std::cout << "\n--------------------\n";
        std::cout << "1. View Details for a Section\n";
        std::cout << "9. Back to Section Management\n\n";
        std::cout << "Enter choice: ";

        if (!(std::cin >> choice)) {
             std::cout << "Invalid input.\n"; std::cin.clear();
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             system("pause"); choice = 0; continue;
        }

        switch(choice) {
            case 1:
                if (allSectionIDs.empty()) {
                    std::cout << "No sections available to view details for.\n";
                    system("pause");
                } else {
                    admin_ViewSpecificSectionDetails(allSectionIDs, allStudents, allSchedule, allCourses, allTeachers);
                }
                break;
            case 9:
                break; 
            default:
                std::cout << "Invalid choice.\n"; system("pause"); break;
        }
    } while (choice != 9);
}

/*
    Adds a new Section ID (e.g., "BCS-2B").
    Checks for duplicates.
*/
void admin_AddSection(std::vector<std::string>& allSectionIDs) {
    system("cls");
    std::cout << "--- Add New Section ---\n\n";
    std::string newID;
    std::cout << "Enter new Section ID (e.g., BCS-1A): ";
    std::cin >> newID;
    
    if (std::find(allSectionIDs.begin(), allSectionIDs.end(), newID) != allSectionIDs.end()) { 
        std::cout << "Error: Section exists.\n"; 
    }
    else { 
        allSectionIDs.push_back(newID); 
        std::cout << "Section added.\n"; 
    }
    system("pause");
}

/*
    Removes a section.
    WARNING: This is a big operation. It sets all students in that section to "Unassigned"
    and deletes every schedule entry associated with that section.
*/
void admin_RemoveSection(std::vector<std::string>& allSectionIDs, std::vector<Student>& allStudents, std::vector<ScheduleEntry>& allSchedule) {
    system("cls");
    std::cout << "--- Remove Section ---\n\n";
    if (allSectionIDs.empty()) { std::cout << "No sections to remove.\n"; system("pause"); return; }
    
    // 1. Select section
    std::cout << "Select section to remove:\n";
    for (size_t i = 0; i < allSectionIDs.size(); ++i) { std::cout << i + 1 << ". " << allSectionIDs[i] << "\n"; }
    std::cout << "Enter choice (0 to cancel): ";
    int choice;
    if(!(std::cin >> choice)){ std::cout << "Invalid input.\n"; std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); system("pause"); return;}

    if (choice > 0 && choice <= static_cast<int>(allSectionIDs.size())) {
        std::string idToRemove = allSectionIDs[choice - 1];
        
        // 2. Warn the admin
        std::cout << "WARNING: Removing section '" << idToRemove << "'!\n";
        std::cout << "Unassigns students, removes schedule entries.\n";
        std::cout << "Type 'CONFIRM' to proceed: ";
        std::string confirmation;
        std::cin >> confirmation;

        if (confirmation == "CONFIRM") {
            // 3. Cleanup students
            for (auto& student : allStudents) { 
                if (student.sectionID == idToRemove) { 
                    student.sectionID = "Unassigned"; 
                } 
            }
            // 4. Cleanup schedule using erase-remove
            allSchedule.erase(std::remove_if(allSchedule.begin(), allSchedule.end(), 
                              [&](const ScheduleEntry& entry){ return entry.sectionID == idToRemove; }), allSchedule.end());
            // 5. Remove the ID from the list
            allSectionIDs.erase(allSectionIDs.begin() + (choice - 1));
            std::cout << "Section removed.\n";
        } else { std::cout << "Removal cancelled.\n"; }
    } else if (choice != 0) { std::cout << "Invalid choice.\n"; }
    else { std::cout << "Operation cancelled.\n"; }
    system("pause");
}

/*
    Menu for managing Teachers.
    Add/Remove teachers and assign them courses.
*/
void admin_ManageTeachers(std::vector<Teacher>& allTeachers, const std::vector<Course>& allCourses) {
    int choice;
    do {
        system("cls");
        std::cout << "--- Teacher Management ---\n";
        std::cout << "1. View All Teachers\n";
        std::cout << "2. Add New Teacher\n";
        std::cout << "3. Remove Teacher\n";
        std::cout << "4. Assign/Remove Course from Teacher\n";
        std::cout << "9. Back to Admin Dashboard\n\n";
        std::cout << "Enter your choice: ";
        
        if (!(std::cin >> choice)) {
              std::cout << "Invalid input.\n"; std::cin.clear();
              std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
              system("pause"); choice = 0; continue;
           }

        switch (choice) {
            case 1:
                admin_ViewTeachers(allTeachers);
                break;
            case 2:
                admin_AddTeacher(allTeachers, allCourses);
                break;
            case 3:
                admin_RemoveTeacher(allTeachers);
                break;
            case 4:
                admin_ManageTeacherCourses(allTeachers, allCourses); 
                break;
            case 9:
                break; 
            default:
                std::cout << "Invalid choice.\n"; system("pause");
                break;
        }
    } while (choice != 9);
}

/*
    Assigns a course to a teacher.
    Checks if they are already teaching it first.
*/
void admin_AddCourseToTeacher(Teacher& teacher, const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Assign Course to " << teacher.name << " ---\n\n";
    if (allCourses.empty()) {
        std::cout << "No courses exist in the system.\n";
        system("pause");
        return;
    }

    std::cout << "Select a course to assign:\n";
    for (size_t i = 0; i < allCourses.size(); ++i) {
        std::cout << i + 1 << ". " << allCourses[i].courseCode << " - " << allCourses[i].courseName << "\n";
    }
    std::cout << "\nChoice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice < 0 || choice > static_cast<int>(allCourses.size())) {
          std::cout << "Invalid choice.\n"; std::cin.clear();
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          system("pause"); return;
    }
    if (choice == 0) { std::cout << "Cancelled.\n"; system("pause"); return; }

    std::string selectedCode = allCourses[choice - 1].courseCode;

    // Check duplicate assignment
    for (const auto& code : teacher.assignedCourseCodes) {
        if (code == selectedCode) {
            std::cout << "Teacher is already assigned to " << selectedCode << ".\n";
            system("pause");
            return;
        }
    }

    teacher.assignedCourseCodes.push_back(selectedCode);
    std::cout << selectedCode << " assigned successfully.\n";
    std::cout << "Remember to save data by exiting.\n";
    system("pause");
}

/*
    Removes a course assignment from a teacher.
*/
void admin_RemoveCourseFromTeacher(Teacher& teacher) {
    system("cls");
    std::cout << "--- Remove Course from " << teacher.name << " ---\n\n";
    if (teacher.assignedCourseCodes.empty()) {
        std::cout << "This teacher is not assigned to any courses.\n";
        system("pause");
        return;
    }

    std::cout << "Select a course to remove:\n";
    for (size_t i = 0; i < teacher.assignedCourseCodes.size(); ++i) {
        std::cout << i + 1 << ". " << teacher.assignedCourseCodes[i] << "\n";
    }
    std::cout << "\nChoice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice < 0 || choice > static_cast<int>(teacher.assignedCourseCodes.size())) {
          std::cout << "Invalid choice.\n"; std::cin.clear();
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          system("pause"); return;
    }
    if (choice == 0) { std::cout << "Cancelled.\n"; system("pause"); return; }
    
    int indexToRemove = choice - 1;
    std::string codeToRemove = teacher.assignedCourseCodes[indexToRemove];
    teacher.assignedCourseCodes.erase(teacher.assignedCourseCodes.begin() + indexToRemove);
    
    std::cout << codeToRemove << " unassigned successfully.\n";
    std::cout << "Remember to save data by exiting.\n";
    system("pause");
}

/*
    Removes a teacher from the system.
    We include a warning because this doesn't auto-update the schedule.
    The admin has to manually fix the schedule if they delete a teacher.
*/
void admin_RemoveTeacher(std::vector<Teacher>& allTeachers) {
    system("cls");
    std::cout << "--- Remove Teacher ---\n\n";
    if (allTeachers.empty()) {
        std::cout << "No teachers in the system to remove.\n";
        system("pause");
        return;
    }
    
    std::cout << "Select teacher to remove:\n";
    for (size_t i = 0; i < allTeachers.size(); ++i) {
        std::cout << i + 1 << ". " << allTeachers[i].name << " (" << allTeachers[i].username << ")\n";
    }
    std::cout << "\nEnter number of teacher to remove (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice < 0 || choice > static_cast<int>(allTeachers.size())) {
          std::cout << "Invalid choice.\n"; std::cin.clear();
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          system("pause"); return;
    }
    if (choice == 0) { std::cout << "Removal cancelled.\n"; system("pause"); return; }

    int indexToRemove = choice - 1;
    
    std::cout << "\nWARNING: You are about to remove '" << allTeachers[indexToRemove].name << "'.\n";
    std::cout << "This will NOT automatically remove them from the schedule.\n";
    std::cout << "You must update the schedule manually to remove them from classes.\n";
    std::cout << "Type 'CONFIRM' to proceed: ";
    std::string confirm;
    std::cin >> confirm;

    if (confirm == "CONFIRM") {
        allTeachers.erase(allTeachers.begin() + indexToRemove);
        std::cout << "Teacher removed.\n";
        std::cout << "Remember to save data by exiting the main menu.\n";
    } else {
        std::cout << "Removal cancelled.\n";
    }
    system("pause");
}

/*
    Sub-menu for specific teacher modifications.
    Lets you add or remove courses for one teacher.
*/
void admin_ManageTeacherCourses(std::vector<Teacher>& allTeachers, const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Assign/Remove Course from Teacher ---\n\n";
    if (allTeachers.empty()) {
        std::cout << "No teachers in the system.\n";
        system("pause");
        return;
    }

    std::cout << "Select Teacher to modify:\n";
    for (size_t i = 0; i < allTeachers.size(); ++i) {
        std::cout << i + 1 << ". " << allTeachers[i].name << "\n";
    }
    std::cout << "\nChoice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice < 0 || choice > static_cast<int>(allTeachers.size())) {
        std::cout << "Invalid choice.\n"; std::cin.clear();
          std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
          system("pause"); return;
    }
    if (choice == 0) { std::cout << "Cancelled.\n"; system("pause"); return; }
    
    Teacher& selectedTeacher = allTeachers[choice - 1]; 

    int subChoice;
    do {
        system("cls");
        std::cout << "Modifying: " << selectedTeacher.name << " (" << selectedTeacher.username << ")\n";
        
        std::cout << "Current Courses: ";
        if (selectedTeacher.assignedCourseCodes.empty()) {
            std::cout << "None\n";
        } else {
            for (const auto& code : selectedTeacher.assignedCourseCodes) {
                std::cout << code << " ";
            }
            std::cout << "\n";
        }
        std::cout << "-----------------------------------\n";
        std::cout << "1. Assign a new course\n";
        std::cout << "2. Remove an assigned course\n";
        std::cout << "9. Back to Teacher Management\n\n";
        std::cout << "Enter choice: ";

        if (!(std::cin >> subChoice)) {
              std::cout << "Invalid input.\n"; std::cin.clear();
              std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
              system("pause"); subChoice = 0; continue;
        }

        switch (subChoice) {
            case 1:
                admin_AddCourseToTeacher(selectedTeacher, allCourses);
                break;
            case 2:
                admin_RemoveCourseFromTeacher(selectedTeacher);
                break;
            case 9:
                break; 
            default:
                std::cout << "Invalid choice.\n"; system("pause");
        }
    } while (subChoice != 9);
}

/*
    Adds a new teacher to the system.
    We check for unique usernames and let the admin assign courses right away.
*/
void admin_AddTeacher(std::vector<Teacher>& allTeachers, const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Add New Teacher ---\n\n";
    Teacher newTeacher;

    std::cout << "Enter Teacher's Full Name (e.g., Dr._New_Teacher): ";
    std::cin >> newTeacher.name;
    
    std::cout << "Enter Teacher's Username (e.g., SamiaRiaz): ";
    std::cin >> newTeacher.username;

    // Check duplicate username
    for (const auto& teacher : allTeachers) {
        if (teacher.username == newTeacher.username) {
            std::cout << "Error: A teacher with this username already exists.\n";
            system("pause");
            return;
        }
    }

    std::cout << "Enter default password: ";
    std::cin >> newTeacher.password;

    std::cout << "\n--- Assign Courses ---\n";
    if (allCourses.empty()) {
        std::cout << "No courses available to assign.\n";
    } else {
        int courseChoice;
        do {
            std::cout << "Available Courses:\n";
            for (size_t i = 0; i < allCourses.size(); ++i) {
                std::cout << i + 1 << ". " << allCourses[i].courseCode << " - " << allCourses[i].courseName << "\n";
            }
            std::cout << "Enter course number to assign (0 to finish): ";

            if (!(std::cin >> courseChoice)) {
                 std::cout << "Invalid input. Please enter a number.\n";
                 std::cin.clear();
                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                 courseChoice = -1; 
                 continue;
            }

            if (courseChoice > 0 && courseChoice <= static_cast<int>(allCourses.size())) {
                std::string selectedCode = allCourses[courseChoice - 1].courseCode;
                bool alreadyAssigned = false;
                for (const auto& code : newTeacher.assignedCourseCodes) {
                    if (code == selectedCode) {
                        alreadyAssigned = true;
                        break;
                    }
                }
                if (alreadyAssigned) {
                    std::cout << "Course already assigned to this teacher.\n";
                } else {
                    newTeacher.assignedCourseCodes.push_back(selectedCode);
                    std::cout << "'" << selectedCode << "' assigned.\n";
                }
            } else if (courseChoice != 0) {
                std::cout << "Invalid choice.\n";
            }
        } while (courseChoice != 0);
    }

    allTeachers.push_back(newTeacher);
    std::cout << "\nTeacher '" << newTeacher.name << "' (" << newTeacher.username << ") added successfully!\n";
    std::cout << "Remember to save data by exiting the main menu.\n";
    system("pause");
}

/*
    Prints a list of all teachers, showing their usernames, names, passwords,
    and what courses they are teaching.
*/
void admin_ViewTeachers(const std::vector<Teacher>& allTeachers) {
    system("cls");
    std::cout << "--- All Teacher Records ---\n\n";
    std::cout << std::left
              << std::setw(25) << "Username"
              << std::setw(30) << "Full Name"
              << std::setw(15) << "Password"
              << "Assigned Courses\n";
    std::cout << std::string(85, '-') << "\n";

    if (allTeachers.empty()) {
        std::cout << "No teacher records found.\n";
    } else {
        for (const auto& teacher : allTeachers) {
            std::cout << std::left
                      << std::setw(25) << teacher.username
                      << std::setw(30) << teacher.name
                      << std::setw(15) << teacher.password;
            
            // Show assigned courses in one line
            std::string courses = "";
            for(const auto& code : teacher.assignedCourseCodes) {
                courses += code + " ";
            }
            std::cout << courses << "\n";
        }
    }
    std::cout << "\n";
    system("pause");
}

/*
    Sub-menu for viewing student details.
    We can see a summary list or pick one student to see everything.
*/
void admin_ViewStudentDetailsMenu(std::vector<Student>& allStudents, const std::vector<Course>& allCourses, 
                                  const std::vector<AttendanceRecord>& allAttendance,
                                  std::vector<SubmissionDefinition>& allSubmissions,
                                  std::vector<StudentSubmission>& studentSubmissions) {
    int choice;
    do {
        system("cls");
        std::cout << "--- View Student Details ---\n";
        std::cout << "1. View All Students (Summary)\n";
        std::cout << "2. View Specific Student Details\n";
        std::cout << "9. Back to Student Management\n\n";
        std::cout << "Enter choice: ";

        if (!(std::cin >> choice)) {
              std::cout << "Invalid input.\n"; std::cin.clear();
              std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
              system("pause"); choice = 0; continue;
        }

        switch(choice) {
            case 1:
                admin_ViewAllStudents(allStudents); 
                break;
            case 2:
                admin_ViewSpecificStudent(allStudents, allCourses, allAttendance,
                                          allSubmissions, studentSubmissions); 
                break;
            case 9:
                break; 
            default:
                std::cout << "Invalid choice.\n"; system("pause"); break;
        }
    } while (choice != 9);
}

// Forward declarations for calculation helpers (defined in utility or other files)
// double calculateCGPA(const std::vector<Grade>& grades);
// double calculateStudentAttendancePercentage(const std::string& studentUsername, const std::string& courseCode, const std::vector<AttendanceRecord>& allAttendance);
// void updateStudentFinalMark(const std::string& studentUsername, const std::string& courseCode, std::vector<Student>& allStudents, const std::vector<SubmissionDefinition>& allSubmissions, const std::vector<StudentSubmission>& studentSubmissions);


/*
    Digs into the details of one specific student.
    Before we show the data, we recalculate their marks to make sure everything is up to date.
    Then we show grades, attendance, and allow editing specific submission marks.
*/
void admin_ViewSpecificStudent(std::vector<Student>& allStudents, const std::vector<Course>& allCourses, 
                               const std::vector<AttendanceRecord>& allAttendance,
                               std::vector<SubmissionDefinition>& allSubmissions,
                               std::vector<StudentSubmission>& studentSubmissions) {
    system("cls");
    std::cout << "--- View Specific Student Details ---\n\n";
    std::string studentID;
    std::cout << "Enter Student ID (Roll No): ";
    std::cin >> studentID;

    // Find the student
    Student* foundStudent = nullptr;
    for (auto& student : allStudents) {
        if (student.studentID == studentID) {
            foundStudent = &student;
            break;
        }
    }

    if (!foundStudent) {
        std::cout << "Error: Student with Roll No '" << studentID << "' not found.\n";
        system("pause");
        return;
    }

    // --- Recalculate marks before displaying ---
    // We need to do this because assignment marks might have changed.
    // We use a temp list because the helper function expects a vector of students.
    std::vector<Student> tempStudentList = {*foundStudent}; 
    
    for (size_t i = 0; i < foundStudent->grades.size(); ++i) {
         updateStudentFinalMark(foundStudent->username, foundStudent->grades[i].courseCode, 
                                 tempStudentList, allSubmissions, studentSubmissions);
                                
         // Copy the updated mark back to the real student object
         foundStudent->grades[i].mark = tempStudentList[0].grades[i].mark;
    }

    // Calculate stats for display
    double cgpa = calculateCGPA(foundStudent->grades);
    int presentCount = 0;
    int totalCount = 0;
    for (const auto& record : allAttendance) {
        if (record.studentUsername == foundStudent->username) {
            totalCount++;
            if (record.status == 'P') presentCount++;
        }
    }
    double overallAttendancePerc = (totalCount == 0) ? 100.0 : (static_cast<double>(presentCount) / totalCount) * 100.0;

    int choice;
    int labelWidth = 16;
    do {
        system("cls");
        std::cout << "--- Details for Student --- \n\n";
        std::cout << std::left << std::setw(labelWidth) << "Name:"            << foundStudent->name << "\n";
        std::cout << std::left << std::setw(labelWidth) << "Roll No:"         << foundStudent->studentID << "\n";
        std::cout << std::left << std::setw(labelWidth) << "Username:"        << foundStudent->username << "\n";
        std::cout << std::left << std::setw(labelWidth) << "Section:"         << foundStudent->sectionID << "\n\n";
        
        std::cout << std::left << std::setw(labelWidth) << "CGPA:"            << std::fixed << std::setprecision(2) << cgpa << std::setprecision(0) << "\n";
        std::cout << std::left << std::setw(labelWidth) << "Attendance Avg.:"<< std::fixed << std::setprecision(1) << overallAttendancePerc << "%" << std::setprecision(0) << "\n";
        std::cout << std::left << std::setw(labelWidth) << "Ratio (P/T):"    << presentCount << "/" << totalCount << " (Present/Total Marked)\n";
        std::cout << "\n========================================\n\n";

        std::cout << "1. View Courses & Detailed Marks\n"; 
        std::cout << "2. View Attendance History\n";
        std::cout << "3. Edit Submission Marks\n"; 
        std::cout << "9. Back to View Details Menu\n\n";
        std::cout << "Enter choice: ";

        if (!(std::cin >> choice)) {
              std::cout << "Invalid input.\n"; std::cin.clear();
              std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
              system("pause"); choice = 0; continue;
        }

        switch(choice) {
            case 1: // View Courses & Marks
                {
                    system("cls");
                    std::cout << "--- Gradebook for " << foundStudent->name << " ---\n\n";
                    size_t maxLen = 11;
                    for(const auto& g : foundStudent->grades) { 
                        if(getCourseName(g.courseCode, allCourses).length() > maxLen) 
                            maxLen = getCourseName(g.courseCode, allCourses).length();
                    }
                    size_t nameWidth = maxLen + 2;

                    std::cout << std::left << std::setw(15) << "Course Code" << std::setw(nameWidth) << "Course Name" << std::setw(10) << "Mark (%)" << std::setw(15) << "Attend. (%)" << "\n";
                    std::cout << std::string(15 + nameWidth + 10 + 15, '-') << "\n";

                    if (foundStudent->grades.empty()) { 
                        std::cout << "Not enrolled in any courses.\n";
                    } else {
                        for(const auto& grade : foundStudent->grades) {
                            double attPerc = calculateStudentAttendancePercentage(foundStudent->username, grade.courseCode, allAttendance);
                            std::cout << std::left << std::setw(15) << grade.courseCode << std::setw(nameWidth) << getCourseName(grade.courseCode, allCourses)
                                      << std::setw(10) << grade.mark
                                      << std::fixed << std::setprecision(1) << std::setw(15);
                            if (attPerc < 0) std::cout << "N/A"; else std::cout << attPerc;
                            std::cout << std::setprecision(0) << "\n";
                        }
                    }
                    std::cout << "\n";
                    system("pause");
                }
                break;
            case 2: // View Attendance History
                {
                    system("cls");
                    std::cout << "--- Attendance History for " << foundStudent->name << " ---\n\n";
                    // Organize attendance records by course code
                    std::map<std::string, std::vector<std::pair<std::string, char>>> groupedAtt;
                    for (const auto& rec : allAttendance) { 
                        if (rec.studentUsername == foundStudent->username) { 
                            groupedAtt[rec.courseCode].push_back({rec.date, rec.status});
                        }
                    }

                    if (groupedAtt.empty()) { 
                        std::cout << "No attendance records found.\n"; 
                    } else {
                        for (const auto& pair : groupedAtt) {
                            std::cout << "--- Course: " << pair.first << " (" << getCourseName(pair.first, allCourses) << ") ---\n";
                            std::cout << std::left << std::setw(20) << "Date" << std::setw(10) << "Status" << "\n" << std::string(30, '-') << "\n";
                            for (const auto& recPair : pair.second) { 
                                std::cout << std::left << std::setw(20) << recPair.first << std::setw(10) << (recPair.second == 'P' ? "Present" : "Absent") << "\n"; 
                            }
                            std::cout << "\n";
                        }
                    }
                    system("pause");
                }
                break;
                case 3: // Edit Submission Marks
                {
                    // Lets the admin manually change a mark for a specific assignment
                    admin_EditStudentSubmissionMarks(*foundStudent, allStudents, allCourses, allSubmissions, studentSubmissions);
                    
                    // Recalculate again after the edit
                    for (auto& grade : foundStudent->grades) {
                         updateStudentFinalMark(foundStudent->username, grade.courseCode, 
                                                 allStudents, allSubmissions, studentSubmissions);
                    }
                    cgpa = calculateCGPA(foundStudent->grades);
                }
                break;
            case 9:
                break; 
            default:
                std::cout << "Invalid choice.\n"; system("pause"); break;
        }

    } while (choice != 9);
}

// Helper prototypes for section details (assumed defined elsewhere)
// std::string getCourseName(const std::string& courseCode, const std::vector<Course>& allCourses);
// std::string getTeacherName(const std::string& teacherUsername, const std::vector<Teacher>& allTeachers);

/*
    Shows a detailed view of a Section.
    It lists every student assigned to that section (sorted by Roll No),
    and lists every course + teacher scheduled for that section.
*/
void admin_ViewSpecificSectionDetails(const std::vector<std::string>& allSectionIDs, const std::vector<Student>& allStudents,
                                      const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                                      const std::vector<Teacher>& allTeachers) {

    system("cls");
    std::cout << "--- View Section Details ---\n\n";
    
    std::cout << "Available Sections:\n";
    for (size_t i = 0; i < allSectionIDs.size(); ++i) {
         std::cout << i + 1 << ". " << allSectionIDs[i] << "\n";
    }
    std::cout << "\nEnter section number to view details (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice < 0 || choice > static_cast<int>(allSectionIDs.size())) {
          std::cout << "Invalid choice.\n"; std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); system("pause"); return;
    }
    if (choice == 0) {
        std::cout << "Operation cancelled.\n";
        return;
    }

    std::string selectedSectionID = allSectionIDs[choice - 1];

    // --- Display Students ---
    system("cls"); 
    std::cout << "--- Details for Section: " << selectedSectionID << " ---\n\n";
    std::cout << "** Students **\n";
    std::cout << std::left << std::setw(15) << "Roll No." << std::setw(30) << "Name" << "\n";
    std::cout << std::string(45, '-') << "\n";

    std::vector<Student> studentsInSection;
    for (const auto& student : allStudents) {
        if (student.sectionID == selectedSectionID) {
            studentsInSection.push_back(student);
        }
    }

    if (studentsInSection.empty()) {
        std::cout << "No students currently assigned to this section.\n";
    } else {
        // Sort numerically
          std::sort(studentsInSection.begin(), studentsInSection.end(),
             [](const Student& a, const Student& b) {
                 try {
                     int idA = std::stoi(a.studentID);
                     int idB = std::stoi(b.studentID);
                     return idA < idB;
                 } catch (...) { return false; } 
             });

        for (const auto& student : studentsInSection) {
            std::cout << std::left << std::setw(15) << student.studentID
                      << std::setw(30) << student.name << "\n";
        }
    }

    // --- Display Courses and Teachers ---
    std::cout << "\n** Courses & Teachers **\n";
    std::cout << std::left << std::setw(15) << "Course Code"
              << std::setw(40) << "Course Name"
              << std::setw(30) << "Teacher Name" << "\n";
    std::cout << std::string(85, '-') << "\n";

    std::map<std::string, std::string> coursesInSchedule; 
    for(const auto& entry : allSchedule) {
        if (entry.sectionID == selectedSectionID) {
            if (coursesInSchedule.find(entry.courseCode) == coursesInSchedule.end()) {
                 coursesInSchedule[entry.courseCode] = entry.teacherID;
            }
        }
    }

    if (coursesInSchedule.empty()) {
        std::cout << "No courses scheduled for this section.\n";
    } else {
        for (const auto& pair : coursesInSchedule) {
            std::cout << std::left << std::setw(15) << pair.first 
                      << std::setw(40) << getCourseName(pair.first, allCourses) 
                      << std::setw(30) << getTeacherName(pair.second, allTeachers) 
                      << "\n";
        }
    }

    std::cout << "\n";
    system("pause"); 
}

// Helper Prototype (assumed in a header, defined elsewhere)
// StudentSubmission* findStudentSubmission(const std::string& submissionID, const std::string& studentUsername, std::vector<StudentSubmission>& studentSubmissions);

/*
    This lets the admin manually edit the marks for a specific student's submission.
    After editing, it triggers a recalculation of the student's final course mark.
*/
void admin_EditStudentSubmissionMarks(Student& student, std::vector<Student>& allStudents,
                                      const std::vector<Course>& allCourses,
                                      std::vector<SubmissionDefinition>& allSubmissions,
                                      std::vector<StudentSubmission>& studentSubmissions) {
    system("cls");
    std::cout << "--- Edit Submission Marks for " << student.name << " ---\n\n";
    if (student.grades.empty()) {
        std::cout << "Student is not enrolled in any courses.\n"; system("pause"); return;
    }

    // 1. Select course
    std::cout << "Select a course to edit marks for:\n";
    for(size_t i = 0; i < student.grades.size(); ++i) {
        std::cout << i + 1 << ". " << student.grades[i].courseCode 
                  << " (" << getCourseName(student.grades[i].courseCode, allCourses) << ")\n";
    }
    std::cout << "\nEnter choice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice <= 0 || choice > student.grades.size()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Cancelled or invalid input.\n"; system("pause"); return;
    }
    
    std::string selectedCourse = student.grades[choice - 1].courseCode;

    // 2. Find all submissions for that course/section
    std::vector<const SubmissionDefinition*> courseSubmissions;
    for(const auto& sub : allSubmissions) {
        if(sub.courseCode == selectedCourse && sub.sectionID == student.sectionID) {
            courseSubmissions.push_back(&sub);
        }
    }
    if (courseSubmissions.empty()) {
        std::cout << "No submissions (assignments, quizzes, etc.) found for this course and section.\n";
        system("pause"); return;
    }
    
    // 3. Select specific submission
    system("cls");
    std::cout << "--- Editing marks for " << selectedCourse << " ---\n";
    std::cout << "Select submission to edit:\n";
    for(size_t i = 0; i < courseSubmissions.size(); ++i) {
        const StudentSubmission* ss = findStudentSubmission(courseSubmissions[i]->submissionID, student.username, studentSubmissions);
        
        std::cout << i + 1 << ". " << std::setw(20) << std::left << courseSubmissions[i]->title
                  << " (Current: ";
        if(ss) 
            std::cout << ss->obtainedMarks << " / " << courseSubmissions[i]->totalMarks << ")\n";
        else 
            std::cout << "N/A - Mark record missing)\n";
    }
    std::cout << "\nEnter choice (0 to cancel): ";
    if (!(std::cin >> choice) || choice <= 0 || choice > courseSubmissions.size()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Cancelled or invalid input.\n"; system("pause"); return;
    }

    const SubmissionDefinition* selectedSub = courseSubmissions[choice - 1];

    // 4. Get the mark record
    StudentSubmission* ssToEdit = findStudentSubmission(selectedSub->submissionID, student.username, studentSubmissions);
    if (!ssToEdit) {
        std::cout << "Error! Student's mark record for this submission does not exist!\n";
        system("pause"); return;
    }

    // 5. Update the mark
    std::cout << "\nEditing: " << selectedSub->title << " for " << student.name << "\n";
    std::cout << "Current Mark: " << ssToEdit->obtainedMarks << " / " << selectedSub->totalMarks << "\n";
    std::cout << "Enter new mark: ";
    double newMark;
    if (!(std::cin >> newMark)) {
        std::cout << "Invalid input.\n";
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    } else if (newMark < 0 || newMark > selectedSub->totalMarks) {
        std::cout << "Mark must be between 0 and " << selectedSub->totalMarks << ".\n";
    } else {
        ssToEdit->obtainedMarks = newMark; // Apply new mark
        std::cout << "Mark updated.\n";
        
        // Update final grade now
        updateStudentFinalMark(student.username, selectedCourse, 
                               allStudents, allSubmissions, studentSubmissions);
        std::cout << "Student's final grade for " << selectedCourse << " has been recalculated.\n";
    }
    system("pause");
}
