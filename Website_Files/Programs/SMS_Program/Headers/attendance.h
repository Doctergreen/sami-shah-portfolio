
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
 --- FILE: attendance.h ---
============================================================================

 Summary:
 This file defines the fundamental unit of attendance tracking.

 An AttendanceRecord represents a single event: one student, in one class,
 on one specific day, being marked as either Present or Absent.

============================================================================
*/

#pragma once 
#include <string>

// This structure acts like a single row in an attendance logbook.
struct AttendanceRecord {
    std::string courseCode;       // Which course is this for? (e.g., "CSC101")
    std::string studentUsername;  // Who is the student? (Links to Student.username)
    std::string sectionID;        // Which section did they attend? (e.g., "BCS-2A")
    std::string date;             // When was the class? (e.g., "November 10, 2025")
    char status;                  // 'P' = Present, 'A' = Absent
};




