
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
 --- FILE: file_handler.cpp ---
============================================================================

 Summary:
 This file implements all functions responsible for reading data from
 text files (loading) and writing data back to them (saving).

 It's the bridge between the program's memory (global vectors)
 and the persistent storage on disk (.txt files).

============================================================================
*/

// --- Standard Library Includes ---
#include <iostream>
#include <fstream>      // For file input/output (ifstream, ofstream)
#include <sstream>      // For std::stringstream (used to parse lines)
#include <string>
#include <vector>
#include <stdexcept>    // For std::stoi (in loadStudentsFromFile)
#include <algorithm>
#include <limits>
#include <iomanip>      // For std::setprecision (in saveStudentSubmissions)

// --- Project Header Includes ---
// We need the full definitions of all our data structures
// to be able to create them and fill them with data.
#include "../headers/file_handler.h"
#include "../headers/student.h"
#include "../headers/teacher.h"
#include "../headers/admin.h"
#include "../headers/course.h"
#include "../headers/schedule.h"
#include "../headers/attendance.h"


/**
 * trim (Helper Function)
 * A small, reusable function to remove leading and trailing
 * whitespace (spaces, tabs, newlines) from a string.
 *
 * @param str The string to trim.
 * @return The trimmed string.
 */
std::string trim(const std::string& str) {
    // Find the first character that is *not* whitespace
    size_t first = str.find_first_not_of(" \t\n\r");
    
    // If the string is all whitespace, return an empty string
    if (std::string::npos == first) {
        return "";
    }
    
    // Find the last character that is *not* whitespace
    size_t last = str.find_last_not_of(" \t\n\r");
    
    // Return the substring between the first and last non-whitespace chars
    return str.substr(first, (last - first + 1));
}

// --- STUDENT FILES ---

/**
 * loadStudentsFromFile
 * Loads the 'students.txt' file.
 * This loader is complex because the student's courses/marks are
 * a variable-length list at the end of the line.
 */
std::vector<Student> loadStudentsFromFile(const std::string& filename) {
    std::vector<Student> students;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) { return students; } // Return empty vector if file not found

    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue; // Skip comments/empty lines

        std::stringstream ss(line);
        std::string field;
        Student tempStudent;

        // --- Read the fixed-size fields ---
        // Format: Username,RollNumber,Name,Password,SectionID,...
        if (std::getline(ss, tempStudent.username, ',') &&
            std::getline(ss, tempStudent.studentID, ',') &&
            std::getline(ss, tempStudent.name, ',') &&
            std::getline(ss, tempStudent.password, ',') &&
            std::getline(ss, tempStudent.sectionID, ','))
        {
            // --- Read the variable-size fields (Grades) ---
            // The rest of the line is a list of "Course:Mark" pairs
            std::string gradeStr;
            while (std::getline(ss, gradeStr, ',')) {
                 std::size_t colonPos = gradeStr.find(':');
                 
                 // Ensure the format is "CODE:MARK"
                 if (colonPos != std::string::npos && colonPos + 1 < gradeStr.length()) {
                     std::string code = gradeStr.substr(0, colonPos);
                     try {
                         // Convert the mark (a string) to an integer
                         int mark = std::stoi(gradeStr.substr(colonPos + 1));
                         tempStudent.grades.push_back(Grade(code, mark));
                     } catch (...) {
                         // Catch errors if mark is not a valid number
                         std::cerr << "Warning: Skipping bad grade data '" << gradeStr << "' for student " << tempStudent.username << "\n";
                     }
                 }
            }
            students.push_back(tempStudent);
        } else {
             std::cerr << "Warning: Skipping malformed student line: " << line << "\n";
        }
    }
    inFile.close();
    return students;
}

/**
 * saveStudentsToFile
 * Saves the 'students.txt' file from the global vector.
 */
void saveStudentsToFile(const std::string& filename, const std::vector<Student>& students) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) { return; }
    
    outFile << "# Format: Username,RollNumber,Name,Password,SectionID,Course:Mark...\n";
    for (const auto& student : students) {
        outFile << student.username << "," << student.studentID << "," << student.name << ","
                << student.password << "," << student.sectionID;
        
        // Loop through and append all the student's grades
        for (const auto& grade : student.grades) {
            outFile << "," << grade.courseCode << ":" << grade.mark;
        }
        outFile << std::endl;
    }
    outFile.close();
}

// --- ATTENDANCE FILES ---

/**
 * loadAttendanceFromFile
 * Loads the 'attendance.txt' file.
 * This is a smart loader that can handle commas in the date field.
 */
std::vector<AttendanceRecord> loadAttendanceFromFile(const std::string& filename) {
    std::vector<AttendanceRecord> attendanceRecords;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) { return attendanceRecords; }

    std::string line;
    int lineNumber = 0;
    while (std::getline(inFile, line)) {
        lineNumber++;
        if (line.empty() || line[0] == '#') continue;

        std::string course, user, section, date, statusStr;
        std::stringstream ss(line);

        // 1. Get the first three fixed fields
        if (!std::getline(ss, course, ',') || 
            !std::getline(ss, user, ',') || 
            !std::getline(ss, section, ',')) {
            std::cerr << "Warning [Attendance Line " << lineNumber << "]: Malformed line: " << line << "\n";
            continue;
        }

        // 2. The rest of the line is "Date,Status" (e.g., "October 27, 2025,P")
        std::string restOfLine;
        if (!std::getline(ss, restOfLine)) {
            std::cerr << "Warning [Attendance Line " << lineNumber << "]: Missing Date/Status in line: " << line << "\n";
            continue;
        }

        // 3. Find the *last* comma. This reliably separates the Date from the Status.
        size_t lastComma = restOfLine.find_last_of(',');
        if (lastComma == std::string::npos || lastComma == restOfLine.length() - 1) {
            // No comma found, or it's the last character (no status)
            std::cerr << "Warning [Attendance Line " << lineNumber << "]: Cannot find Status in line: " << line << "\n";
            continue;
        }

        // 4. Extract Date (everything before last comma) and Status (everything after)
        date = restOfLine.substr(0, lastComma);
        statusStr = restOfLine.substr(lastComma + 1);

        // 5. Trim whitespace and validate status
        date = trim(date);
        statusStr = trim(statusStr);

        if (statusStr.length() != 1 || (statusStr[0] != 'P' && statusStr[0] != 'A')) {
            std::cerr << "Warning [Attendance Line " << lineNumber << "]: Invalid status '" << statusStr << "' in line: " << line << "\n";
            continue;
        }

        // 6. Create and store the record
        AttendanceRecord tempRecord;
        tempRecord.courseCode = trim(course); // Trim all fields for safety
        tempRecord.studentUsername = trim(user);
        tempRecord.sectionID = trim(section);
        tempRecord.date = date;
        tempRecord.status = statusStr[0];
        attendanceRecords.push_back(tempRecord);
    }
    inFile.close();
    return attendanceRecords;
}

/**
 * saveAttendanceToFile
 * Saves the 'attendance.txt' file from the global vector.
 */
void saveAttendanceToFile(const std::string& filename, const std::vector<AttendanceRecord>& attendance) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open attendance file for saving: " << filename << std::endl;
        return;
    }
    outFile << "# Format: CourseCode,StudentUsername,SectionID,Date,Status(P/A)\n";
    for (const auto& record : attendance) {
        outFile << record.courseCode << ","
                << record.studentUsername << ","
                << record.sectionID << ","
                << record.date << ","
                << record.status << std::endl;
    }
    outFile.close();
}


// --- TEACHER FILES ---

/**
 * loadTeachersFromFile
 * Loads the 'teachers.txt' file.
 * This loader is simple: 3 fixed fields, then a variable list of courses.
 */
std::vector<Teacher> loadTeachersFromFile(const std::string& filename) {
    std::vector<Teacher> teachers;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) { return teachers; }
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::stringstream ss(line);
        std::string field;
        Teacher tempTeacher;

        // 1. Read the fixed fields (Username, Name, Password)
        if (!std::getline(ss, tempTeacher.username, ',') ||
            !std::getline(ss, tempTeacher.name, ',') ||
            !std::getline(ss, tempTeacher.password, ','))
        {
            std::cerr << "Warning [Teacher]: Skipping malformed line: " << line << "\n";
            continue;
        }

        // 2. Read all remaining fields as assigned course codes
        while (std::getline(ss, field, ',')) {
            field = trim(field);
            if (!field.empty()) {
                tempTeacher.assignedCourseCodes.push_back(field);
            }
        }
        teachers.push_back(tempTeacher);
    }
    inFile.close();
    return teachers;
}

/**
 * saveTeachersToFile
 * Saves the 'teachers.txt' file from the global vector.
 */
void saveTeachersToFile(const std::string& filename, const std::vector<Teacher>& teachers) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) { return; }
    
    outFile << "# Format: Username,Name,Password,CourseCode1,CourseCode2...\n";
    for (const auto& teacher : teachers) {
        outFile << teacher.username << "," << teacher.name << "," << teacher.password;
        // Append all assigned courses
        for (const auto& code : teacher.assignedCourseCodes) {
            outFile << "," << code;
        }
        outFile << std::endl;
    }
    outFile.close();
}

// --- ADMIN FILES ---

/**
 * loadAdminsFromFile
 * Loads the 'admins.txt' file. This is a simple, fixed-field format.
 */
std::vector<Admin> loadAdminsFromFile(const std::string& filename) {
    std::vector<Admin> admins;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) { return admins; }
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::stringstream ss(line);
        Admin tempAdmin;
        
        // Format: Username,Name,Password,Position
        if (std::getline(ss, tempAdmin.username, ',') &&
            std::getline(ss, tempAdmin.name, ',') &&
            std::getline(ss, tempAdmin.password, ',') &&
            std::getline(ss, tempAdmin.position, ','))
        {
            admins.push_back(tempAdmin);
        } else {
            std::cerr << "Warning [Admin]: Skipping malformed line: " << line << "\n";
        }
    }
    inFile.close();
    return admins;
}

/**
 * saveAdminsToFile
 * Saves the 'admins.txt' file from the global vector.
 */
void saveAdminsToFile(const std::string& filename, const std::vector<Admin>& admins) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) { return; }
    
    outFile << "# Format: Username,Name,Password,Position\n";
    for (const auto& admin : admins) {
        outFile << admin.username << "," << admin.name << "," << admin.password << "," << admin.position << std::endl;
    }
    outFile.close();
}

// --- COURSE FILES ---

/**
 * loadCoursesFromFile
 * Loads the 'courses.txt' file. This is a simple, fixed-field format.
 */
std::vector<Course> loadCoursesFromFile(const std::string& filename) {
    std::vector<Course> courses;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) { return courses; }
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::stringstream ss(line);
        Course tempCourse;
        
        // Format: CourseCode,CourseName
        if (std::getline(ss, tempCourse.courseCode, ',') &&
            std::getline(ss, tempCourse.courseName, ','))
        {
            courses.push_back(tempCourse);
        } else {
            std::cerr << "Warning [Course]: Skipping malformed line: " << line << "\n";
        }
    }
    inFile.close();
    return courses;
}

/**
 * saveCoursesToFile
 * Saves the 'courses.txt' file from the global vector.
 */
void saveCoursesToFile(const std::string& filename, const std::vector<Course>& courses) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) { return; }
    
    outFile << "# BCS Courses\n"; // Keep the original header
    for (const auto& course : courses) {
        outFile << course.courseCode << "," << course.courseName << std::endl;
    }
    outFile.close();
}

// --- SCHEDULE FILES ---

/**
 * loadScheduleFromFile
 * Loads the 'schedule.txt' file. This is a fixed-field format
 * but uses trim() just in case of whitespace in the file.
 */
std::vector<ScheduleEntry> loadScheduleFromFile(const std::string& filename) {
    std::vector<ScheduleEntry> schedule;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) { return schedule; }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(inFile, line)) {
        lineNumber++;
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        ScheduleEntry tempEntry;
        // Temporary strings to read into
        std::string code, teacher, day, time, room, section;

        // Format: CourseCode,TeacherID,Day,TimeSlot,Classroom,SectionID
        if (std::getline(ss, code, ',') &&
            std::getline(ss, teacher, ',') &&
            std::getline(ss, day, ',') &&
            std::getline(ss, time, ',') &&
            std::getline(ss, room, ',') &&
            std::getline(ss, section, ','))
        {
            // Apply trim to all fields to remove any extra whitespace
            tempEntry.courseCode = trim(code);
            tempEntry.teacherID = trim(teacher);
            tempEntry.day = trim(day);
            tempEntry.timeSlot = trim(time);
            tempEntry.classroom = trim(room);
            tempEntry.sectionID = trim(section);

            // Check if any field is empty *after* trimming
            if (tempEntry.courseCode.empty() || tempEntry.teacherID.empty() ||
                tempEntry.day.empty() || tempEntry.timeSlot.empty() ||
                tempEntry.classroom.empty() || tempEntry.sectionID.empty()) {
                std::cerr << "Warning [Schedule Line " << lineNumber << "]: Skipping line with empty field(s): " << line << "\n";
            } else {
                schedule.push_back(tempEntry);
            }
        } else {
             std::cerr << "Warning [Schedule Line " << lineNumber << "]: Skipping malformed line (expected 6 fields): " << line << "\n";
        }
    }
    inFile.close();
    return schedule;
}

/**
 * saveScheduleToFile
 * Saves the 'schedule.txt' file from the global vector.
 */
void saveScheduleToFile(const std::string& filename, const std::vector<ScheduleEntry>& schedule) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) { return; }
    
    outFile << "# Format: CourseCode,TeacherID,Day,TimeSlot,Classroom,SectionID\n";
    for (const auto& entry : schedule) {
        outFile << entry.courseCode << "," << entry.teacherID << ","
                << entry.day << "," << entry.timeSlot << ","
                << entry.classroom << "," << entry.sectionID << std::endl;
    }
    outFile.close();
}


// --- SUBMISSION (DEFINITION) FILES ---

/**
 * loadSubmissionsFromFile
 * Loads the 'submissions.txt' file.
 */
std::vector<SubmissionDefinition> loadSubmissionsFromFile(const std::string& filename) {
    std::vector<SubmissionDefinition> submissions;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) { return submissions; }
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::stringstream ss(line);
        SubmissionDefinition tempSub;
        
        // Format: SubmissionID,CourseCode,SectionID,Type,Title,TotalMarks
        if (std::getline(ss, tempSub.submissionID, ',') &&
            std::getline(ss, tempSub.courseCode, ',') &&
            std::getline(ss, tempSub.sectionID, ',') &&
            std::getline(ss, tempSub.type, ',') &&
            std::getline(ss, tempSub.title, ',') &&
            (ss >> tempSub.totalMarks)) // Read the last item (an int)
        {
            submissions.push_back(tempSub);
        } else {
            std::cerr << "Warning [Submissions]: Skipping malformed line: " << line << "\n";
        }
    }
    inFile.close();
    return submissions;
}

/**
 * saveSubmissionsToFile
 * Saves the 'submissions.txt' file from the global vector.
 */
void saveSubmissionsToFile(const std::string& filename, const std::vector<SubmissionDefinition>& submissions) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) { return; }
    
    outFile << "# Format: SubmissionID,CourseCode,SectionID,Type,Title,TotalMarks\n";
    for (const auto& sub : submissions) {
        outFile << sub.submissionID << "," << sub.courseCode << ","
                << sub.sectionID << "," << sub.type << ","
                << sub.title << "," << sub.totalMarks << std::endl;
    }
    outFile.close();
}


// --- STUDENT SUBMISSION (MARKS) FILES ---

/**
 * loadStudentSubmissionsFromFile
 * Loads the 'student_marks.txt' file.
 */
std::vector<StudentSubmission> loadStudentSubmissionsFromFile(const std::string& filename) {
    std::vector<StudentSubmission> studentSubmissions;
    std::ifstream inFile(filename);
    if (!inFile.is_open()) { return studentSubmissions; }
    
    std::string line;
    while (std::getline(inFile, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        std::stringstream ss(line);
        StudentSubmission tempMark;
        
        // Format: SubmissionID,StudentUsername,ObtainedMarks
        if (std::getline(ss, tempMark.submissionID, ',') &&
            std::getline(ss, tempMark.studentUsername, ',') &&
            (ss >> tempMark.obtainedMarks)) // Read the last item (a double)
        {
            studentSubmissions.push_back(tempMark);
        } else {
            std::cerr << "Warning [Student Marks]: Skipping malformed line: " << line << "\n";
        }
    }
    inFile.close();
    return studentSubmissions;
}

/**
 * saveStudentSubmissionsToFile
 * Saves the 'student_marks.txt' file from the global vector.
 */
void saveStudentSubmissionsToFile(const std::string& filename, const std::vector<StudentSubmission>& studentSubmissions) {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) { return; }
    
    outFile << "# Format: SubmissionID,StudentUsername,ObtainedMarks\n";
    // Set precision for saving double values (e.g., 12.5)
    outFile << std::fixed << std::setprecision(1); 
    
    for (const auto& mark : studentSubmissions) {
        outFile << mark.submissionID << "," << mark.studentUsername << ","
                << mark.obtainedMarks << std::endl;
    }
    outFile.close();
}
