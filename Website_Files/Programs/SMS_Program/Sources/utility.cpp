
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
 --- FILE: utility.cpp ---
============================================================================

 Summary:
 This file contains various helper and calculation functions that are
 used by different parts of the application.

 It includes functions for:
 - Getting the current date.
 - Calculating student and class attendance percentages.
 - Calculating grade points and CGPA.
 - Finding submission records.
 - Calculating a student's final weighted mark in a course.

============================================================================
*/

#include <vector>
#include <string>
#include <map>
#include <ctime>    

// Project headers
#include "../headers/utility.h"
#include "../headers/attendance.h" 
#include "../headers/student.h"    


// Returns current date as a string (e.g., "November 19, 2025")
std::string getCurrentDate() {
    time_t t = time(nullptr); 
    char buffer[100];         

    if (strftime(buffer, sizeof(buffer), "%B %d, %Y", localtime(&t))) {
        return std::string(buffer);
    }
    return "Date Error";
}

// Calculates attendance % for one student in one specific course.
// Logic: Count Total 'P's / Total Classes * 100.
double calculateStudentAttendancePercentage(const std::string& studentUsername, const std::string& courseCode,
                                            const std::vector<AttendanceRecord>& allAttendance) {
    int presentCount = 0; 
    int totalCount = 0;   

    for (const auto& record : allAttendance) {
        if (record.studentUsername == studentUsername && record.courseCode == courseCode) {
            totalCount++; 
            if (record.status == 'P') {
                presentCount++; 
            }
        }
    }

    if (totalCount == 0) return 100.0; // Default to 100% if no classes yet
    return (static_cast<double>(presentCount) / totalCount) * 100.0;
}


// Calculates the average attendance for an entire section (e.g., "How is BCS-2A doing?").
// It sums up the attendance % of every student in that section and averages it.
double calculateClassAttendancePercentage(const std::string& sectionID, const std::string& courseCode,
                                          const std::vector<Student>& allStudents,
                                          const std::vector<AttendanceRecord>& allAttendance) {
    double totalPercentageSum = 0.0; 
    int studentCount = 0;            

    for (const auto& student : allStudents) {
        if (student.sectionID == sectionID) {
            double studentPerc = calculateStudentAttendancePercentage(student.username, courseCode, allAttendance);
            totalPercentageSum += studentPerc;
            studentCount++;
        }
    }

    if (studentCount == 0) return 100.0; 
    return totalPercentageSum / studentCount;
}

// Converts raw 0-100 marks to GPA points (4.0 scale).

double getGradePoint(int mark) {
    if (mark >= 90) return 4.0; 
    if (mark >= 85) return 3.7; 
    if (mark >= 80) return 3.3; 
    if (mark >= 75) return 3.0; 
    if (mark >= 70) return 2.7; 
    if (mark >= 65) return 2.3; 
    if (mark >= 60) return 2.0; 
    if (mark >= 55) return 1.7; 
    if (mark >= 50) return 1.0; 
    return 0.0; 
}

// Simple average of all course GPAs.
double calculateCGPA(const std::vector<Grade>& grades) {
    double totalPoints = 0.0; 

    if (grades.empty()) return 0.0;

    for (const auto& grade : grades) {
        totalPoints += getGradePoint(grade.mark);
    }
    return totalPoints / grades.size();
}

// --- Search Helpers ---

// Linear search to find a specific mark entry. 
// Used when a Teacher needs to EDIT a score.


StudentSubmission* findStudentSubmission(const std::string& subID, const std::string& username,
                                         std::vector<StudentSubmission>& studentSubmissions) {
    for (auto& ss : studentSubmissions) {
        if (ss.submissionID == subID && ss.studentUsername == username) {
            return &ss;
        }
    }
    return nullptr;
}

// Read-only version for viewing grades.
const StudentSubmission* findStudentSubmission(const std::string& subID, const std::string& username,
                                               const std::vector<StudentSubmission>& studentSubmissions) {
    for (const auto& ss : studentSubmissions) {
        if (ss.submissionID == subID && ss.studentUsername == username) {
            return &ss;
        }
    }
    return nullptr;
}


/*
    THE GRADING ENGINE
    
    This function calculates the final weighted grade (0-100) for a student.
    It is called whenever the Dashboard loads or a Teacher updates a mark.
    
    Algorithm:
    1. Identify the Student and Course.
    2. Find all Submissions (Assignments/Quizzes) for that course/section.
    3. For each submission, find the student's score and calculate the %.
    4. Group percentages into buckets (Assignments, Quizzes, Mids, Finals).
    5. Average the buckets.
    6. Apply weights (e.g., Final is worth 50%).
    7. Save the result into the student's Grade record.
*/

void updateStudentFinalMark(const std::string& studentUsername, const std::string& courseCode,
                            std::vector<Student>& allStudents,
                            const std::vector<SubmissionDefinition>& allSubmissions,
                            const std::vector<StudentSubmission>& studentSubmissions)
{
    Student* student = nullptr;
    Grade* gradeToUpdate = nullptr;
    const StudentSubmission* ss = nullptr; 

    // Buckets to hold total % for weighted averages
    double totalAssignmentPerc = 0.0; int assignmentCount = 0;
    double totalQuizPerc = 0.0;       int quizCount = 0;
    double totalMidtermPerc = 0.0;    int midtermCount = 0;
    double totalFinalPerc = 0.0;      int finalCount = 0;

    double percentage = 0.0; 
    double finalMark = 0.0;

    // 1. Find Student Object (so we can write the final grade back to it)
    for (auto& s : allStudents) {
        if (s.username == studentUsername) {
            student = &s;
            break;
        }
    }
    if (!student) return; 

    // 2. Find the Grade Object for this specific course
    for (auto& g : student->grades) {
        if (g.courseCode == courseCode) {
            gradeToUpdate = &g;
            break;
        }
    }
    if (!gradeToUpdate) return; 

    // 3. Process all submissions
    for (const auto& sub : allSubmissions) {
        
        // Does this submission apply to this student's section/course?
        if (sub.courseCode == courseCode && sub.sectionID == student->sectionID) {
            
            // Get the student's score
            ss = findStudentSubmission(sub.submissionID, student->username, studentSubmissions);
            
            if (ss && sub.totalMarks > 0) {
                // Calculate raw percentage for this item
                percentage = (ss->obtainedMarks / sub.totalMarks) * 100.0;

                // Add to the appropriate category bucket
                if (sub.type == "Assignment") {
                    totalAssignmentPerc += percentage;
                    assignmentCount++;
                } else if (sub.type == "Quiz") {
                    totalQuizPerc += percentage;
                    quizCount++;
                } else if (sub.type == "Midterm" || sub.type == "Exam") {
                    totalMidtermPerc += percentage;
                    midtermCount++;
                } else if (sub.type == "Final") {
                    totalFinalPerc += percentage;
                    finalCount++;
                } 
            }
        }
    }

    // 4. Calculate Category Averages (handling division by zero)
    double avgAssignment = (assignmentCount == 0) ? 0.0 : (totalAssignmentPerc / assignmentCount);
    double avgQuiz       = (quizCount == 0)       ? 0.0 : (totalQuizPerc / quizCount);
    double avgMidterm    = (midtermCount == 0)    ? 0.0 : (totalMidtermPerc / midtermCount);
    double avgFinal      = (finalCount == 0)      ? 0.0 : (totalFinalPerc / finalCount);

    // 5. Apply Weights (defined in course.h)
    finalMark = (avgAssignment * ASSIGNMENT_WEIGHT) +
                (avgQuiz * QUIZ_WEIGHT) +
                (avgMidterm * MIDTERM_WEIGHT) +
                (avgFinal * FINAL_WEIGHT);

    // 6. Save the result (rounded to nearest integer)
    gradeToUpdate->mark = static_cast<int>(finalMark + 0.5); 
}
