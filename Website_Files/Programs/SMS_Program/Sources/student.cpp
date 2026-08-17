
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
 --- FILE: student.cpp ---
============================================================================

 Summary:
 This file implements all functions related to the Student user role.
 It handles the main student dashboard, viewing grades, viewing
 attendance, viewing the class schedule, and changing passwords.

============================================================================
*/

// --- Standard Library Includes ---
#include <iostream>     // For console input/output (std::cout, std::cin)
#include <iomanip>      // For stream manipulators (setw, setprecision, left)
#include <limits>       // For std::numeric_limits (used in input validation)
#include <vector>       // For std::vector
#include <string>       // For std::string
#include <map>          // For std::map (used in attendance history)
#include <algorithm>    // For std::find (not used here, but good practice)
#include <sstream>      // For std::stringstream (used for formatting marks)

// --- Project Header Includes ---
#include "../headers/student.h"
#include "../headers/schedule.h"
#include "../headers/course.h"      // Contains Submission definitions
#include "../headers/teacher.h"
#include "../headers/utility.h"      // For utility functions (getCourseName, etc.)
#include "../headers/attendance.h"   // For AttendanceRecord


// --- Function Prototypes (for functions defined in this file) ---
void student_ViewMyMarks(const Student& student, const std::vector<Course>& allCourses,
                          const std::vector<AttendanceRecord>& allAttendance,
                          const std::vector<SubmissionDefinition>& allSubmissions,
                          const std::vector<StudentSubmission>& studentSubmissions);
                          
void student_ViewMySchedule(const Student& student, const std::vector<ScheduleEntry>& allSchedule,
                            const std::vector<Course>& allCourses, const std::vector<Teacher>& allTeachers);

void student_ViewAttendanceHistory(const Student& student, const std::vector<AttendanceRecord>& allAttendance, 
                                   const std::vector<Course>& allCourses);
                                   
void student_ViewDetailedMarks(const Student& student, const std::string& courseCode, const std::vector<Course>& allCourses,
                               const std::vector<SubmissionDefinition>& allSubmissions,
                               const std::vector<StudentSubmission>& studentSubmissions);

// Helper function to reduce repetition in student_ViewDetailedMarks
void printMarkCategory(const std::string& title,
                        const std::vector<const SubmissionDefinition*>& submissions,
                        const Student& student,
                        const std::vector<StudentSubmission>& studentSubmissions);


/*
    This is the main menu loop for the Student.
*/
void showStudentDashboard(Student& loggedInStudent, const std::vector<ScheduleEntry>& allSchedule,
                          const std::vector<Course>& allCourses, const std::vector<Teacher>& allTeachers,
                          const std::vector<AttendanceRecord>& allAttendance,
                          const std::vector<SubmissionDefinition>& allSubmissions,
                          const std::vector<StudentSubmission>& studentSubmissions) {
    // --- Variable Declarations ---
    int choice;
    double cgpa = 0.0;
    
    Student tempStudent; 
    std::vector<Student> tempStudentList;

    // Loop until the user chooses to logout (option 9)
    do {
        system("cls");

        // --- Header Calculation ---
        tempStudent = loggedInStudent; 
        tempStudentList.clear(); 
        tempStudentList.push_back(tempStudent);

        // Iterate through every course to calculate the latest weighted marks
        for (size_t i = 0; i < tempStudent.grades.size(); ++i) {
             updateStudentFinalMark(tempStudent.username, tempStudent.grades[i].courseCode,
                                    tempStudentList, 
                                    allSubmissions, studentSubmissions);

             tempStudent.grades[i].mark = tempStudentList[0].grades[i].mark;
        }
        
        cgpa = calculateCGPA(tempStudent.grades); 
        // --- End Header Calculation ---

        // --- Display Dashboard Header ---
        std::cout << "COMSATS University Islamabad, Wah Campus\n";
        std::cout << "Date: " << getCurrentDate() << "\n";
        std::cout << "------------------------------------------------------------------\n";
        std::cout << "Student: " << loggedInStudent.name << " | Roll No: " << loggedInStudent.studentID
                  << " | Section: " << loggedInStudent.sectionID << " | CGPA: "
                  << std::fixed << std::setprecision(2) << cgpa << std::setprecision(0) << "\n";
        std::cout << "==================================================================\n\n";
        
        // --- Display Menu Options ---
        std::cout << "1. View My Gradebook\n";
        std::cout << "2. View My Weekly Schedule\n";
        std::cout << "3. Change My Password\n";
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
                student_ViewMyMarks(loggedInStudent, allCourses, allAttendance, allSubmissions, studentSubmissions);
                break;
            case 2:
                student_ViewMySchedule(loggedInStudent, allSchedule, allCourses, allTeachers);
                break;
            case 3:
                changePassword(loggedInStudent);
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


/*
    Displays the weekly schedule.
*/
void student_ViewMySchedule(const Student& student, const std::vector<ScheduleEntry>& allSchedule,
                            const std::vector<Course>& allCourses, const std::vector<Teacher>& allTeachers) {
    std::string title = "My Weekly Schedule (" + student.sectionID + ")";
    
    std::vector<ScheduleEntry> mySectionSchedule;

    system("cls");
    std::cout << "--- " << title << " ---\n\n";

    // Filter the master schedule
    for (const auto& entry : allSchedule) {
        if (entry.sectionID == student.sectionID) {
            mySectionSchedule.push_back(entry);
        }
    }

    displayScheduleGrid(mySectionSchedule, allCourses, allTeachers, title);

    std::cout << "\n";
    system("pause");
}

/*
    This shows the "Report Card" view.
*/
void student_ViewMyMarks(const Student& student, const std::vector<Course>& allCourses,
                         const std::vector<AttendanceRecord>& allAttendance,
                         const std::vector<SubmissionDefinition>& allSubmissions,
                         const std::vector<StudentSubmission>& studentSubmissions) {
                             
    Student tempStudent = student; 
    std::vector<Student> tempStudentList = {tempStudent}; 
    
    size_t maxCourseNameLength = 11; 
    size_t courseNameWidth = 0;
    
    int historyChoice = 0;
    std::string courseCode;
    bool foundCourse = false;

    // 1. Refresh marks before displaying
    for (auto& grade : tempStudent.grades) {
         updateStudentFinalMark(tempStudent.username, grade.courseCode,
                                tempStudentList, allSubmissions, studentSubmissions);
         
         size_t index = &grade - &tempStudent.grades[0];
         grade.mark = tempStudentList[0].grades.at(index).mark;
    }

    // --- Main Gradebook Display ---
    system("cls");
    std::cout << "--- My Gradebook (Final Marks) ---\n\n";

    for (const auto& grade : tempStudent.grades) {
        std::string courseName = getCourseName(grade.courseCode, allCourses);
        if (courseName.length() > maxCourseNameLength) {
            maxCourseNameLength = courseName.length();
        }
    }
    courseNameWidth = maxCourseNameLength + 2;

    std::cout << std::left
              << std::setw(15) << "Course Code"
              << std::setw(courseNameWidth) << "Course Name"
              << std::setw(10) << "Mark (%)"
              << std::setw(15) << "Attendance (%)"
              << "\n";
    std::cout << std::string(15 + courseNameWidth + 10 + 15, '-') << "\n";

    if (tempStudent.grades.empty()) {
        std::cout << "You are not enrolled in any courses.\n";
    } else {
        for (const auto& grade : tempStudent.grades) {
            
            double attendancePercent = calculateStudentAttendancePercentage(student.username, grade.courseCode, allAttendance);
            
            std::cout << std::left
                      << std::setw(15) << grade.courseCode
                      << std::setw(courseNameWidth) << getCourseName(grade.courseCode, allCourses)
                      << std::setw(10) << grade.mark 
                      << std::fixed << std::setprecision(1)
                      << std::setw(15);
                      
             if (attendancePercent < 0) std::cout << "N/A"; else std::cout << attendancePercent;
             
             std::cout << std::setprecision(0) << "\n"; 
        }
    }
    std::cout << "\n";

    std::cout << "-----------------------------------\n";
    std::cout << "1. View Detailed Marks for a Course\n";
    std::cout << "2. View Attendance History\n";
    std::cout << "9. Back to Dashboard\n";
    std::cout << "Enter choice: ";

    if (!(std::cin >> historyChoice)) {
        std::cout << "Invalid input. Returning to dashboard.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        system("pause");
        return;
    }

    if (historyChoice == 1) {
        std::cout << "\nEnter Course Code (e.g., CSC103): ";
        std::cin >> courseCode;

        foundCourse = false;
        for(const auto& g : student.grades) { 
            if (g.courseCode == courseCode) { 
                foundCourse = true; 
                break; 
            } 
        }

        if (foundCourse) {
            student_ViewDetailedMarks(student, courseCode, allCourses, allSubmissions, studentSubmissions);
        } else {
            std::cout << "You are not enrolled in that course.\n";
        }
        system("pause");

    } else if (historyChoice == 2) {
        student_ViewAttendanceHistory(student, allAttendance, allCourses);
        system("pause");
    } else {
         return; 
    }
}


/*
    Prints the raw attendance log. 
*/
void student_ViewAttendanceHistory(const Student& student, const std::vector<AttendanceRecord>& allAttendance, const std::vector<Course>& allCourses) {
    std::map<std::string, std::vector<std::pair<std::string, char>>> groupedAttendance;

    system("cls");
    std::cout << "--- Attendance History for " << student.name << " ---\n\n";

    for (const auto& record : allAttendance) {
        if (record.studentUsername == student.username) {
            groupedAttendance[record.courseCode].push_back({record.date, record.status});
        }
    }

    if (groupedAttendance.empty()) {
        std::cout << "No attendance records found.\n";
    } else {
        for (const auto& pair : groupedAttendance) {
            std::string courseCode = pair.first;
            const auto& records = pair.second;
            
            std::cout << "--- Course: " << courseCode << " (" << getCourseName(courseCode, allCourses) << ") ---\n";
            std::cout << std::left << std::setw(20) << "Date" << std::setw(10) << "Status" << "\n";
            std::cout << std::string(30, '-') << "\n";
            
            for (const auto& recPair : records) {
                std::cout << std::left << std::setw(20) << recPair.first
                          << std::setw(10) << (recPair.second == 'P' ? "Present" : "Absent") << "\n";
            }
            std::cout << "\n"; 
        }
    }
}


/*
    Sorts submissions and prints details.
*/
void student_ViewDetailedMarks(const Student& student, const std::string& courseCode, const std::vector<Course>& allCourses,
                               const std::vector<SubmissionDefinition>& allSubmissions,
                               const std::vector<StudentSubmission>& studentSubmissions) {
    
    std::vector<const SubmissionDefinition*> assignments;
    std::vector<const SubmissionDefinition*> quizzes;
    std::vector<const SubmissionDefinition*> midterms;
    std::vector<const SubmissionDefinition*> finals;

    system("cls");
    std::cout << "--- Detailed Marks for " << courseCode << " (" << getCourseName(courseCode, allCourses) << ") ---\n\n";

    for (const auto& sub : allSubmissions) {
        if (sub.courseCode == courseCode && sub.sectionID == student.sectionID) {
            if (sub.type == "Assignment") assignments.push_back(&sub);
            else if (sub.type == "Quiz") quizzes.push_back(&sub);
            else if (sub.type == "Midterm") midterms.push_back(&sub);
            else if (sub.type == "Final") finals.push_back(&sub);
            else if (sub.type == "Exam") midterms.push_back(&sub); 
        }
    }

    printMarkCategory("Assignments", assignments, student, studentSubmissions);
    printMarkCategory("Quizzes", quizzes, student, studentSubmissions);
    printMarkCategory("Midterm Exams", midterms, student, studentSubmissions);
    printMarkCategory("Final Exams", finals, student, studentSubmissions);
    
    std::cout << "\n";
}


/*
    Helper function to print a standardized table.
*/
void printMarkCategory(const std::string& title,
                        const std::vector<const SubmissionDefinition*>& submissions,
                        const Student& student,
                        const std::vector<StudentSubmission>& studentSubmissions) {
                            
    const StudentSubmission* ss = nullptr; 
    std::stringstream ss_mark; 

    std::cout << "--- " << title << " ---\n";
    if (submissions.empty()) {
        std::cout << "No " << title << " marked.\n\n";
    } else {
        std::cout << std::left << std::setw(25) << "Title" << std::setw(15) << "Mark" << "\n";
        std::cout << std::string(40, '-') << "\n";
        
        for (const auto* sub : submissions) {
            ss = findStudentSubmission(sub->submissionID, student.username, studentSubmissions);
            
            std::cout << std::left << std::setw(25) << sub->title;
            if (ss) {
                ss_mark.str(""); 
                ss_mark.clear(); 
                ss_mark << std::fixed << std::setprecision(1) << ss->obtainedMarks;
                
                std::cout << std::setw(15) << ss_mark.str() + " / " + std::to_string(sub->totalMarks) << "\n";
            } else {
                std::cout << std::setw(15) << "N/A" << "\n";
            }
        }
        std::cout << "\n"; 
    }
}
