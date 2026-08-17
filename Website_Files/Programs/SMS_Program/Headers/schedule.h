
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
//   Last Modified: November 14, 2025                                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

/*
============================================================================
 --- FILE: schedule.h ---
============================================================================

 Summary:
 This header defines what a "Class" looks like in the timetable (ScheduleEntry).

 It also declares the functions needed to:
 1. Translate IDs (like teacher usernames) into real names.
 2. Draw the big schedule grid that everyone sees on their dashboard.

============================================================================
*/

#pragma once
#include <string>
#include <vector>

// --- Forward Declarations ---
// We tell the compiler these classes exist so we can use them in function arguments
// without needing to include their full header files right here (avoids circular dependency loops).
class Course;
class Teacher;
class Student;
class ScheduleEntry; 

/*
    This represents a single slot in the timetable.
    e.g., "Monday 8:30, Room F-05, Teaching Math to Section A".
    
    The system holds a giant vector of these entries to make up the Master Schedule.
*/
class ScheduleEntry {
public:
    std::string courseCode; // e.g., CSC101
    std::string teacherID;  // The username of the teacher (e.g., "SamiaRiaz")
    std::string sectionID;  // Who is taking this class? (e.g., "BCS-2A")
    std::string day;        // e.g., "Monday"
    std::string timeSlot;   // e.g., "08:30-10:00"
    std::string classroom;  // e.g., "Lab-3"
};

// --- Helper Function Prototypes ---

/*
    These are lookup helpers. 
    In the schedule file, we only store codes (like "CSC101" or "DrSmith").
    These functions look through the main lists to find the actual human-readable
    Names so we can print them nicely on the screen.
*/
std::string getCourseName(const std::string& code, const std::vector<Course>& courses);
std::string getTeacherName(const std::string& id, const std::vector<Teacher>& teachers);

// --- Common Grid Display Prototype ---

/*
    This is the main function that draws the schedule.
    It takes a list of entries (filtered for a specific student or teacher)
    and prints the formatted ASCII table.
*/
void displayScheduleGrid(const std::vector<ScheduleEntry>& filteredSchedule, const std::vector<Course>& allCourses,
                         const std::vector<Teacher>& allTeachers, const std::string& title);
                         







