
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
 --- FILE: file_handler.h ---
============================================================================

 Summary:
 This header declares the interface for the File I/O system.

 It defines functions to:
 1. Load data from text files into program memory (Vectors).
 2. Save data from memory back to text files for persistence.

============================================================================
*/

#pragma once
#include <vector>
#include <string>
// Include all necessary class definitions so we can use them in function arguments
#include "student.h"
#include "teacher.h"
#include "admin.h"
#include "course.h"
#include "schedule.h"
#include "attendance.h" 

// --- Load Functions ---
// These functions act as the "Extraction" layer. They open specific text files,
// parse the CSV-like format, and reconstruct the objects in memory.


// Reads student data, including the variable-length list of enrolled courses and grades.
std::vector<Student> loadStudentsFromFile(const std::string& filename);

// Reads the list of teachers and their assigned course codes.
std::vector<Teacher> loadTeachersFromFile(const std::string& filename);

// Reads the list of administrators.
std::vector<Admin> loadAdminsFromFile(const std::string& filename);

// Reads the master list of available courses.
std::vector<Course> loadCoursesFromFile(const std::string& filename);

// Reads the master schedule (timetable) entries.
std::vector<ScheduleEntry> loadScheduleFromFile(const std::string& filename);

// Reads the log of all attendance records (Who was present/absent when).
std::vector<AttendanceRecord> loadAttendanceFromFile(const std::string& filename);

// Reads the definitions of assignments/quizzes (e.g., "Quiz 1 for CSC101").
std::vector<SubmissionDefinition> loadSubmissionsFromFile(const std::string& filename);

// Reads the actual marks students achieved on specific submissions.
std::vector<StudentSubmission> loadStudentSubmissionsFromFile(const std::string& filename);


// --- Save Functions ---
// These functions act as the "Storage" layer. They take the current state of
// the vectors and overwrite the text files with the updated data.

// Saves student data. It formats the grades list back into a single line string.
void saveStudentsToFile(const std::string& filename, const std::vector<Student>& students);

// Saves teacher credentials and their list of assigned courses.
void saveTeachersToFile(const std::string& filename, const std::vector<Teacher>& teachers);

// Saves the master schedule.
void saveScheduleToFile(const std::string& filename, const std::vector<ScheduleEntry>& schedule);

// Saves the list of courses.
void saveCoursesToFile(const std::string& filename, const std::vector<Course>& courses);

// Saves the attendance log.
void saveAttendanceToFile(const std::string& filename, const std::vector<AttendanceRecord>& attendance);

// Saves the list of created assignments/quizzes.
void saveSubmissionsToFile(const std::string& filename, const std::vector<SubmissionDefinition>& submissions);

// Saves the marks students received.
void saveStudentSubmissionsToFile(const std::string& filename, const std::vector<StudentSubmission>& studentSubmissions);

// Saves admin credentials.
void saveAdminsToFile(const std::string& filename, const std::vector<Admin>& admins);
