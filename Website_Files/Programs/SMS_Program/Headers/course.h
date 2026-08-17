
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
 --- FILE: course.h ---
============================================================================

 Summary:
 This header defines the fundamental academic structures: Courses, Grades,
 and the specific data types needed for the detailed grading system.

 It separates the "Definition" of an assignment (what is it?) from the
 "Submission" (what did the student get?), allowing for flexible gradebooks.

============================================================================
*/

#pragma once
#include <string>

// Represents a subject taught at the university.
class Course {
public:
    std::string courseCode; // e.g., "CSC103"
    std::string courseName; // e.g., "Programming Fundamentals"
};

// The "Report Card" grade. 
// This is the final, weighted number (0-100) stored in the Student object.
// It is automatically recalculated whenever a teacher updates a specific assignment mark.
class Grade {
public:
    std::string courseCode; 
    int mark;
    
    Grade(std::string code, int m) : courseCode(code), mark(m) {}
};

// --- The Grading Data Model ---

// 1. The "Column Header" in the gradebook.
// Created by the Teacher. It defines that an assignment EXISTS (e.g., "Quiz 1").
struct SubmissionDefinition {
    std::string submissionID;  // Unique Key (e.g. "CSC101_BCS-2A_Quiz1")
    std::string courseCode;    // Which course is this for?
    std::string sectionID;     // Which class section takes this?
    std::string type;          // "Assignment", "Quiz", "Midterm", or "Final"
    std::string title;         // Display name (e.g., "Lab 1")
    int totalMarks;            // Max possible score (e.g., 10)
};

// 2. The "Cell" in the gradebook.
// Links a specific Student to a specific SubmissionDefinition.
// This holds the actual score the student achieved.
struct StudentSubmission {
    std::string submissionID;     // Links back to the definition above
    std::string studentUsername;  // Links to the Student
    double obtainedMarks;         // The score (e.g., 8.5)
};

// --- Grading Weights (CSC101 Policy) ---
// Used by utility.cpp to calculate the final grade.
const double ASSIGNMENT_WEIGHT = 0.10; // 10%
const double QUIZ_WEIGHT = 0.15;       // 15%
const double MIDTERM_WEIGHT = 0.25;    // 25%
const double FINAL_WEIGHT = 0.50;      // 50%
