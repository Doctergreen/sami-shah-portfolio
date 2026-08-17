
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
 --- FILE: schedule.cpp ---
============================================================================

 Summary:
 This file implements all functions related to schedules.

 It handles:
 1. Displaying the visual grid for any schedule (student or teacher).
 2. Admin functions for adding, removing, and swapping class entries.
 3. Helper functions to get Course/Teacher names from their IDs.

============================================================================
*/

// --- Standard Library Includes ---
#include <iostream>
#include <iomanip>      // For std::setw, std::left
#include <limits>       // For std::numeric_limits
#include <vector>
#include <string>
#include <map>          // For std::map (used in grid display)
#include <algorithm>    // For std::max, std::find_if
#include <set>          // For std::set

// --- Project Header Includes ---
#include "../headers/schedule.h"
#include "../headers/course.h"
#include "../headers/teacher.h"
#include "../headers/student.h"
#include "../headers/admin.h"


// --- Helper Function Prototypes (for functions in this file) ---
int getValidatedChoice(int maxChoice);
int selectFromList(const std::string& title, const std::vector<std::string>& items);


/*
    A simple helper to get a number from the user and make sure it's valid.
    It prevents the program from crashing if the user types "abc" instead of "1".
*/
int getValidatedChoice(int maxChoice) {
    int choice;
    if (!(std::cin >> choice)) {
        std::cout << "Invalid input. Please enter a number.\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        system("pause");
        return -1; // Use -1 to signal an error
    }

    if (choice < 0 || choice > maxChoice) {
        std::cout << "Invalid choice. Please try again.\n";
        system("pause");
        return -1; // Use -1 to signal an error
    }
    
    if (choice == 0) {
        return 0; // 0 is always the "cancel" option
    }
    
    return choice;
}

/*
    A generic menu builder.
    Give it a title ("Pick a Day") and a list of items (Monday, Tuesday...),
    and it handles printing them out and getting the user's choice.
*/
int selectFromList(const std::string& title, const std::vector<std::string>& items) {
    std::cout << "\n" << title << "\n";
    for (size_t i = 0; i < items.size(); ++i) {
        std::cout << i + 1 << ". " << items[i] << "\n";
    }
    std::cout << "Enter choice (0 to cancel): ";

    int choice = getValidatedChoice(items.size()); // Get validated choice
    
    if (choice <= 0) {
        return -1; // -1 means cancel
    }
    
    return choice - 1; // Convert 1-based choice to 0-based index
}


// --- Main Function Definitions ---

/*
    Finds a course name (like "Intro to Programming") based on its code ("CSC101").
    Returns "Unknown Course" if it can't find it.
*/
std::string getCourseName(const std::string& code, const std::vector<Course>& courses) {
    // Loop through all courses
    for (const auto& c : courses) {
        if (c.courseCode == code) return c.courseName; // Found it
    }
    return "Unknown Course"; // Not found
}

/*
    Finds a teacher's real name based on their username ID.
*/
std::string getTeacherName(const std::string& id, const std::vector<Teacher>& teachers) {
    // Loop through all teachers
    for (const auto& t : teachers) {
        if (t.username == id) return t.name; // Found it
    }
    return "Unknown Teacher"; // Not found
}


/*
    This is the big function that draws the schedule grid.
    It works for Students, Teachers, and Admins because it just takes a list of 
    "ScheduleEntries" and draws them.
    
    It uses a Map to organize the entries by {Day, Time} so we can print them
    in the correct grid cells.
*/
void displayScheduleGrid(const std::vector<ScheduleEntry>& filteredSchedule, const std::vector<Course>& allCourses,
                         const std::vector<Teacher>& allTeachers, const std::string& title) {

    system("cls");
    std::cout << "--- " << title << " ---\n\n";

    if (filteredSchedule.empty()) {
        std::cout << "No schedule entries found for this view.\n";
        return; // Nothing to print
    }

    // --- Phase 1: Data Preparation ---
    // We do two things here:
    // 1. Calculate the best column width to fit all text nicely.
    // 2. Put all schedule entries into a 'map' for fast lookup.

    size_t maxCodeLength = 10;       // Min width for "Course Code"
    size_t maxCourseRoomLength = 15; // Min width for "Course (Room)"
    size_t maxTeacherLength = 15;    // Min width for "Teacher Name"
    const size_t MAX_REASONABLE_LENGTH = 50; // Cap to prevent huge columns

    // This map lets us look up a class just by its Day and Time
    // e.g., scheduleMap[{"Monday", "08:30-10:00"}]
    std::map<std::pair<std::string, std::string>, ScheduleEntry> scheduleMap;
    
    for (const auto& entry : filteredSchedule) {
        // Add this entry to the map for easy grid lookup
        scheduleMap[{entry.day, entry.timeSlot}] = entry;

        // --- Check column widths ---
        std::string courseName = getCourseName(entry.courseCode, allCourses);
        std::string teacherName = getTeacherName(entry.teacherID, allTeachers);
        std::string courseAndRoom = courseName + " (" + entry.classroom + ")";

        if (entry.courseCode.length() > maxCodeLength) {
            maxCodeLength = entry.courseCode.length();
        }
        if (teacherName.length() > maxTeacherLength && teacherName.length() < MAX_REASONABLE_LENGTH) {
            maxTeacherLength = teacherName.length();
        }
        if (courseAndRoom.length() > maxCourseRoomLength && courseAndRoom.length() < MAX_REASONABLE_LENGTH) {
            maxCourseRoomLength = courseAndRoom.length();
        }
    }

    // Find the largest of the three text types to set our column width
    size_t columnWidth = std::max({maxCodeLength, maxCourseRoomLength, maxTeacherLength}) + 2; // +2 for padding
    columnWidth = std::max(columnWidth, (size_t)15); // Ensure a minimum width

    // --- Phase 2: Display Logic ---
    const std::vector<std::string> timeSlots = {"08:30-10:00", "10:00-11:30", "11:30-13:00", "13:00-13:30", "13:30-15:00", "15:00-16:30"};
    const std::vector<std::string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    
    // --- Calculate total grid width for separators ---
    const size_t dayWidth = 12;
    const size_t breakWidth = 8;
    const size_t numTimeSlots = timeSlots.size() - 1; // -1 because "BREAK" has its own width
    const size_t totalWidth = dayWidth + (numTimeSlots * columnWidth) + breakWidth;
    
    // --- Print Header Row (Time Slots) ---
    std::cout << std::left << std::setw(dayWidth) << "Day";
    for (const auto& slot : timeSlots) {
         if(slot == "13:00-13:30") std::cout << std::setw(breakWidth) << "BREAK";
         else std::cout << std::setw(columnWidth) << slot;
    }
    std::cout << "\n" << std::string(totalWidth, '=') << "\n";

    // --- Print Data Rows (3 lines per day) ---
    for (const auto& day : days) {
        
        // This helper lambda prints one cell for a given day and slot
        auto printCell = [&](const std::string& slot, const std::string& content) {
             int currentWidth = (slot == "13:00-13:30") ? breakWidth : columnWidth;
             // Truncate content if it's too long for the cell
             std::string text = content.substr(0, currentWidth > 0 ? currentWidth - 1 : 0);
             std::cout << std::left << std::setw(currentWidth) << text;
        };

        // Row 1: Course Code
        std::cout << std::left << std::setw(dayWidth) << day;
        for (const auto& slot : timeSlots) {
             if (slot == "13:00-13:30") { printCell(slot, ""); continue; }
             auto key = std::make_pair(day, slot);
             if (scheduleMap.count(key)) {
                 printCell(slot, scheduleMap[key].courseCode);
             } else { printCell(slot, "---"); }
        }
        std::cout << "\n";

        // Row 2: Course Name (Room)
        std::cout << std::left << std::setw(dayWidth) << ""; // Empty day column
        for (const auto& slot : timeSlots) {
             if (slot == "13:00-13:30") { printCell(slot, "LUNCH"); continue; }
             auto key = std::make_pair(day, slot);
             if (scheduleMap.count(key)) {
                 std::string courseName = getCourseName(scheduleMap[key].courseCode, allCourses);
                 std::string room = scheduleMap[key].classroom;
                 printCell(slot, courseName + " (" + room + ")");
             } else { printCell(slot, ""); }
         }
         std::cout << "\n";

        // Row 3: Teacher Name
        std::cout << std::left << std::setw(dayWidth) << ""; // Empty day column
        for (const auto& slot : timeSlots) {
             if (slot == "13:00-13:30") { printCell(slot, ""); continue; }
             auto key = std::make_pair(day, slot);
             if (scheduleMap.count(key)) {
                 printCell(slot, getTeacherName(scheduleMap[key].teacherID, allTeachers));
             } else { printCell(slot, ""); }
         }
         std::cout << "\n" << std::string(totalWidth, '-') << "\n"; // Separator
    }
     std::cout << "\n";
}


/*
    The Admin's menu for creating a new class entry.
    It walks them through picking a Course, Teacher, Section, Day, and Time.
    Then it adds the new entry to the master schedule vector.
*/
void admin_AddScheduleEntry(std::vector<ScheduleEntry>& schedule, const std::vector<Course>& allCourses,
                            const std::vector<Teacher>& allTeachers, const std::vector<std::string>& allSectionIDs) {
    system("cls");
    std::cout << "--- Add New Class Entry ---\n\n";
    
    // Can't add an entry if there's nothing to add
    if (allCourses.empty() || allTeachers.empty() || allSectionIDs.empty()) {
        std::cout << "Error: Courses, Teachers, or Sections list is empty.\n";
        system("pause"); return;
    }
    
    ScheduleEntry newEntry;
    int choice;

    // --- Step 1: Select Course (Type Code) ---
    std::cout << "Available Courses:\n";
    for(const auto& c : allCourses) {
        // Display format: "CSC101 - Introduction to ICT"
        std::cout << " - " << c.courseCode << ": " << c.courseName << "\n";
    }
    std::cout << "\n";

    std::string typedCode;
    bool isValidCourse = false;

    // Validation Loop
    while (!isValidCourse) {
        std::cout << "Enter Course Code exactly (e.g., CSC103) or '0' to cancel: ";
        std::cin >> typedCode;

        if (typedCode == "0") {
            std::cout << "Operation cancelled.\n";
            system("pause");
            return;
        }

        // Check if typed code exists in the system
        for (const auto& c : allCourses) {
            if (c.courseCode == typedCode) {
                isValidCourse = true;
                newEntry.courseCode = c.courseCode; // Assign it
                std::cout << "Selected: " << c.courseName << "\n";
                break;
            }
        }

        if (!isValidCourse) {
            std::cout << "Error: Course code '" << typedCode << "' not found. Try again.\n";
        }
    }

    // --- Step 2: Select Teacher ---
    std::cout << "\nSelect a Teacher:\n";
    for(size_t i = 0; i < allTeachers.size(); ++i) {
        std::cout << i+1 << ". " << allTeachers[i].name << "\n";
    }
    std::cout << "Enter choice (0 to cancel): ";
    choice = getValidatedChoice(allTeachers.size());
    if (choice <= 0) { std::cout << "Operation cancelled.\n"; system("pause"); return; }
    newEntry.teacherID = allTeachers[choice - 1].username; // Assign the teacher's USERNAME

    // --- Step 3: Select Section ---
    std::cout << "\nSelect Section:\n";
    for(size_t i = 0; i < allSectionIDs.size(); ++i) {
        std::cout << i+1 << ". " << allSectionIDs[i] << "\n";
    }
    std::cout << "Enter choice (0 to cancel): ";
    choice = getValidatedChoice(allSectionIDs.size());
    if (choice <= 0) { std::cout << "Operation cancelled.\n"; system("pause"); return; }
    newEntry.sectionID = allSectionIDs[choice - 1];

    // --- Step 4: Get Day/Time/Room ---
    std::cout << "\nEnter Day (e.g., Monday): ";
    std::cin >> newEntry.day;
    std::cout << "Enter Time Slot (e.g., 08:30-10:00): ";
    std::cin >> newEntry.timeSlot;
    std::cout << "Enter Classroom (e.g., F-05): ";
    std::cin >> newEntry.classroom;
    
    // --- Step 5: Add to schedule ---
    schedule.push_back(newEntry);
    std::cout << "\nEntry added.\n";
    std::cout << "Remember to save data by exiting.\n";
    system("pause");
}

/*
    Allows the Admin to delete a specific class from the schedule.
*/
void admin_RemoveScheduleEntry(std::vector<ScheduleEntry>& schedule, const std::vector<Course>& allCourses) {
    system("cls");
    std::cout << "--- Remove Schedule Entry ---\n\n";
    if (schedule.empty()) { 
        std::cout << "Schedule is empty.\n";
        system("pause");
        return;
    }

    // --- Step 1: Display all current entries ---
    std::cout << "Select entry to remove:\n";
    for (size_t i = 0; i < schedule.size(); ++i) {
        std::cout << std::setw(3) << i + 1 << ". "
                  << std::setw(10) << schedule[i].sectionID << " | "
                  << std::setw(10) << schedule[i].day << " | "
                  << std::setw(15) << schedule[i].timeSlot << " | "
                  << getCourseName(schedule[i].courseCode, allCourses) << "\n";
    }

    // --- Step 2: Get user's choice ---
    std::cout << "\nEnter number to remove (0 to cancel): ";
    int choice = getValidatedChoice(schedule.size());

    // --- Step 3: Perform action ---
    if (choice > 0) {
        // User selected a valid entry
        schedule.erase(schedule.begin() + (choice - 1)); // (choice-1) converts to 0-based index
        std::cout << "Entry removed.\n";
        std::cout << "Remember to save data by exiting.\n";
    } else if (choice == 0) {
        std::cout << "Operation cancelled.\n";
    }
    
    system("pause");
}


/*
    The Swap function allows the Admin to exchange two class slots.
    Example: Move Monday 8:30 class to Friday 10:00.
    If the target slot is empty, it just moves the class there.
*/
void admin_SwapScheduleEntries(std::vector<ScheduleEntry>& schedule, const std::vector<Course>& allCourses,
                               const std::vector<std::string>& allSectionIDs) {
    system("cls");
    std::cout << "--- Swap Schedule Entries ---\n\n";
    
    if (schedule.empty() || allSectionIDs.empty()) {
        std::cout << "Schedule or sections list is empty.\n";
        system("pause");
        return;
    }

    // --- Define the options for Days and Times ---
    const std::vector<std::string> days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};
    const std::vector<std::string> timeSlots = {"08:30-10:00", "10:00-11:30", "11:30-13:00", "13:30-15:00", "15:00-16:30"};

    // --- Variable Declarations ---
    std::string selectedSection, day1, timeSlot1, day2, timeSlot2;
    int sectionIndex, day1Index, slot1Index, day2Index, slot2Index;

    // --- Step 1: Select Section ---
    sectionIndex = selectFromList("Select Section to modify:", allSectionIDs);
    if (sectionIndex == -1) { std::cout << "Operation cancelled.\n"; system("pause"); return; }
    selectedSection = allSectionIDs[sectionIndex];

    // --- Step 2: Get Slot 1 ---
    day1Index = selectFromList("Select Day for Slot 1:", days);
    if (day1Index == -1) { std::cout << "Operation cancelled.\n"; system("pause"); return; }
    day1 = days[day1Index];
    
    slot1Index = selectFromList("Select Time Slot for Slot 1:", timeSlots);
    if (slot1Index == -1) { std::cout << "Operation cancelled.\n"; system("pause"); return; }
    timeSlot1 = timeSlots[slot1Index];

    // --- Step 3: Get Slot 2 ---
    day2Index = selectFromList("Select Day for Slot 2:", days);
    if (day2Index == -1) { std::cout << "Operation cancelled.\n"; system("pause"); return; }
    day2 = days[day2Index];

    slot2Index = selectFromList("Select Time Slot for Slot 2:", timeSlots);
    if (slot2Index == -1) { std::cout << "Operation cancelled.\n"; system("pause"); return; }
    timeSlot2 = timeSlots[slot2Index];

    // --- Step 4: Find the schedule entries in the master vector ---
    // We search for entries that match the chosen Section + Day + Time
    int index1 = -1, index2 = -1;
    for(size_t i = 0; i < schedule.size(); ++i) {
        if (schedule[i].sectionID == selectedSection && schedule[i].day == day1 && schedule[i].timeSlot == timeSlot1) {
            index1 = static_cast<int>(i); // Found slot 1
        }
        if (schedule[i].sectionID == selectedSection && schedule[i].day == day2 && schedule[i].timeSlot == timeSlot2) {
            index2 = static_cast<int>(i); // Found slot 2
        }
    }

    // --- Step 5: Perform Swap Logic ---
    if (index1 != -1 && index2 != -1) {
        // Case 1: Both slots are occupied. Swap their contents.
        std::cout << "\nSwapping contents of [" << day1 << " " << timeSlot1 << "] with [" << day2 << " " << timeSlot2 << "]\n";
        // We only swap the class info, not the day/time/section
        std::swap(schedule[index1].courseCode, schedule[index2].courseCode);
        std::swap(schedule[index1].teacherID, schedule[index2].teacherID);
        std::swap(schedule[index1].classroom, schedule[index2].classroom);
        std::cout << "Swap successful!\n";
        
    } else if (index1 != -1 && index2 == -1) {
         // Case 2: Slot 1 has a class, Slot 2 is empty. Move 1 to 2.
         std::cout << "\nMoving entry from [" << day1 << " " << timeSlot1 << "] to empty slot [" << day2 << " " << timeSlot2 << "]\n";
         schedule[index1].day = day2;        // Update the entry's day
         schedule[index1].timeSlot = timeSlot2; // Update the entry's time
         std::cout << "Move successful!\n";
         
    } else if (index1 == -1 && index2 != -1) {
         // Case 3: Slot 2 has a class, Slot 1 is empty. Move 2 to 1.
         std::cout << "\nMoving entry from [" << day2 << " " << timeSlot2 << "] to empty slot [" << day1 << " " << timeSlot1 << "]\n";
         schedule[index2].day = day1;        // Update the entry's day
         schedule[index2].timeSlot = timeSlot1; // Update the entry's time
         std::cout << "Move successful!\n";
         
    } else {
        // Case 4: Both slots are empty.
        std::cout << "\nBoth selected slots are empty. Nothing to swap.\n";
    }

    std::cout << "Remember to save data by exiting.\n";
    system("pause");
}
