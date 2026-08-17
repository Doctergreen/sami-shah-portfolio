
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
 --- FILE: admin.h ---
============================================================================

 Summary:
 This header file defines the structures and function prototypes used by
 the Administrator module.

 It outlines the capabilities of an Admin user, including:
 1. Managing Students (adding, removing, editing, enrolling).
 2. Managing Teachers (hiring, firing, assigning courses).
 3. Managing Courses and Sections.
 4. Overseeing the Schedule.

============================================================================
*/

#pragma once
#include <string>
#include <vector>

// Include necessary headers for the data types used in function arguments
#include "student.h"
#include "teacher.h"
#include "course.h"
#include "schedule.h"
#include "attendance.h" 

// --- Forward Declarations ---
// We promise the compiler that these classes exist so we can use them 
// in function signatures without circular dependency issues.
class Student;
class Teacher;
class Course;
class ScheduleEntry;
class Admin; 
struct AttendanceRecord;
struct SubmissionDefinition;
struct StudentSubmission;


// Represents an Administrator user. They have full control over the system.
class Admin {
public:
    std::string username;
    std::string name;
    std::string password;
    std::string position; // e.g., "Registrar", "Head of Dept"
};

// The main dashboard function. Launches the Admin menu loop.
void showAdminDashboard(Admin& loggedInAdmin, std::vector<Student>& allStudents, std::vector<Teacher>& allTeachers,
                        std::vector<Course>& allCourses, std::vector<ScheduleEntry>& allSchedule,
                        const std::vector<AttendanceRecord>& allAttendance,
                        std::vector<SubmissionDefinition>& allSubmissions,
                        std::vector<StudentSubmission>& studentSubmissions);

// --- Helper Function Prototypes ---

// --- Student Management ---

// The main menu for Student operations (Add, Remove, Update).
void admin_ManageStudents(std::vector<Student>& allStudents, const std::vector<Course>& allCourses, 
                          const std::vector<std::string>& allSectionIDs, const std::vector<AttendanceRecord>& allAttendance,
                          std::vector<SubmissionDefinition>& allSubmissions,
                          std::vector<StudentSubmission>& studentSubmissions);

void admin_AddStudent(std::vector<Student>& allStudents, const std::vector<Course>& allCourses, const std::vector<std::string>& allSectionIDs);
void admin_RemoveStudent(std::vector<Student>& allStudents);

// Sub-menu to look at specific student data (grades, attendance).
void admin_ViewStudentDetailsMenu(std::vector<Student>& allStudents, const std::vector<Course>& allCourses, 
                                  const std::vector<AttendanceRecord>& allAttendance,
                                  std::vector<SubmissionDefinition>& allSubmissions,
                                  std::vector<StudentSubmission>& studentSubmissions);

// Lists every student in the system.
void admin_ViewAllStudents(const std::vector<Student>& allStudents);

// Shows detailed info (transcript/attendance) for one specific student.
void admin_ViewSpecificStudent(std::vector<Student>& allStudents, const std::vector<Course>& allCourses, 
                               const std::vector<AttendanceRecord>& allAttendance,
                               std::vector<SubmissionDefinition>& allSubmissions,
                               std::vector<StudentSubmission>& studentSubmissions);

void admin_EnrollStudentInCourse(Student& student, const std::vector<Course>& allCourses);
void admin_AssignStudentToSection(std::vector<Student>& allStudents, const std::vector<std::string>& allSectionIDs);
void admin_TransferStudentSection(std::vector<Student>& allStudents, const std::vector<std::string>& allSectionIDs);

// --- Teacher Management ---

// Main menu for Teacher operations.
void admin_ManageTeachers(std::vector<Teacher>& allTeachers, const std::vector<Course>& allCourses);

void admin_ViewTeachers(const std::vector<Teacher>& allTeachers);
void admin_AddTeacher(std::vector<Teacher>& allTeachers, const std::vector<Course>& allCourses);
void admin_RemoveTeacher(std::vector<Teacher>& allTeachers);
void admin_ManageTeacherCourses(std::vector<Teacher>& allTeachers, const std::vector<Course>& allCourses);

// --- Course Management ---

// Main menu for creating or deleting courses.
void admin_ManageCourses(std::vector<Course>& allCourses);

void admin_ViewCourses(const std::vector<Course>& allCourses);
void admin_AddCourse(std::vector<Course>& allCourses);
void admin_RemoveCourse(std::vector<Course>& allCourses);

// --- Section Management ---

// Main menu for managing class sections (e.g., BCS-2A).
void admin_ManageSections(std::vector<std::string>& allSectionIDs, std::vector<Student>& allStudents,
                          std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                          const std::vector<Teacher>& allTeachers);

// Lists all available sections.
void admin_ViewSections(const std::vector<std::string>& allSectionIDs, const std::vector<Student>& allStudents,
                        const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                        const std::vector<Teacher>& allTeachers);

// Shows the roster and teachers for a specific section.
void admin_ViewSpecificSectionDetails(const std::vector<std::string>& allSectionIDs, const std::vector<Student>& allStudents,
                                      const std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                                      const std::vector<Teacher>& allTeachers); 

void admin_AddSection(std::vector<std::string>& allSectionIDs);
void admin_RemoveSection(std::vector<std::string>& allSectionIDs, std::vector<Student>& allStudents, std::vector<ScheduleEntry>& allSchedule);

// --- Schedule Management ---

// Sub-menu to view or edit the master schedule.
void showAdminScheduleMenu(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                           const std::vector<Teacher>& allTeachers, const std::vector<std::string>& allSectionIDs);

void admin_ViewTeacherSchedule(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                               const std::vector<Teacher>& allTeachers);

void admin_ViewSectionSchedule(std::vector<ScheduleEntry>& allSchedule, const std::vector<Course>& allCourses,
                               const std::vector<Teacher>& allTeachers, const std::vector<std::string>& allSectionIDs); 

void admin_AddScheduleEntry(std::vector<ScheduleEntry>& schedule, const std::vector<Course>& allCourses,
                            const std::vector<Teacher>& allTeachers, const std::vector<std::string>& allSectionIDs);

void admin_RemoveScheduleEntry(std::vector<ScheduleEntry>& schedule, const std::vector<Course>& allCourses);

void admin_SwapScheduleEntries(std::vector<ScheduleEntry>& schedule, const std::vector<Course>& allCourses,
                               const std::vector<std::string>& allSectionIDs);

// Allows an admin to manually override a student's mark for a specific assignment.
void admin_EditStudentSubmissionMarks(Student& student, std::vector<Student>& allStudents,
                                      const std::vector<Course>& allCourses,
                                      std::vector<SubmissionDefinition>& allSubmissions,
                                      std::vector<StudentSubmission>& studentSubmissions);
