
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
 --- FILE: teacher.cpp ---
============================================================================

 Summary:
 This file contains all the logic for the Teacher's side of the application.
 It handles:
 1. The Teacher Dashboard (Main Menu).
 2. Viewing personal schedules (filtering the master schedule).
 3. Viewing student rosters and current standing.
 4. The Grading System: Creating assignments, entering marks, and recalculating grades.
 5. The Attendance System: Marking students present/absent and fixing past mistakes.

============================================================================
*/


#include "../headers/teacher.h"         // Teacher-specific structures and functions
#include "../headers/student.h"         // Student-specific structures and functions
#include "../headers/course.h"          // Course structures
#include "../headers/schedule.h"        // Schedule entry structures and display functions
#include "../headers/utility.h"         // Helper functions like changePassword, getCurrentDate

#include <iostream>                     // For console input/output
#include <iomanip>                      // For output formatting (setw, setprecision)
#include <limits>                       // For std::numeric_limits (input validation)
#include <vector>                       // For std::vector container
#include <string>                       // For std::string class
#include <set>                          // For std::set (to store unique sections/courses)
#include <map>                          // For std::map (to group data)
#include <numeric>                      // For numeric operations (if needed)
#include <algorithm>                    // For std::sort, std::remove_if, std::find
#include <tuple>                        // For std::tuple (used in attendance modification)

// --- Helper Function Prototypes ---
void teacher_ViewMySchedule(const Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses, const std::vector<Teacher>& allTeachers);
void teacher_ViewMyStudents(const Teacher& teacher, const std::vector<Student>& allStudents, const std::vector<Course>& allCourses, const std::vector<ScheduleEntry>& allSchedule, const std::vector<AttendanceRecord>& allAttendance);

// The new main sub-menu for grade management
void teacher_MarksMenu(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                        std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                        std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses);

// Handles the creation of a new submission (quiz, assignment, etc.)
void teacher_AddSubmission(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                           const std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                           std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses);

// Function to delete an existing submission and clear student marks
void teacher_DeleteSubmission(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                              std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                              std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses);

// Function to modify a submission's total marks (and scale student marks)
void teacher_ModifySubmission(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                              std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                              std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses);

// This prototype is for the "Enter/Edit Student Marks" function
void teacher_ManageStudentMarks(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                                std::vector<Student>& allStudents, const std::vector<SubmissionDefinition>& allSubmissions,
                                std::vector<StudentSubmission>& studentSubmissions);

// Sub-menu for attendance options
void teacher_AttendanceMenu(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                            const std::vector<Student>& allStudents, std::vector<AttendanceRecord>& allAttendance,
                            const std::vector<Course>& allCourses);

// Function to mark today's attendance
void teacher_MarkAttendance(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                            const std::vector<Student>& allStudents, std::vector<AttendanceRecord>& allAttendance,
                            const std::vector<Course>& allCourses);

// Function to modify existing attendance records
void teacher_ModifyAttendance(Teacher& teacher, std::vector<AttendanceRecord>& allAttendance,
                              const std::vector<Student>& allStudents, const std::vector<Course>& allCourses);


/*
    This is the main dashboard for the Teacher.
    It displays the courses the teacher is currently teaching (by looking at the schedule)
    and gives them the menu to manage their classes.
    
    It loops continually until the teacher chooses to Logout.
*/
void showTeacherDashboard(Teacher& loggedInTeacher, std::vector<Student>& allStudents,
                          const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                          const std::vector<Teacher>& allTeachers, std::vector<AttendanceRecord>& allAttendance,
                          std::vector<SubmissionDefinition>& allSubmissions,
                          std::vector<StudentSubmission>& studentSubmissions) {
    int choice;
    do {
        system("cls");
        // --- HEADER Display ---
        std::cout << "COMSATS University Islamabad, Wah Campus\n";
        std::cout << "Date: " << getCurrentDate() << "\n";
        std::cout << "------------------------------------------------------------------\n";
        std::cout << "Teacher: " << loggedInTeacher.name << "\n";
        
        // We need to figure out what this teacher actually teaches so we can show it in the header.
        // We use a map to group sections by course code.
        std::map<std::string, std::set<std::string>> coursesTaught; 
        for(const auto& entry : allSchedule) {
            if (entry.teacherID == loggedInTeacher.username) {
                coursesTaught[entry.courseCode].insert(entry.sectionID);
            }
        }
        
        if (coursesTaught.empty()) {
            std::cout << "Not currently assigned to any courses in the schedule.\n";
        } else {
            std::cout << "Courses Taught:\n";
            for (const auto& pair : coursesTaught) {
                std::cout << "  - " << getCourseName(pair.first, allCourses) << " (";
                for (const auto& section : pair.second) {
                    std::cout << section << " ";
                }
                std::cout << ")\n";
            }
        }
        std::cout << "==================================================================\n\n";

        // --- TEACHER DASHBOARD MENU ---
        std::cout << "1. View My Class Schedule\n";
        std::cout << "2. View Class Roster & Grades\n"; 
        std::cout << "3. Manage Marks & Submissions\n";
        std::cout << "4. Manage Attendance\n";         
        std::cout << "5. Change My Password\n";         
        std::cout << "9. Logout\n\n";
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
                // View the weekly schedule for this teacher
                teacher_ViewMySchedule(loggedInTeacher, allSchedule, allCourses, allTeachers);
                break;
            case 2:
                // View students, their marks, and attendance summary
                teacher_ViewMyStudents(loggedInTeacher, allStudents, allCourses, allSchedule, allAttendance);
                break;
            case 3:
                // Go to the Marks/Submissions sub-menu
                teacher_MarksMenu(loggedInTeacher, allSchedule, allStudents, allSubmissions, studentSubmissions, allCourses);
                break;
            case 4: 
                // Go to the Attendance sub-menu
                teacher_AttendanceMenu(loggedInTeacher, allSchedule, allStudents, allAttendance, allCourses);
                break;
            case 5: 
                changePassword(loggedInTeacher);
                break;
            case 9:
                std::cout << "Logging out...\n";
                break;
            default:
                std::cout << "Invalid choice.\n";
                system("pause");
                break;
        }
    } while (choice != 9);

}

// --- Helper Function Definitions ---

/*
    Shows the teacher's weekly schedule.
    The system holds one giant schedule for everyone (`allSchedule`).
    This function filters that list to find only the entries where `teacherID` matches this teacher.
*/

void teacher_ViewMySchedule(const Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses, const std::vector<Teacher>& allTeachers) {
    system("cls");
    std::string title = "My Weekly Schedule (" + teacher.name + ")";
    std::cout << "--- " << title << " ---\n\n";

    // Filter schedule entries for the specific teacher
    std::vector<ScheduleEntry> mySchedule;
    for (const auto& entry : allSchedule) {
        if (entry.teacherID == teacher.username) {
            mySchedule.push_back(entry);
        }
    }

      // Prepare a string listing the sections involved for the header
      std::set<std::string> sections;
      for(const auto& entry : mySchedule) sections.insert(entry.sectionID);
      std::string section_info = " Sections: ";
      if(!sections.empty()){
          for(const auto& s : sections) section_info += s + " ";
      } else {
          section_info = " (No classes scheduled)";
      }

    // Call the common display function (defined in schedule.cpp) which draws the grid
    displayScheduleGrid(mySchedule, allCourses, allTeachers, title + section_info);

    std::cout << "\n";
    system("pause");
}

/*
    Displays the roster (list of students).
    It iterates through the schedule to see which sections this teacher has,
    then finds all students in those sections. 
    It also prints their current Mark and Attendance % for the teacher's specific courses.
*/
void teacher_ViewMyStudents(const Teacher& teacher, const std::vector<Student>& allStudents, const std::vector<Course>& allCourses, const std::vector<ScheduleEntry>& allSchedule, const std::vector<AttendanceRecord>& allAttendance) { 
    system("cls");
    std::cout << "--- My Students & Marks ---\n\n";

    // 1. Find all unique sections the teacher teaches
    std::set<std::string> taughtSections;
    std::map<std::string, std::vector<std::string>> sectionCourses; // map<SectionID, vector<CourseCode>>
    
    for(const auto& entry : allSchedule) { 
        if (entry.teacherID == teacher.username) {
            taughtSections.insert(entry.sectionID);
            
            // Ensure we list the course code for this section (avoiding duplicates)
            bool found = false;
            for(const auto& code : sectionCourses[entry.sectionID]) {
                if (code == entry.courseCode) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                sectionCourses[entry.sectionID].push_back(entry.courseCode);
            }
        }
    }

    if (taughtSections.empty()) {
        std::cout << "You are not assigned to any classes in the schedule.\n";
        system("pause");
        return;
    }

    // 2. Loop through each section found
    for (const std::string& sectionID : taughtSections) {
        std::cout << "--- Students in Section: " << sectionID << " ---\n";
        const auto& coursesInSection = sectionCourses[sectionID];
        if (coursesInSection.empty()) continue;

        // --- Print Table Header ---
        std::cout << std::left
                  << std::setw(15) << "Roll Number"
                  << std::setw(25) << "Name";
        // Add columns for every course taught in this section
        for(const std::string& courseCode : coursesInSection) {
            std::cout << std::setw(10) << courseCode.substr(0,9); // Course Code (Mark)
            std::cout << std::setw(10) << "Attd.(%)";            // Attendance %
        }
        
        size_t totalWidth = 40 + coursesInSection.size() * 20; 
        std::cout << "\n" << std::string(totalWidth, '-') << "\n";

        bool foundStudentsInSection = false;
        // 3. Find students belonging to this section
        for (const auto& student : allStudents) {
            if (student.sectionID == sectionID) {
                foundStudentsInSection = true;
                std::cout << std::left
                          << std::setw(15) << student.studentID
                          << std::setw(25) << student.name;

                // 4. Print mark and attendance for each course
                for(const std::string& courseCode : coursesInSection) {
                    // Find Mark in student's grade vector
                    int mark = -1;
                    for(const auto& grade : student.grades) {
                        if (grade.courseCode == courseCode) {
                            mark = grade.mark; break;
                        }
                    }
                    if (mark != -1) { std::cout << std::setw(10) << mark; }
                    else { std::cout << std::setw(10) << "N/A"; }

                    // Calculate Attendance Percentage
                    double attendancePercent = calculateStudentAttendancePercentage(student.username, courseCode, allAttendance);
                     std::cout << std::fixed << std::setprecision(1) << std::setw(10);
                     if (attendancePercent < 0) { std::cout << "N/A"; } 
                     else { std::cout << attendancePercent; }
                     std::cout << std::setprecision(0); // Reset precision
                }
                std::cout << "\n";
            }
        } 

        // 5. Calculate and display class averages at the bottom
        if (foundStudentsInSection) {
            std::cout << std::string(totalWidth, '-') << "\n";
            std::cout << std::left << std::setw(40) << "Averages:"; 
            
            for(const std::string& courseCode : coursesInSection) {
                int courseMarkSum = 0;
                int studentCountMarks = 0; 
                
                 for (const auto& student : allStudents) {
                    if (student.sectionID == sectionID) {
                         for(const auto& grade : student.grades) {
                            if (grade.courseCode == courseCode && grade.mark >= 0) {
                                courseMarkSum += grade.mark;
                                studentCountMarks++; 
                                break;
                            }
                         }
                    }
                }
                 
                // Display Mark Average
                if(studentCountMarks > 0){ 
                    double courseAverage = static_cast<double>(courseMarkSum) / studentCountMarks; 
                    std::cout << std::fixed << std::setprecision(1) << std::setw(10) << courseAverage;
                } else {
                    std::cout << std::setw(10) << "N/A";
                }

                // Display Attendance Average
                double avgAttendance = calculateClassAttendancePercentage(sectionID, courseCode, allStudents, allAttendance);
                 std::cout << std::fixed << std::setprecision(1) << std::setw(10);
                 if (avgAttendance < 0) { std::cout << "N/A"; } 
                 else { std::cout << avgAttendance; }
                 std::cout << std::setprecision(0);
             }
             std::cout << "\n";
         } else {
             std::cout << "No students found in this section.\n";
         }
         std::cout << "\n";
    } 

    system("pause");
}

/*
    This allows the teacher to select a specific assignment/quiz and enter marks for it.
    It lists all students in the relevant section so the teacher can just go down the list
    and type in scores.
*/
void teacher_ManageStudentMarks(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                                std::vector<Student>& allStudents, const std::vector<SubmissionDefinition>& allSubmissions,
                                std::vector<StudentSubmission>& studentSubmissions) {
    system("cls");
    std::cout << "--- Manage Student Marks by Submission ---\n\n";

    // 1. Find submissions relevant to this teacher
    std::vector<const SubmissionDefinition*> mySubmissions;
    std::set<std::string> mySections; 
    for(const auto& entry : allSchedule) {
        if(entry.teacherID == teacher.username) mySections.insert(entry.sectionID);
    }

    for(const auto& sub : allSubmissions) {
        // Teacher must teach the section AND the course
        if(mySections.count(sub.sectionID)) {
            for(const auto& courseCode : teacher.assignedCourseCodes) {
                if(sub.courseCode == courseCode) {
                    mySubmissions.push_back(&sub);
                    break;
                }
            }
        }
    }

    if (mySubmissions.empty()) {
        std::cout << "You have not created any submissions yet that match your classes.\n";
        std::cout << "Go to 'Manage Marks & Submissions -> Add New Submission' to create one.\n";
        system("pause");
        return;
    }

    // 2. Let teacher select a submission
    std::cout << "Select a submission to enter/edit marks for:\n";
    for(size_t i = 0; i < mySubmissions.size(); ++i) {
        std::cout << i + 1 << ". [" << mySubmissions[i]->sectionID << " " << mySubmissions[i]->courseCode
                  << "] " << mySubmissions[i]->title << " (Type: " << mySubmissions[i]->type << ")\n";
    }
    std::cout << "\nEnter choice (0 to cancel): ";
    int choice;
    
    if (!(std::cin >> choice) || choice <= 0 || choice > mySubmissions.size()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Cancelled or invalid input.\n"; system("pause"); return;
    }

    const SubmissionDefinition* selectedSub = mySubmissions[choice - 1];

    // 3. List students and loop for editing
    system("cls");
    std::cout << "--- Editing: " << selectedSub->title << " (" << selectedSub->courseCode 
              << ") ---\n";
    std::cout << "Section: " << selectedSub->sectionID 
              << " | Total Marks: " << selectedSub->totalMarks << "\n\n";

    // Get list of students for this section
    std::vector<Student*> studentsInSection;
    for(auto& s : allStudents) {
        if(s.sectionID == selectedSub->sectionID) {
            studentsInSection.push_back(&s);
        }
    }
    
    // Sort students numerically by Roll No
    std::sort(studentsInSection.begin(), studentsInSection.end(),
        [](const Student* a, const Student* b) {
            try { return std::stoi(a->studentID) < std::stoi(b->studentID); }
            catch (...) { return false; }
        });


    std::string studentID;
    while(true) {
        // Show list of students and their current marks
        std::cout << std::left << std::setw(15) << "Roll No." << std::setw(30) << "Name" << "Mark\n";
        std::cout << std::string(55, '-') << "\n";
        for (const auto* s : studentsInSection) {
            StudentSubmission* ss = findStudentSubmission(selectedSub->submissionID, s->username, studentSubmissions);
            std::cout << std::left << std::setw(15) << s->studentID
                      << std::setw(30) << s->name;
            if(ss) { std::cout << ss->obtainedMarks << " / " << selectedSub->totalMarks << "\n"; }
            else { std::cout << "N/A\n"; }
        }
        std::cout << std::string(55, '-') << "\n";
        
        std::cout << "\nEnter Student ID (Roll No) to edit (or '9' to finish): ";
        std::cin >> studentID;
        if (studentID == "9") break;

        // Find the specific student
        Student* studentToEdit = nullptr;
        for (auto* s : studentsInSection) {
            if (s->studentID == studentID) {
                studentToEdit = s;
                break;
            }
        }

        if (!studentToEdit) {
            std::cout << "Student not found in this section.\n";
            system("pause");
        } else {
            StudentSubmission* ssToEdit = findStudentSubmission(selectedSub->submissionID, studentToEdit->username, studentSubmissions);
            if (!ssToEdit) {
                std::cout << "Error: Student submission mark record not found!\n";
                system("pause");
                continue;
            }

            std::cout << "Editing for " << studentToEdit->name << " (Current: " << ssToEdit->obtainedMarks << ")\n";
            std::cout << "Enter new mark (out of " << selectedSub->totalMarks << "): ";
            double newMark;
            if (!(std::cin >> newMark)) {
                std::cout << "Invalid input.\n";
                std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else if (newMark < 0 || newMark > selectedSub->totalMarks) {
                std::cout << "Mark must be between 0 and " << selectedSub->totalMarks << ".\n";
            } else {
                ssToEdit->obtainedMarks = newMark; // Update the mark
                std::cout << "Mark updated.\n";
                
                // --- CRITICAL: Recalculate Final Grade ---
                // Because this mark counts towards the total course grade, we must update the student's final mark immediately.
                updateStudentFinalMark(studentToEdit->username, selectedSub->courseCode, 
                                       allStudents, allSubmissions, studentSubmissions);
                std::cout << "Final grade for " << selectedSub->courseCode << " recalculated.\n";
            }
            system("pause");
        }
        system("cls"); // Refresh screen to show new mark
    }
    
    std::cout << "Finished editing marks.\n";
    std::cout << "Remember to save data by exiting.\n";
    system("pause");
}

/*
    Marks attendance for "Today".
    1. Finds classes taught by the teacher.
    2. Asks teacher to pick a class.
    3. Lists students and asks for P/A.
*/
void teacher_MarkAttendance(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                            const std::vector<Student>& allStudents, std::vector<AttendanceRecord>& allAttendance,
                            const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Mark Attendance for Today (" << getCurrentDate() << ") ---\n\n";

    std::string todayDate = getCurrentDate(); 

    // Find classes teacher is responsible for
    std::map<std::pair<std::string, std::string>, bool> taughtCombos; 
    for (const auto& entry : allSchedule) {
        if (entry.teacherID == teacher.username) {
            taughtCombos[{entry.courseCode, entry.sectionID}] = true;
        }
    }

    if (taughtCombos.empty()) {
        std::cout << "You are not assigned to any classes in the schedule.\n";
        system("pause");
        return;
    }

    // Display options
    std::cout << "Select the class session to mark attendance for:\n";
    int classIndex = 1;
    std::vector<std::pair<std::string, std::string>> selectableClasses; 
    for (const auto& comboPair : taughtCombos) {
        const std::string& courseCode = comboPair.first.first;
        const std::string& sectionID = comboPair.first.second;

        std::cout << classIndex << ". " << sectionID
                  << " - " << getCourseName(courseCode, allCourses) 
                  << " (" << courseCode << ")\n";
        selectableClasses.push_back({courseCode, sectionID});
        classIndex++;
    }

    std::cout << "\nEnter choice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice < 0 || choice >= classIndex) { 
        std::cout << "Invalid choice.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        system("pause");
        return;
    }
    if (choice == 0) {
        std::cout << "Operation cancelled.\n";
        system("pause");
        return;
    }

    const std::string& selectedCourseCode = selectableClasses[choice - 1].first;
    const std::string& selectedSectionID = selectableClasses[choice - 1].second;

    // Find students in that section
    std::vector<const Student*> studentsInSection;
    for (const auto& student : allStudents) {
        if (student.sectionID == selectedSectionID) { 
            studentsInSection.push_back(&student);
        }
    }
    
    // Sort by Roll No
    std::sort(studentsInSection.begin(), studentsInSection.end(),
        [](const Student* a, const Student* b) {
            try { return std::stoi(a->studentID) < std::stoi(b->studentID); }
            catch (...) { return false; }
        });


    if (studentsInSection.empty()) {
        std::cout << "No students found in section " << selectedSectionID << ".\n"; 
        system("pause");
        return;
    }

    // Mark Attendance Loop
    std::cout << "\n--- Marking Attendance for " << selectedSectionID << " - " << selectedCourseCode << " on " << todayDate << " ---\n"; 
    std::cout << "Enter 'P' for Present, 'A' for Absent.\n\n";

    int presentCount = 0;
    int absentCount = 0;

    for (const Student* studentPtr : studentsInSection) {
        // Check if already marked today
        bool alreadyMarked = false;
        for (const auto& record : allAttendance) {
            if (record.studentUsername == studentPtr->username &&
                record.courseCode == selectedCourseCode && 
                record.date == todayDate) {
                alreadyMarked = true;
                break;
            }
        }

        if (!alreadyMarked) {
            char statusInput;
            std::cout << std::setw(15) << studentPtr->studentID
                      << std::setw(25) << studentPtr->name
                      << " (P/A): ";

            while (true) {
                std::cin >> statusInput;
                statusInput = toupper(statusInput);
                if (statusInput == 'P' || statusInput == 'A') {
                    break; 
                } else {
                    std::cout << "Invalid input. Please enter 'P' or 'A': ";
                    std::cin.clear(); 
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
                }
            }

            // Save record
            AttendanceRecord newRecord;
            newRecord.courseCode = selectedCourseCode; 
            newRecord.studentUsername = studentPtr->username;
            newRecord.sectionID = selectedSectionID; 
            newRecord.date = todayDate;
            newRecord.status = statusInput;
            allAttendance.push_back(newRecord);

            if (statusInput == 'P') presentCount++; else absentCount++;
        }
    }

    std::cout << "\n-----------------------------------\n";
    std::cout << "Attendance marked for " << selectedCourseCode << " in section " << selectedSectionID << ".\n"; 
    std::cout << "Present: " << presentCount << " | Absent: " << absentCount << "\n";
    std::cout << "Remember to save data by exiting the main menu.\n";
    system("pause");
}

/*
    Allows fixing past attendance.
    It groups attendance records by "Session" (Course + Section + Date).
    The teacher selects a session, and we list the students so they can toggle P/A.
*/
void teacher_ModifyAttendance(Teacher& teacher, std::vector<AttendanceRecord>& allAttendance,
                              const std::vector<Student>& allStudents, const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Modify Past Attendance ---\n\n";

    // 1. Find unique sessions relevant to teacher
    // Key: "Course|Section|Date", Value: vector of indices in allAttendance
    std::map<std::string, std::vector<size_t>> sessions;
    std::vector<std::tuple<std::string, std::string, std::string>> sessionDetails; 

    for (size_t i = 0; i < allAttendance.size(); ++i) {
        bool teacherTaught = false;
        for(const auto& assignedCode : teacher.assignedCourseCodes){
             if(assignedCode == allAttendance[i].courseCode){
                 teacherTaught = true;
                 break;
             }
        }

        if (teacherTaught) { 
            std::string key = allAttendance[i].courseCode + "|" + allAttendance[i].sectionID + "|" + allAttendance[i].date;
            
            if (sessions.find(key) == sessions.end()) {
                sessionDetails.emplace_back(allAttendance[i].courseCode, allAttendance[i].sectionID, allAttendance[i].date);
            }
            sessions[key].push_back(i); 
        }
    }


    if (sessions.empty()) {
        std::cout << "No past attendance records found for your courses/sections.\n";
        system("pause");
        return;
    }

    // 2. Select session
    std::cout << "Select attendance session to modify:\n";
    for (size_t i = 0; i < sessionDetails.size(); ++i) {
         std::cout << i + 1 << ". Course: " << std::get<0>(sessionDetails[i])
                   << ", Section: " << std::get<1>(sessionDetails[i])
                   << ", Date: " << std::get<2>(sessionDetails[i]) << "\n"; 
    }
    std::cout << "\nEnter choice (0 to cancel): ";
    int sessionChoice;
    if (!(std::cin >> sessionChoice) || sessionChoice < 0 || sessionChoice > sessionDetails.size()) {
         std::cout << "Invalid choice.\n"; std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); system("pause"); return;
    }
    if (sessionChoice == 0) { std::cout << "Cancelled.\n"; system("pause"); return; }

    // 3. Prepare session data
    const auto& selectedDetails = sessionDetails[sessionChoice - 1];
    std::string selectedKey = std::get<0>(selectedDetails) + "|" + std::get<1>(selectedDetails) + "|" + std::get<2>(selectedDetails);
    const auto& recordIndices = sessions[selectedKey];

    // 4. Display students in this session
    system("cls");
    std::cout << "--- Modifying Session ---\n";
    std::cout << "Course: " << std::get<0>(selectedDetails)
              << " | Section: " << std::get<1>(selectedDetails)
              << " | Date: " << std::get<2>(selectedDetails) << "\n\n";
    std::cout << "Students in this session:\n";
    std::cout << std::left << std::setw(15) << "Roll No."
              << std::setw(25) << "Name"
              << std::setw(20) << "Username"
              << "Current Status\n";
    std::cout << std::string(75, '-') << "\n";

    std::map<std::string, size_t> studentIdToIndexMap;

    for (size_t originalIndex : recordIndices) {
        std::string studentUsername = allAttendance[originalIndex].studentUsername;
        std::string studentName = "Unknown";
        std::string studentID = "N/A";

        for(const auto& s : allStudents){
            if(s.username == studentUsername){
                studentName = s.name;
                studentID = s.studentID; 
                break;
            }
        }

         std::cout << std::left << std::setw(15) << studentID
                   << std::setw(25) << studentName
                   << std::setw(20) << studentUsername
                   << (allAttendance[originalIndex].status == 'P' ? "Present" : "Absent") << "\n";

        if (studentID != "N/A") {
            studentIdToIndexMap[studentID] = originalIndex;
        }
    }

    std::cout << std::string(75, '-') << "\n\n";

    // 5. Modification Loop
    std::string studentIdInput;
    while (true) {
        std::cout << "Enter Student ID (Roll No) to modify status (or type '9' to finish): ";
        std::cin >> studentIdInput;

        if (studentIdInput == "9") {
            break;
        }

        if (studentIdToIndexMap.count(studentIdInput)) {
            size_t indexToModify = studentIdToIndexMap[studentIdInput];
            char currentStatus = allAttendance[indexToModify].status;
            char newStatus = (currentStatus == 'P' ? 'A' : 'P'); // Toggle

            std::string studentNameToDisplay = "Unknown Student"; 
            for(const auto& s : allStudents){
                if(s.username == allAttendance[indexToModify].studentUsername){
                    studentNameToDisplay = s.name; 
                    break;
                }
            }
            std::string currentStatusWord = (currentStatus == 'P' ? "Present" : "Absent");
            std::string newStatusWord = (newStatus == 'P' ? "Present" : "Absent");

            std::cout << "\nChanging status for " << studentNameToDisplay
                      << " role no " << studentIdInput
                      << " from " << currentStatusWord
                      << " to " << newStatusWord << ". Confirm (Y/N): ";
                      
            char confirm;
            std::cin >> confirm;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');


            if (toupper(confirm) == 'Y') {
                allAttendance[indexToModify].status = newStatus;
                std::cout << "      Status updated.\n\n";
            } else {
                std::cout << "      Modification cancelled.\n\n";
            }
        } else {
            std::cout << "  Error: Student ID '" << studentIdInput << "' not found in this attendance session's records.\n\n";
            if (std::cin.fail()) {
                 std::cin.clear();
                 std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
    } 

    std::cout << "\nFinished modifying this session.\n";
    std::cout << "Remember to save data by exiting the main program.\n";
    system("pause");
}

// --- MARKS SUB-MENU FUNCTIONS ---

/*
    Creates a new submission (like a Quiz or Assignment).
    1. Teacher picks a course/section.
    2. Teacher types details (Title, Total Marks).
    3. We create empty (0) marks for every student in that section so they are initialized.
*/
void teacher_AddSubmission(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                           const std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                           std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Add New Submission ---\n\n";

    // 1. Find classes teacher teaches
    std::map<std::pair<std::string, std::string>, bool> taughtCombos;
    for (const auto& entry : allSchedule) {
        if (entry.teacherID == teacher.username) {
            taughtCombos[{entry.courseCode, entry.sectionID}] = true;
        }
    }
    if (taughtCombos.empty()) {
        std::cout << "You are not assigned to any classes in the schedule.\n";
        system("pause"); return;
    }

    // 2. Select Class
    std::cout << "Select the class to create a submission for:\n";
    int classIndex = 1;
    std::vector<std::pair<std::string, std::string>> selectableClasses;
    for (const auto& comboPair : taughtCombos) {
        const std::string& courseCode = comboPair.first.first;
        const std::string& sectionID = comboPair.first.second;
        std::cout << classIndex << ". " << sectionID << " - " << getCourseName(courseCode, allCourses) << " (" << courseCode << ")\n";
        selectableClasses.push_back({courseCode, sectionID});
        classIndex++;
    }
    std::cout << "\nEnter choice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice <= 0 || choice >= classIndex) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Cancelled or invalid input.\n"; system("pause"); return;
    }

    std::string selectedCourse = selectableClasses[choice - 1].first;
    std::string selectedSection = selectableClasses[choice - 1].second;

    // 3. Enter details
    system("cls");
    std::cout << "--- Creating New Submission for " << selectedSection << " - " << selectedCourse << " ---\n";
    
    std::string typeStr;
    int typeChoice;
    std::cout << "Select Type:\n 1. Assignment\n 2. Quiz\n 3. Midterm Exam\n 4. Final Exam\nChoice: ";
    if (!(std::cin >> typeChoice) || typeChoice < 1 || typeChoice > 4) {
         std::cout << "Invalid type.\n"; system("pause"); return;
    }
    if (typeChoice == 1) typeStr = "Assignment";
    else if (typeChoice == 2) typeStr = "Quiz";
    else if (typeChoice == 3) typeStr = "Midterm"; 
    else typeStr = "Final"; 

    std::string title;
    std::cout << "Enter Title (e.g., A1_Chapter1 or Midterm, use_underscores): ";
    std::cin >> title;

    int totalMarks;
    std::cout << "Enter Total Marks (e.g., 25): ";
    if (!(std::cin >> totalMarks) || totalMarks <= 0) {
        std::cout << "Invalid total marks.\n"; system("pause"); return;
    }

    // 4. Create definition
    SubmissionDefinition newSub;
    newSub.courseCode = selectedCourse;
    newSub.sectionID = selectedSection;
    newSub.type = typeStr;
    newSub.title = title;
    newSub.totalMarks = totalMarks;
    newSub.submissionID = selectedCourse + "_" + selectedSection + "_" + title; // Unique ID
    
    for(const auto& sub : allSubmissions) {
        if(sub.submissionID == newSub.submissionID) {
            std::cout << "Error: A submission with this exact course, section, and title already exists.\n";
            system("pause"); return;
        }
    }
    
    allSubmissions.push_back(newSub);

    // 5. Create blank marks for students
    int studentsAdded = 0;
    for (const auto& student : allStudents) {
        if (student.sectionID == selectedSection) {
            StudentSubmission newStudentMark;
            newStudentMark.submissionID = newSub.submissionID;
            newStudentMark.studentUsername = student.username;
            newStudentMark.obtainedMarks = 0; 
            studentSubmissions.push_back(newStudentMark);
            studentsAdded++;
        }
    }

    std::cout << "\nSubmission '" << title << "' created successfully.\n";
    std::cout << "Blank mark entries created for " << studentsAdded << " students in section " << selectedSection << ".\n";
    std::cout << "You can now enter marks in the 'Manage Student Marks' menu.\n";
    std::cout << "Remember to save data by exiting.\n";
    system("pause");
}

/*
    The sub-menu just for grading options.
*/
void teacher_MarksMenu(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                       std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                       std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses) {
    int choice;
    do {
        system("cls");
        std::cout << "--- Manage Marks & Submissions ---\n\n";
        std::cout << "1. Add New Submission (Assignment, Quiz, etc.)\n";
        std::cout << "2. Delete Submission\n";
        std::cout << "3. Modify Submission (e.g., Total Marks)\n";
        std::cout << "4. Enter/Edit Student Marks\n";
        std::cout << "9. Back to Teacher Dashboard\n\n";
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice)) {
             std::cout << "Invalid input. Please enter a number.\n";
             std::cin.clear();
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             system("pause");
             choice = 0;
             continue;
        }
        
        switch(choice) {
            case 1:
                teacher_AddSubmission(teacher, allSchedule, allStudents, allSubmissions, studentSubmissions, allCourses);
                break;
            case 2:
                teacher_DeleteSubmission(teacher, allSchedule, allStudents, allSubmissions, studentSubmissions, allCourses);
                break;
            case 3:
                teacher_ModifySubmission(teacher, allSchedule, allStudents, allSubmissions, studentSubmissions, allCourses);
                break;
            case 4:
                teacher_ManageStudentMarks(teacher, allSchedule, allStudents, allSubmissions, studentSubmissions);
                break;
            case 9:
                break; 
            default:
                std::cout << "Invalid choice.\n";
                system("pause");
                break;
        }

    } while (choice != 9);
}

/*
    Deletes a submission entirely.
    WARNING: This uses the "Erase-Remove Idiom". It's a C++ pattern where we swap the items
    we want to delete to the end of the vector, and then chop off the end of the vector.
    
    After deleting the submission, we must recalculate student grades because they just lost an assignment.
*/

void teacher_DeleteSubmission(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                              std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                              std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Delete Submission ---\n\n";

    // 1. Find submissions for this teacher
    std::vector<const SubmissionDefinition*> mySubmissions;
    std::set<std::string> mySections;
    for(const auto& entry : allSchedule) {
        if(entry.teacherID == teacher.username) mySections.insert(entry.sectionID);
    }
    for(const auto& sub : allSubmissions) {
        if(mySections.count(sub.sectionID)) {
            for(const auto& courseCode : teacher.assignedCourseCodes) {
                if(sub.courseCode == courseCode) {
                    mySubmissions.push_back(&sub);
                    break;
                }
            }
        }
    }
    if (mySubmissions.empty()) {
        std::cout << "No submissions found to delete.\n";
        system("pause"); return;
    }

    // 2. Select submission to delete
    std::cout << "Select a submission to DELETE:\n";
    for(size_t i = 0; i < mySubmissions.size(); ++i) {
        std::cout << i + 1 << ". [" << mySubmissions[i]->sectionID << " " << mySubmissions[i]->courseCode
                  << "] " << mySubmissions[i]->title << "\n";
    }
    std::cout << "\nEnter choice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice <= 0 || choice > mySubmissions.size()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Cancelled or invalid input.\n"; system("pause"); return;
    }

    const SubmissionDefinition* subToDelete = mySubmissions[choice - 1];
    std::string subID = subToDelete->submissionID;
    std::string courseCode = subToDelete->courseCode;
    std::string sectionID = subToDelete->sectionID;

    // 3. Warning
    std::cout << "\nWARNING: You are about to delete '" << subToDelete->title << "'.\n";
    std::cout << "This will remove the submission AND all associated student marks.\n";
    std::cout << "This CANNOT be undone. Type 'DELETE' to confirm: ";
    std::string confirm;
    std::cin >> confirm;

    if (confirm != "DELETE") {
        std::cout << "Deletion cancelled.\n";
        system("pause"); return;
    }

    // 4. Perform deletion (Erase-Remove Idiom)
    allSubmissions.erase(
        std::remove_if(allSubmissions.begin(), allSubmissions.end(), 
            [subID](const SubmissionDefinition& s) { return s.submissionID == subID; }),
        allSubmissions.end()
    );

    // Delete student marks associated with it
    studentSubmissions.erase(
        std::remove_if(studentSubmissions.begin(), studentSubmissions.end(),
            [subID](const StudentSubmission& ss) { return ss.submissionID == subID; }),
        studentSubmissions.end()
    );

    // 5. Recalculate grades
    std::cout << "\nSubmission deleted. Recalculating final marks for section " << sectionID << "...\n";
    for (auto& student : allStudents) {
        if (student.sectionID == sectionID) {
            updateStudentFinalMark(student.username, courseCode, allStudents, allSubmissions, studentSubmissions);
        }
    }

    std::cout << "Grades recalculated.\n";
    std::cout << "Remember to save data by exiting.\n";
    system("pause");
}

/*
    Modifies an assignment (e.g., changing total marks from 10 to 20).
    If the total marks change, we scale existing student marks so they keep the same percentage.
*/

void teacher_ModifySubmission(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                              std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                              std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Modify Submission ---\n\n";

    // 1. Find submissions
    std::vector<SubmissionDefinition*> mySubmissions; 
    std::set<std::string> mySections;
    for(const auto& entry : allSchedule) {
        if(entry.teacherID == teacher.username) mySections.insert(entry.sectionID);
    }
    for(auto& sub : allSubmissions) { 
        if(mySections.count(sub.sectionID)) {
            for(const auto& courseCode : teacher.assignedCourseCodes) {
                if(sub.courseCode == courseCode) {
                    mySubmissions.push_back(&sub); 
                    break;
                }
            }
        }
    }
    if (mySubmissions.empty()) {
        std::cout << "No submissions found to modify.\n";
        system("pause"); return;
    }

    // 2. Select submission
    std::cout << "Select a submission to MODIFY:\n";
    for(size_t i = 0; i < mySubmissions.size(); ++i) {
        std::cout << i + 1 << ". [" << mySubmissions[i]->sectionID << " " << mySubmissions[i]->courseCode
                  << "] " << mySubmissions[i]->title << " (Total: " << mySubmissions[i]->totalMarks << ")\n";
    }
    std::cout << "\nEnter choice (0 to cancel): ";
    int choice;
    if (!(std::cin >> choice) || choice <= 0 || choice > mySubmissions.size()) {
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Cancelled or invalid input.\n"; system("pause"); return;
    }

    SubmissionDefinition* subToModify = mySubmissions[choice - 1];

    // 3. Get new total
    std::cout << "\nModifying: " << subToModify->title << "\n";
    std::cout << "Current Total Marks: " << subToModify->totalMarks << "\n";
    std::cout << "Enter NEW Total Marks: ";
    int newTotalMarks;
    if (!(std::cin >> newTotalMarks) || newTotalMarks <= 0) {
        std::cout << "Invalid marks. Operation cancelled.\n";
        std::cin.clear(); std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        system("pause"); return;
    }

    double oldTotalMarks = subToModify->totalMarks;
    
    if (oldTotalMarks == newTotalMarks) {
        std::cout << "No change made.\n";
        system("pause"); return;
    }
    
    // 4. Scaling warning
    std::cout << "\nThis will scale all existing student marks for this submission.\n";
    std::cout << "e.g., a " << oldTotalMarks/2 << "/" << oldTotalMarks << " will become a " << newTotalMarks/2 << "/" << newTotalMarks << ".\n";
    std::cout << "Confirm (Y/N): ";
    char confirm;
    std::cin >> confirm;

    if (toupper(confirm) != 'Y') {
        std::cout << "Operation cancelled.\n";
        system("pause"); return;
    }

    // 5. Perform scaling
    double scaleFactor = static_cast<double>(newTotalMarks) / oldTotalMarks;

    for (auto& ss : studentSubmissions) {
        if (ss.submissionID == subToModify->submissionID) {
            ss.obtainedMarks = ss.obtainedMarks * scaleFactor;
        }
    }

    // 6. Update definition
    subToModify->totalMarks = newTotalMarks;
    
    // 7. Recalculate grades
    std::cout << "\nMarks scaled. Recalculating final grades for section " << subToModify->sectionID << "...\n";
    for (auto& student : allStudents) {
        if (student.sectionID == subToModify->sectionID) {
            updateStudentFinalMark(student.username, subToModify->courseCode, allStudents, allSubmissions, studentSubmissions);
        }
    }
    
    std::cout << "Grades recalculated.\n";
    std::cout << "Remember to save data by exiting.\n";
    system("pause");
}

// --- ATTENDANCE SUB-MENU FUNCTIONS ---

/*
    The sub-menu for Attendance.
*/
void teacher_AttendanceMenu(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                            const std::vector<Student>& allStudents, std::vector<AttendanceRecord>& allAttendance,
                            const std::vector<Course>& allCourses) {
    int choice;
    do {
        system("cls");
        std::cout << "--- Manage Attendance ---\n\n";
        std::cout << "1. Mark Attendance for Today\n";
        std::cout << "2. Modify Past Attendance\n";
        std::cout << "9. Back to Teacher Dashboard\n\n";
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice)) {
             std::cout << "Invalid input. Please enter a number.\n";
             std::cin.clear();
             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
             system("pause");
             choice = 0;
             continue;
        }

        switch(choice) {
            case 1:
                teacher_MarkAttendance(teacher, allSchedule, allStudents, allAttendance, allCourses);
                break;
            case 2:
                teacher_ModifyAttendance(teacher, allAttendance, allStudents, allCourses);
                break;
            case 9:
                break;
            default:
                std::cout << "Invalid choice.\n";
                system("pause");
                break;
        }
    } while (choice != 9);
}
