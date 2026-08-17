
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
 --- FILE: teacher.h ---
============================================================================

 Summary:
 This header defines the Teacher structure and the functions available
 to faculty members.

 Teachers can:
 1. View their specific schedule and student rosters.
 2. Create and manage assignments/quizzes.
 3. Enter grades for their students.
 4. Mark daily attendance.

============================================================================
*/

#pragma once
#include <string>
#include <vector>
#include "attendance.h" 

// We need to know about Submissions to grade them
struct SubmissionDefinition;
struct StudentSubmission;

// --- Forward Declarations ---
class Student;
class ScheduleEntry;
class Course;
class Teacher; 

/*
    Represents a faculty member.
    Stores login info and a list of Course Codes they are authorized to teach.
*/
class Teacher {
public:
    std::string username;
    std::string name;
    std::string password;
    
    // A list of course codes (e.g., "CSC101", "MTH202") assigned to this teacher.
    std::vector<std::string> assignedCourseCodes;
};


// --- Teacher Dashboard Functions ---

/*
    The main menu for Teachers.
    Passes editable references to data (Students, Attendance, Marks) because
    teachers need to modify these things daily.
*/
void showTeacherDashboard(Teacher& loggedInTeacher, std::vector<Student>& allStudents,
                          const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                          const std::vector<Teacher>& allTeachers, std::vector<AttendanceRecord>& allAttendance,
                          std::vector<SubmissionDefinition>& allSubmissions,
                          std::vector<StudentSubmission>& studentSubmissions);

// --- Attendance Management ---

/*
    Sub-menu for Attendance tasks (Mark Today, Fix Past Records).
*/
void teacher_AttendanceMenu(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                            const std::vector<Student>& allStudents, std::vector<AttendanceRecord>& allAttendance,
                            const std::vector<Course>& allCourses);

// --- Grade Management ---

/*
    Sub-menu for Grading tasks.
    Allows teachers to create assignments, delete them, or enter marks.
*/
void teacher_MarksMenu(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                       std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                       std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses);

/*
    Create a new assignment/quiz for a specific class section.
*/
void teacher_AddSubmission(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                           const std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                           std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses);

/*
    Remove an assignment. This also deletes all student marks associated with it.
*/
void teacher_DeleteSubmission(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                              std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                              std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses);

/*
    Change details of an assignment (like Total Marks).
    May trigger a rescaling of existing student grades.
*/
void teacher_ModifySubmission(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                              std::vector<Student>& allStudents, std::vector<SubmissionDefinition>& allSubmissions,
                              std::vector<StudentSubmission>& studentSubmissions, const std::vector<Course>& allCourses);

/*
    The actual data entry screen where teachers input scores for students.
*/
void teacher_ManageStudentMarks(Teacher& teacher, const std::vector<ScheduleEntry>& allSchedule,
                                std::vector<Student>& allStudents, const std::vector<SubmissionDefinition>& allSubmissions,
                                std::vector<StudentSubmission>& studentSubmissions);
