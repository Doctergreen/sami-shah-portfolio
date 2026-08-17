
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
 --- FILE: student.h ---
============================================================================

 Summary:
 This header defines the Student structure, which is the core data entity
 for learners in the system.

 It also declares the functions for the Student Dashboard, allowing
 students to check their own grades, attendance history, and schedule.

============================================================================
*/

#pragma once
#include <string>
#include <vector>
#include "course.h"      // Needed because a Student holds a vector of 'Grade' objects
#include "attendance.h"  // Needed to reference AttendanceRecord in functions

// --- Forward Declarations ---
// We declare these classes here so we can use them in function arguments
// without creating circular dependency headers.
class ScheduleEntry;
class Teacher;
struct SubmissionDefinition;
struct StudentSubmission;

/*
    Represents a student user.
    Contains personal info, login credentials, and their academic record.
*/
class Student {
public:
    std::string username;   // Unique login ID (e.g., "FA25-BCS-109")
    std::string studentID;  // The Roll Number (e.g., "109")
    std::string name;       // Full Name
    std::string password;   // Login password
    std::string sectionID;  // Class section (e.g., "BCS-2A")
    
    // A list of final grades for every course this student is enrolled in.
    // This is updated dynamically based on assignment/quiz scores.
    std::vector<Grade> grades; 
};

// --- Student Dashboard Functions ---

/*
    The main menu for the Student. 
    It passes in read-only references to all system data so the student can view
    their info but cannot modify anything (except their own password).
*/
void showStudentDashboard(Student& loggedInStudent, const std::vector<ScheduleEntry>& allSchedule,
                          const std::vector<Course>& allCourses, const std::vector<Teacher>& allTeachers,
                          const std::vector<AttendanceRecord>& allAttendance,
                          const std::vector<SubmissionDefinition>& allSubmissions,
                          const std::vector<StudentSubmission>& studentSubmissions);

/*
    Shows a complete history of attendance dates (Present/Absent) for all courses.
*/
void student_ViewAttendanceHistory(const Student& student, const std::vector<AttendanceRecord>& allAttendance, 
                                   const std::vector<Course>& allCourses);

/*
    Shows a detailed breakdown of a specific course grade.
    It lists every assignment, quiz, and exam score that contributes to the final mark.
*/
void student_ViewDetailedMarks(const Student& student, const std::string& courseCode, const std::vector<Course>& allCourses,
                               const std::vector<SubmissionDefinition>& allSubmissions,
                               const std::vector<StudentSubmission>& studentSubmissions);
