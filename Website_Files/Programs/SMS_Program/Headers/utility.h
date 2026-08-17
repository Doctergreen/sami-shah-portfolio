
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
 --- FILE: utility.h ---
============================================================================

 Summary:
 This header file declares a "toolkit" of general-purpose functions.

 Instead of cluttering up the main Student/Teacher classes with math logic,
 we keep all the calculation heavy-lifting (CGPA, Weighted Averages, Attendance %)
 and generic tools (Password Change, Date) in here.

============================================================================
*/

#pragma once
#include <string>
#include <vector>
#include <ctime>   // For date/time
#include <iomanip> // For setprecision
#include <iostream>
#include <limits>
#include "../headers/course.h" // For Grade class

// Forward declarations prevent circular dependency errors.
// We promise the compiler these structs/classes exist elsewhere.
struct AttendanceRecord; 
struct SubmissionDefinition;
struct StudentSubmission;
class Student; 

// --- Attendance Calculations ---

// Calculates the percentage (Present / Total) for one specific student in one course.
double calculateStudentAttendancePercentage(const std::string& studentUsername, const std::string& courseCode,
                                            const std::vector<AttendanceRecord>& allAttendance);


// Calculates the average attendance for a whole class section.
// Useful for teachers to see if a specific class has low attendance overall.
double calculateClassAttendancePercentage(const std::string& sectionID, const std::string& courseCode,
                                          const std::vector<Student>& allStudents,
                                          const std::vector<AttendanceRecord>& allAttendance);


// --- Date/Time Function ---
// Returns a string like "November 19, 2025".
std::string getCurrentDate();

// --- Grading & CGPA Calculations ---

// Converts a raw mark (0-100) into a GPA point (0.0 - 4.0).
double getGradePoint(int mark);

// Averages the GPA points of all courses in the vector.
double calculateCGPA(const std::vector<Grade>& grades);


// The heavy lifter: It looks at all assignments, quizzes, and exams a student has done,
// applies the weighting percentages (e.g., Finals are 50%), and updates their final course grade.
void updateStudentFinalMark(const std::string& studentUsername, const std::string& courseCode,
                            std::vector<Student>& allStudents,
                            const std::vector<SubmissionDefinition>& allSubmissions,
                            const std::vector<StudentSubmission>& studentSubmissions);

// --- Search Helpers ---


// Finds a specific submission record.
// Non-const version: Use this when you want to EDIT the mark.
StudentSubmission* findStudentSubmission(const std::string& subID, const std::string& username,
                                         std::vector<StudentSubmission>& studentSubmissions);

// Const version: Use this when you just want to READ the mark (safe/read-only).
const StudentSubmission* findStudentSubmission(const std::string& subID, const std::string& username,
                                               const std::vector<StudentSubmission>& studentSubmissions);
                                               
// --- Password Change Function ---

// This is a "Template Function". It's a special C++ feature that allows this
// single function to work for Students, Teachers, AND Admins.
// As long as the object passed in (`user`) has a `.password` field, this code works.
template <typename T>
void changePassword(T& user) {
    system("cls");
    std::cout << "--- Change Password ---\n\n";
    std::string currentPass, newPass1, newPass2;
    
    // 1. Verify identity
    std::cout << "Enter CURRENT password: ";
    std::cin >> currentPass;

    if (currentPass != user.password) {
        std::cout << "\nError: Incorrect current password.\n";
        system("pause");
        return;
    }

    // 2. Get new password twice
    std::cout << "Enter NEW password: ";
    std::cin >> newPass1;
    std::cout << "Confirm NEW password: ";
    std::cin >> newPass2;

    // 3. Validate and Update
    if (newPass1 != newPass2) {
        std::cout << "\nError: New passwords do not match.\n";
    } else if (newPass1 == user.password) {
        std::cout << "\nError: New password cannot be the same as the old password.\n";
    } else {
        user.password = newPass1;
        std::cout << "\nPassword changed successfully!\n";
    }
    system("pause");
}
