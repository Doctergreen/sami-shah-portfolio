
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
 --- FILE: auth.h ---
============================================================================

 Summary:
 This header defines the entry points for user authentication.

 It declares the functions responsible for:
 1. Prompting users for credentials (Username/Password).
 2. Validating those credentials against the loaded data.
 3. Launching the appropriate Dashboard (Student, Teacher, or Admin).

============================================================================
*/

#pragma once
#include <string>
#include <vector>
#include "attendance.h" 

// --- Forward Declarations ---
// We tell the compiler these classes exist so we can use them in function 
// arguments here without including every single header file (which would cause circular dependency errors).
class Student;
class Teacher;
class Admin;
class Course;
class ScheduleEntry;
struct SubmissionDefinition;
struct StudentSubmission;

// --- Function Prototypes ---

/*
    Handles the Student login process.
    It checks the 'allStudents' list for a match and, if successful, 
    passes all necessary read-only data to the Student Dashboard.
*/
void loginAsStudent(std::vector<Student>& allStudents, const std::vector<ScheduleEntry>& allSchedule,
                    const std::vector<Course>& allCourses, const std::vector<Teacher>& allTeachers,
                    const std::vector<AttendanceRecord>& allAttendance,
                    const std::vector<SubmissionDefinition>& allSubmissions,
                    const std::vector<StudentSubmission>& studentSubmissions);

/*
    Handles the Teacher login process.
    If authenticated, it passes editable references (for Grades, Attendance, Submissions)
    to the Teacher Dashboard so they can manage their classes.
*/
void loginAsTeacher(std::vector<Teacher>& allTeachers, std::vector<Student>& allStudents,
                    const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                    std::vector<AttendanceRecord>& allAttendance,
                    std::vector<SubmissionDefinition>& allSubmissions,
                    std::vector<StudentSubmission>& studentSubmissions);
                    
/*
    Handles the Admin login process.
    Admins need access to almost everything in the system to add users, 
    fix schedules, and manage courses.
*/
void loginAsAdmin(std::vector<Admin>& allAdmins, std::vector<Student>& allStudents,
                  std::vector<Teacher>& allTeachers, std::vector<Course>& allCourses,
                  std::vector<ScheduleEntry>& allSchedule,
                  std::vector<AttendanceRecord>& allAttendance,
                  std::vector<SubmissionDefinition>& allSubmissions,
                  std::vector<StudentSubmission>& studentSubmissions);
