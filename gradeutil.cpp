/*gradeutil.cpp*/

//
// Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 341, Spring 2019
// Project #02: GradeUtil API (Solution)
//
// Edited by Sean Deitz
// for CS 341, Fall 2019
//

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <set>
#include <map>
#include <functional>

#include "gradeutil.h"

using namespace std;

//Functions prototypes
College InputGradeData(string filename);
void collegeSummary(const College& C);
void askCommand(College& college);
void summary(const College& college, string userInput1, map<string, Dept>& map);
void search(const College& college, string userInput1, map<string, Dept>& map);
void satisfactory(const College& college, string userInput1, map<string, Dept>& map);
void dfw(const College& college, string userInput1, map<string, Dept>& map);
void letterB(const College& college, string userInput1, map<string, Dept>& map);
void average(const College& college);

Course ParseCourse(string csvline);
double GetDFWRate(const Course& c, int& DFW, int& N);
double GetDFWRate(const Dept& dept, int& DFW, int& N);
double GetDFWRate(const College& college, int& DFW, int& N);
GradeStats GetGradeDistribution(const Course& c);
GradeStats GetGradeDistribution(const Dept& dept);
GradeStats GetGradeDistribution(const College& college);
vector<Course> FindCourses(const Dept& dept, int courseNumber);
vector<Course> FindCourses(const Dept& dept, string instructorPrefix);
vector<Course> FindCourses(const College& college, int courseNumber);
vector<Course> FindCourses(const College& college, string instructorPrefix);


void Dept::addCourse(Course course)
{
   Courses.push_back(course);
}


void College::addDepartment(Dept department)
{
   Depts.push_back(department);
}


int main()
{
   string filename;
   string userInput;

   cout << std::fixed;
   cout << std::setprecision(2);

   //cin >> filename; //Use fall-2018.csv for convinence

   //Read in the file and push the courses and department into the vectors
   College college = InputGradeData("fall-2018.csv"); //filename = fall-2018.csv

   //Summary for the file
   collegeSummary(college);

   cout << endl;

   //Ask user to enter the command
   askCommand(college);

   return 0;
}


College InputGradeData(string filename)
{
    College college;
    ifstream file(filename);
    string line, value;

    if (!file.good())
    {
        cout << "**Error: unable to open input file '" << filename << "'." << endl;
        return college;
    }

    // first line contains semester,year
    getline(file, line);
    stringstream ss(line);

    getline(ss, college.Name, ',');
    getline(ss, college.Semester, ',');
    getline(ss, value);
    college.Year = stoi(value);

    // second line contains column headers --- skip
    getline(file, line);

    //
    // now start inputting and parse course data:
    //

    while (getline(file, line))
    {
        Course c = ParseCourse(line);

        //
        // search for correct dept to add course to, otherwise create a new dept:
        //
        auto dept_iter = std::find_if(college.Depts.begin(),
                                      college.Depts.end(),
                                      [&](const Dept &d) {
                                          return (d.Name == c.Dept);
                                      });

        if (dept_iter == college.Depts.end())
        {
            //
            // doesn't exist, so we have to create a new dept
            // and insert course:
            //
            Dept d(c.Dept);

            d.Courses.push_back(c);

            college.Depts.push_back(d);
        }
        else
        {
            // dept exists, so insert course into existing dept:
            dept_iter->Courses.push_back(c);
        }

    } //while

    //
    // done:
    //
    return college;
}


void collegeSummary(const College& C)
{
   int DFW, N;

   //variable to store the college member
   size_t numberCourses = C.NumCourses();
   size_t numberStudent = C.NumStudents();

   //Variable to store the DFW rate
   double DFWRate = GetDFWRate(C, DFW, N);

   //Create an GradeStats object
   GradeStats gradeStats = GetGradeDistribution(C);

   //Print the title
   cout << "** College of " << C.Name << ", " << C.Semester
        << " " << C.Year << " **" << endl;

   //Print the number of courses
   cout << "# of courses taught: " << numberCourses << endl;

   //Print the number of student
   cout << "# of students taught: " << numberStudent << endl;

   //Print the grade distribution
   cout << "grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
        << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;

   //Print the DFW rate
   cout << "DFW rate: " << DFWRate << "%" << endl;
}


void askCommand(College& college)
{
   string userInput, userInput1;

   //Ask user to input the command
   cout << "Enter a command> ";
   cin >> userInput;

   //While user input is not #, keep asking to enter a command
   while(userInput != "#")
   {
      map<string, Dept> map;

      if(userInput == "summary")
      {
         //Ask user to enter dept name or for all depts
         cout << "dept name, or all? ";
         cin >> userInput1;

         //If user input is not "all", we push dept into map
         if(userInput1 != "all")
         {
            //Loop through all depts in the college.Depts vector
            for(const Dept& dept: college.Depts)
            {
               //Using dept's name as key, dept as value
               auto keyvaluepair = pair<string, Dept>(dept.Name, dept);
               map.insert(keyvaluepair);
            }
         }

         //Function to do the summary command
         summary(college, userInput1, map);
      }
      else if(userInput == "search")
      {
         //Ask user to enter dept name or for all depts
         cout << "dept name, or all? ";
         cin >> userInput1;

         //If user input is not "all", we push dept into map
         if(userInput1 != "all")
         {
            //Loop through all depts in the college.Depts vector
            for(const Dept& dept: college.Depts)
            {
               //Using dept's name as key, dept as value
               auto keyvaluepair = pair<string, Dept>(dept.Name, dept);
               map.insert(keyvaluepair);
            }
         }

         search(college, userInput1, map);
      }
      else if(userInput == "satisfactory")
      {
         //Ask user to enter dept name or for all depts
         cout << "dept name, or all? ";
         cin >> userInput1;

         //If user input is not "all", we push dept into map
         if(userInput1 != "all")
         {
            //Loop through all depts in the college.Depts vector
            for(const Dept& dept: college.Depts)
            {
               //Using dept's name as key, dept as value
               auto keyvaluepair = pair<string, Dept>(dept.Name, dept);
               map.insert(keyvaluepair);
            }
         }

         satisfactory(college, userInput1, map);
      }
      else if(userInput == "dfw")
      {
         //Ask user to enter dept name or for all depts
         cout << "dept name, or all? ";
         cin >> userInput1;

         //If user input is not "all", we push dept into map
         if(userInput1 != "all")
         {
            //Loop through all depts in the college.Depts vector
            for(const Dept& dept: college.Depts)
            {
               //Using dept's name as key, dept as value
               auto keyvaluepair = pair<string, Dept>(dept.Name, dept);
               map.insert(keyvaluepair);
            }
         }

         dfw(college, userInput1, map);
      }
      else if(userInput == "letterB")
      {
         //Ask user to enter dept name or for all depts
         cout << "dept name, or all? ";
         cin >> userInput1;

         //If user input is not "all", we push dept into map
         if(userInput1 != "all")
         {
            //Loop through all depts in the college.Depts vector
            for(const Dept& dept: college.Depts)
            {
               //Using dept's name as key, dept as value
               auto keyvaluepair = pair<string, Dept>(dept.Name, dept);
               map.insert(keyvaluepair);
            }
         }

         letterB(college, userInput1, map);
      }
      else if(userInput == "average")
      {
         //Ask user to enter dept name or for all depts
         cout << "dept name, or all? ";
         cin >> userInput1;

         //If user input is not "all", we push dept into map
         if(userInput1 != "all")
         {
            //Loop through all depts in the college.Depts vector
            for(const Dept& dept: college.Depts)
            {
               //Using dept's name as key, dept as value
               auto keyvaluepair = pair<string, Dept>(dept.Name, dept);
               map.insert(keyvaluepair);
            }
         }

         average(college);
      }
      else
      {
         //Command is not valid, print out the error message
         cout << "**unknown command" << endl;
      }

      cout << "Enter a command> ";
      cin >> userInput;
   }
}


void summary(const College& college, string userInput, map<string, Dept>& map)
{
   int DFW, N;

   if(userInput == "all")
   {
      //Push Dept into map
      for(const Dept& dept: college.Depts)
      {
         auto keyvaluepair = pair<string, Dept>(dept.Name, dept);
         map.insert(keyvaluepair);
      }

      for(auto keyvaluepair : map)
      {
         double DFWRate = GetDFWRate(keyvaluepair.second, DFW, N);

         GradeStats gradeStats = GetGradeDistribution(keyvaluepair.second);

         cout << keyvaluepair.first << ":" << endl;
         cout << " # courses taught: " << keyvaluepair.second.NumCourses() << endl;
         cout << " # students taught: " << keyvaluepair.second.NumStudents() << endl;
         cout << " grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
              << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;
         cout << " DFW rate: " << DFWRate << "%" << endl;
      }
   }
   else
   {
      auto it = map.find(userInput);
      if(it == map.end())
      {
         cout << "**dept not found" << endl;

      }
      else
      {

         double DFWRate = GetDFWRate(it->second, DFW, N);

         GradeStats gradeStats = GetGradeDistribution(it->second);

         cout << userInput << ":" << endl;
         cout << " # courses taught: " << it->second.NumCourses() << endl;
         cout << " # students taught: " << it->second.NumStudents() << endl;
         cout << " grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
         << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;
         cout << " DFW rate: " << DFWRate << "%" << endl;
      }
   }
}


void search(const College& college, string userInput, map<string, Dept>& map)
{
   string gradingType, userInput1;
   int DFW, N, courseNum;

   vector<Course> course;
   //map<string, Dept> map;

   if(userInput == "all")
   {
      cout << "course # or instructor prefix? ";
      cin >> userInput1;

      stringstream ss(userInput1);  // create stringstream object
      ss >> courseNum;

      if (ss.fail())  // conversion failed, input is not numeric
         course = FindCourses(college, userInput1);
      else            // conversion worked, courseNum contains numeric value
         course = FindCourses(college, courseNum);

      if(course.size() == 0)
      {
         cout << "**none found" << endl;
      }

      for(auto c : course)
      {
         double DFWRate = GetDFWRate(c, DFW, N);

         GradeStats gradeStats = GetGradeDistribution(c);

         if(c.getGradingType() == 0)
         {
            gradingType = "letter";

         }
         else if(c.getGradingType() == 1)
         {
            gradingType = "satisfactory";
         }
         else
         {
            gradingType = "unknown";
         }

         cout << c.Dept << " " << c.Number << " (section " << c.Section << "): " << c.Instructor << endl;
         cout << " # students: " << c.getNumStudents() << endl;
         cout << " course type: " << gradingType << endl;
         cout << " grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
              << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;
         cout << " DFW rate: " << DFWRate << "%" << endl;
      }
   }
   else
   {
      cout << "course # or instructor prefix? ";
      cin >> userInput1;

      auto it = map.find(userInput);
      if(it == map.end())
      {
         cout << "**dept not found" << endl;
      }
      else
      {

         stringstream ss(userInput1);  // create stringstream object
         ss >> courseNum;

         if (ss.fail())  // conversion failed, input is not numeric
         {
            for(auto dept : college.Depts)
            {
               if(dept.Name == userInput)
               {
                  Dept d;
                  d = dept;

                  course = FindCourses(d, userInput1);

                  if(course.size() == 0)
                  {
                     cout << "**none found" << endl;
                  }
                  else
                  {
                     for(auto c : course)
                     {
                        double DFWRate = GetDFWRate(c, DFW, N);

                        GradeStats gradeStats = GetGradeDistribution(c);

                        if(c.getGradingType() == 0)
                        {
                           gradingType = "letter";
                        }
                        else if(c.getGradingType() == 1)
                        {
                           gradingType = "satisfactory";
                        }
                        else
                        {
                           gradingType = "unknown";
                        }

                        cout << c.Dept << " " << c.Number << " (section " << c.Section << "): " << c.Instructor << endl;
                        cout << " # students: " << c.getNumStudents() << endl;
                        cout << " course type: " << gradingType << endl;
                        cout << " grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
                        << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;
                        cout << " DFW rate: " << DFWRate << "%" << endl;
                     }
                  }
               }
            }
         }
         else            // conversion worked, courseNum contains numeric value
         {
            for(auto dept : college.Depts)
            {
               if(dept.Name == userInput)
               {
                  Dept d = dept;

                  course = FindCourses(d, courseNum);

                  if(course.size() == 0)
                     cout << "**none found" << endl;
                  else
                  {
                     for(auto c : course)
                     {
                        double DFWRate = GetDFWRate(c, DFW, N);

                        GradeStats gradeStats = GetGradeDistribution(c);

                        if(c.getGradingType() == 0)
                        {
                           gradingType = "letter";
                        }
                        else if(c.getGradingType() == 1)
                        {
                           gradingType = "satisfactory";
                        }
                        else
                        {
                           gradingType = "unknown";
                        }

                        cout << c.Dept << " " << c.Number << " (section " << c.Section << "): " << c.Instructor << endl;
                        cout << " # students: " << c.getNumStudents() << endl;
                        cout << " course type: " << gradingType << endl;
                        cout << " grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
                        << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;
                        cout << " DFW rate: " << DFWRate << "%" << endl;
                     }
                  }
               }
            }
         }
      }
   }
}


void satisfactory(const College& college, string userInput, map<string, Dept>& map)
{
   string gradingType = "satisfactory";

   vector<Course> c;

   if(userInput == "all")
   {
      for(auto dept : college.Depts)
      {
         for(auto course : dept.Courses)
         {
            if(course.getGradingType() == 1)
               c.push_back(course);
         }
      }
      if(c.size() == 0)
      {
         cout << "**none found" << endl;
      }

      //sort the course
      if (c.size() > 1)
      {
         sort(c.begin(), c.end(),
              [](const Course& c1, const Course& c2)
              {
            if (c1.Dept < c2.Dept)
               return true;
            else if (c1.Dept > c2.Dept)
               return false;
            else // same dept, look at course #:
               if (c1.Number < c2.Number)
                  return true;
               else if (c1.Number > c2.Number)
                  return false;
               else // same course #, look at section #:
                  if (c1.Section < c2.Section)
                     return true;
                  else
                     return false;
         }
              );
      }

      for(auto it : c)
      {
         cout << it.Dept << " " << it.Number << " (section "
         << it.Section << "): " << it.Instructor << endl;
         cout << " # students: " << it.getNumStudents() << endl;
         cout << " course type: " << gradingType << endl;
      }
   }
   else
   {
      auto it = map.find(userInput);
      if(it == map.end())
      {
         cout << "**dept not found" << endl;
      }
      else
      {

         for(auto dept : college.Depts)
         {
            if(dept.Name == userInput)
            {
               Dept d;
               d = dept;

               for(auto course : d.Courses)
               {
                  if(course.getGradingType() == 1)
                     c.push_back(course);
               }
               if(c.size() == 0)
               {
                  cout << "**none found" << endl;
               }

               //Sort the course
               if (c.size() > 1)  // not required, just a tiny optimization:
               {
                  sort(c.begin(), c.end(),
                       [](const Course& c1, const Course& c2)
                       {
                     if (c1.Number < c2.Number)
                        return true;
                     else if (c1.Number > c2.Number)
                        return false;
                     else // equal, determine by section number:
                        if (c1.Section < c2.Section)
                           return true;
                        else
                           return false;
                  }
                       );
               }

               for(auto it : c)
               {
                  cout << it.Dept << " " << it.Number << " (section "
                  << it.Section << "): " << it.Instructor << endl;
                  cout << " # students: " << it.getNumStudents() << endl;
                  cout << " course type: " << gradingType << endl;
               }
            }
         }
      }
   }
}


void dfw(const College& college, string userInput, map<string, Dept>& map)  //NOT DONE
{
   string gradingType;
   int DFW, N;
   double threshold;

   vector<Course> c;

   if(userInput == "all")
   {
      cout << "dfw threshold? ";
      cin >> threshold;

      for(auto dept : college.Depts)
      {
         for(auto course : dept.Courses)
         {
            if(GetDFWRate(course, DFW, N) > threshold)
            {
               c.push_back(course);
            }
         }
      }
      if(c.size() == 0)
      {
         cout << "**none found" << endl;
      }
            //Sort the course
            if (c.size() > 1)
            {
               sort(c.begin(), c.end(),
                    [&](const Course& c1, const Course& c2)
                    {
                  if(GetDFWRate(c1, DFW, N) > GetDFWRate(c2, DFW, N))
                     return true;
                  else if(GetDFWRate(c1, DFW, N) < GetDFWRate(c2, DFW, N))
                     return false;
                  else
                  {
                     if (c1.Dept < c2.Dept)
                        return true;
                     else if (c1.Dept > c2.Dept)
                        return false;
                     else // same dept, look at course #:
                        if (c1.Number < c2.Number)
                           return true;
                        else if (c1.Number > c2.Number)
                           return false;
                        else // same course #, look at section #:
                           if (c1.Section < c2.Section)
                              return true;
                           else
                              return false;
                  }
               }
                    );
            }

            for(auto it : c)
            {
               double DFWRate = GetDFWRate(it, DFW, N);

               GradeStats gradeStats = GetGradeDistribution(it);

               if(it.getGradingType() == 0)
               {
                  gradingType = "letter";
               }
               else if(it.getGradingType() == 1)
               {
                  gradingType = "satisfactory";
               }
               else
               {
                  gradingType = "unknown";
               }

               cout << it.Dept << " " << it.Number << " (section "
                    << it.Section << "): " << it.Instructor << endl;
               cout << " # students: " << it.getNumStudents() << endl;
               cout << " course type: " << gradingType << endl;
               cout << " grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
               << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;
               cout << " DFW rate: " << DFWRate << "%" << endl;
            }
         }
   else
   {
      cout << "dfw threshold? ";
      cin >> threshold;

      auto it = map.find(userInput);
      if(it == map.end())
      {
         cout << "**dept not found" << endl;
      }
      else
      {

         for(auto dept : college.Depts)
         {
            if(dept.Name == userInput)
            {
               Dept d = dept;

               for(auto course : d.Courses)
               {
                  if(GetDFWRate(course, DFW, N) > threshold)
                     c.push_back(course);
               }
            }
         }

         if(c.size() == 0)
         {
            cout << "**none found" << endl;
         }
         //Sort the course
         if (c.size() > 1)
         {
            sort(c.begin(), c.end(),
                 [&](const Course& c1, const Course& c2)
                 {
               if(GetDFWRate(c1, DFW, N) > GetDFWRate(c2, DFW, N))
                  return true;
               else if(GetDFWRate(c1, DFW, N) < GetDFWRate(c2, DFW, N))
                  return false;
               else
               {
                  if (c1.Dept < c2.Dept)
                     return true;
                  else if (c1.Dept > c2.Dept)
                     return false;
                  else // same dept, look at course #:
                     if (c1.Number < c2.Number)
                        return true;
                     else if (c1.Number > c2.Number)
                        return false;
                     else // same course #, look at section #:
                        if (c1.Section < c2.Section)
                           return true;
                        else
                           return false;
               }
            }
                 );
         }

         for(auto it : c)
         {
            double DFWRate = GetDFWRate(it, DFW, N);

            GradeStats gradeStats = GetGradeDistribution(it);

            if(it.getGradingType() == 0)
            {
               gradingType = "letter";
            }
            else if(it.getGradingType() == 1)
            {
               gradingType = "satisfactory";
            }
            else
            {
               gradingType = "unknown";
            }

            cout << it.Dept << " " << it.Number << " (section "
            << it.Section << "): " << it.Instructor << endl;
            cout << " # students: " << it.getNumStudents() << endl;
            cout << " course type: " << gradingType << endl;
            cout << " grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
            << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;
            cout << " DFW rate: " << DFWRate << "%" << endl;
         }
      }
   }
}


void letterB(const College& college, string userInput, map<string, Dept>& map)  //NOT DONE
{
   string gradingType;
   int DFW, N;
   double threshold;
   //GradeStats percentB;

   vector<Course> c;

   if(userInput == "all")
   {
      cout << "letter B threshold? ";
       cin >> threshold;

      for(auto dept : college.Depts)
      {
         for(auto course : dept.Courses)
         {
            if(GetGradeDistribution(course).PercentB > threshold)
            {
               c.push_back(course);
            }
         }
      }
      if(c.size() == 0)
      {
         cout << "**none found" << endl;
      }
            //Sort the course
            if (c.size() > 1)
            {
               sort(c.begin(), c.end(),
                    [&](const Course& c1, const Course& c2)
                    {
                  if(GetGradeDistribution(c1).PercentB > GetGradeDistribution(c2).PercentB)
                     return true;
                  else if(GetGradeDistribution(c1).PercentB < GetGradeDistribution(c2).PercentB)
                     return false;
                  else
                  {
                     if (c1.Dept < c2.Dept)
                        return true;
                     else if (c1.Dept > c2.Dept)
                        return false;
                     else // same dept, look at course #:
                        if (c1.Number < c2.Number)
                           return true;
                        else if (c1.Number > c2.Number)
                           return false;
                        else // same course #, look at section #:
                           if (c1.Section < c2.Section)
                              return true;
                           else
                              return false;
                  }
               }
                    );
            }

            for(auto it : c)
            {
               double DFWRate = GetDFWRate(it, DFW, N);

               GradeStats gradeStats = GetGradeDistribution(it);

               if(it.getGradingType() == 0)
               {
                  gradingType = "letter";
               }
               else if(it.getGradingType() == 1)
               {
                  gradingType = "satisfactory";
               }
               else
               {
                  gradingType = "unknown";
               }

               cout << it.Dept << " " << it.Number << " (section "
               << it.Section << "): " << it.Instructor << endl;
               cout << " # students: " << it.getNumStudents() << endl;
               cout << " course type: " << gradingType << endl;
               cout << " grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
               << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;
               cout << " DFW rate: " << DFWRate << "%" << endl;
            }
   }
   else
   {
      cout << "letter B threshold? ";
      cin >> threshold;
      auto it = map.find(userInput);
      if(it == map.end())
      {
         cout << "**dept not found" << endl;
      }
      else
      {

         for(auto dept : college.Depts)
         {
            if(dept.Name == userInput)
            {
               Dept d = dept;

               for(auto course : d.Courses)
               {
                  if(GetGradeDistribution(course).PercentB > threshold)
                     c.push_back(course);
               }
            }
         }
         if(c.size() == 0)
         {
            cout << "**none found" << endl;
         }

         //Sort the course
         if (c.size() > 1)
         {
            sort(c.begin(), c.end(),
                 [&](const Course& c1, const Course& c2)
                 {
               if(GetGradeDistribution(c1).PercentB > GetGradeDistribution(c2).PercentB)
                  return true;
               else if(GetGradeDistribution(c1).PercentB < GetGradeDistribution(c2).PercentB)
                  return false;
               else
               {
                  if (c1.Dept < c2.Dept)
                     return true;
                  else if (c1.Dept > c2.Dept)
                     return false;
                  else // same dept, look at course #:
                     if (c1.Number < c2.Number)
                        return true;
                     else if (c1.Number > c2.Number)
                        return false;
                     else // same course #, look at section #:
                        if (c1.Section < c2.Section)
                           return true;
                        else
                           return false;
               }
            }
                 );
         }

         for(auto it : c)
         {
            double DFWRate = GetDFWRate(it, DFW, N);

            GradeStats gradeStats = GetGradeDistribution(it);

            if(it.getGradingType() == 0)
            {
               gradingType = "letter";
            }
            else if(it.getGradingType() == 1)
            {
               gradingType = "satisfactory";
            }
            else
            {
               gradingType = "unknown";
            }

            cout << it.Dept << " " << it.Number << " (section "
            << it.Section << "): " << it.Instructor << endl;
            cout << " # students: " << it.getNumStudents() << endl;
            cout << " course type: " << gradingType << endl;
            cout << " grade distribution (A-F): " << gradeStats.PercentA << "%, " << gradeStats.PercentB << "%, "
            << gradeStats.PercentC << "%, " << gradeStats.PercentD << "%, " << gradeStats.PercentF << "%" << endl;
            cout << " DFW rate: " << DFWRate << "%" << endl;
         }
      }
   }
}


void average(const College& college)
{

}



//
// API:
//

//
// ParseCourse:
//
// Parses a CSV (comma-separated values) line into a Course
// object, which is then returned.  The given line must have
// the following format:
//
//   Dept,Number,Section,Title,A,B,C,D,F,I,NR,S,U,W,Instructor
//
// Example:
//   BIOE,101,01,Intro to Bioengineering,22,8,2,1,0,1,0,0,0,5,Eddington
//
// Note the lack of spaces, except perhaps in the title.
// If the given line does not have this format, the behavior
// of the function is undefined (it may crash, it may throw
// an exception, it may return).
// You are probably familiar with A-F, the other letters are as follows
// I ñ Incomplete, grading has been deferred
// NR ñ No report, the grade has not been reported
// S ñ Satisfactory, passing in a course that does not use letter grades
// U ñ Unsastifactory, failing in a course that does not use letter grades
// W ñ Withdrawn ñ student withdrew from the course so no grade could be assigned
//
Course ParseCourse(string csvline)
{
   Course  course;
   string  value;

   stringstream  ss(csvline);

   // format: Dept,Number,Section,Title,A,B,C,D,F,I,NR,S,U,W,Instructor

   getline(ss, course.Dept, ',');

   getline(ss, value, ',');
   course.Number = stoi(value);
   getline(ss, value, ',');
   course.Section = stoi(value);

   getline(ss, course.Title, ',');

   getline(ss, value, ',');
   course.NumA = stoi(value);
   getline(ss, value, ',');
   course.NumB = stoi(value);
   getline(ss, value, ',');
   course.NumC = stoi(value);
   getline(ss, value, ',');
   course.NumD = stoi(value);
   getline(ss, value, ',');
   course.NumF = stoi(value);

   getline(ss, value, ',');
   course.NumI = stoi(value);
   getline(ss, value, ',');
   course.NumNR = stoi(value);
   getline(ss, value, ',');
   course.NumS = stoi(value);
   getline(ss, value, ',');
   course.NumU = stoi(value);
   getline(ss, value, ',');
   course.NumW = stoi(value);

   getline(ss, course.Instructor, ',');

   //
   // done, return course object:
   //
   return course;
}


//
// GetDFWRate:
//
// Returns the DFW rate as a percentage for a given course,
// department, or college.  For a course whose grading type
// is defined as Course::Letter, the DFW rate is defined as
//
//   # of D grades + F grades + Withdrawals
//   -------------------------------------- * 100.0
//   # of A, B, C, D, F grades + Withdrawls
//
// The numerator is returned via the reference parameter DFW;
// the denominator is returned via the reference parameter N.
// If the course grading type is not Course::Letter, the DFW
// rate is 0.0, and parameters DFW and N are set to 0.
//
// When computed for a dept or college, all courses of type
// Course::Letter are considered in computing an overall DFW
// rate for the dept or college.  The reference parameters
// DFW and N are also computed across the dept or college.
//
double GetDFWRate(const Course& c, int& DFW, int& N)
{
   DFW = 0;
   N   = 0;

   //
   // how was course graded?  If not a letter, then cannot compute
   // DFW rate, so return 0.0:
   //
   Course::GradingType grading = c.getGradingType();

   if (grading != Course::Letter)
      return 0.0;

   //
   // we have letter grades (at least 1), so compute DFW rate:
   //
   DFW = c.NumD + c.NumF + c.NumW;
   N   = c.NumA + c.NumB + c.NumC + c.NumD + c.NumF + c.NumW;

   if (N == 0) // just in case, guard against divide by 0:
      return 0.0;

   return (static_cast<double>(DFW) / static_cast<double>(N)) * 100.0;
}

double GetDFWRate(const Dept& dept, int& DFW, int& N)
{
   DFW = 0;
   N = 0;
   int courseDFW;
   int courseN;

   for (const Course& c : dept.Courses)
   {
      GetDFWRate(c, courseDFW, courseN);
      // course had letter grades, so include in DFW calculation:
      DFW += courseDFW;
      N += courseN;
   }

   if (N == 0) // just in case, guard against divide by 0:
      return 0.0;

   return (static_cast<double>(DFW) / static_cast<double>(N)) * 100.0;
}

double GetDFWRate(const College& college, int& DFW, int& N)
{
   DFW = 0;
   N = 0;
   int courseDFW;
   int courseN;

   for (const Dept& dept : college.Depts)
   {
      GetDFWRate(dept, courseDFW, courseN);
      // course had letter grades, so include in DFW calculation:
      DFW += courseDFW;
      N += courseN;
   }

   if (N == 0) // just in case, guard against divide by 0:
      return 0.0;

   return (static_cast<double>(DFW) / static_cast<double>(N)) * 100.0;
}


//
// GetGradeDistribution
//
// Returns an object containing the grade distribution for a given
// course, dept or college.  For a course whose grading type is
// defined as Course::Letter, the grade distribution is defined by
// the following values:
//
//   N: the # of A, B, C, D, F grades
//   NumA, NumB, NumC, NumD, NumF: # of A, B, C, D, F grades
//   PercentA, PercentB, PercentC, PercentD, PercentF: % of A, B,
//     C, D, F grades.  Example: PercentA = NumA / N * 100.0
//
// If the course grading type is not Course::Letter, all values
// are 0.  When computed for a dept or college, all courses of
// type Course::Letter are considered in computing an overall
// grade distribution for the dept or college.
//
GradeStats GetGradeDistribution(const Course& c)
{
   GradeStats stats;

   //
   // how was course graded?  If not a letter, then cannot compute
   // grade distributions:
   //
   Course::GradingType grading = c.getGradingType();

   if (grading != Course::Letter)
      return stats;

   //
   // we have letter grades (at least 1), so compute distribution:
   //
   stats.N = c.NumA + c.NumB + c.NumC + c.NumD + c.NumF;

   if (stats.N == 0) // just in case, guard against divide by 0:
      return stats;

   stats.NumA = c.NumA;
   stats.PercentA = (static_cast<double>(stats.NumA) / static_cast<double>(stats.N)) * 100.0;

   stats.NumB = c.NumB;
   stats.PercentB = (static_cast<double>(stats.NumB) / static_cast<double>(stats.N)) * 100.0;

   stats.NumC = c.NumC;
   stats.PercentC = (static_cast<double>(stats.NumC) / static_cast<double>(stats.N)) * 100.0;

   stats.NumD = c.NumD;
   stats.PercentD = (static_cast<double>(stats.NumD) / static_cast<double>(stats.N)) * 100.0;

   stats.NumF = c.NumF;
   stats.PercentF = (static_cast<double>(stats.NumF) / static_cast<double>(stats.N)) * 100.0;

   return stats;
}

GradeStats GetGradeDistribution(const Dept& dept)
{
   //
   // let's collect all the data in a course object, then we can call
   // getGradeDistribution to do the work:
   //
   Course  all;

   //
   // loop through courses in dept, accumulating the grades:
   //
   for (const Course& c : dept.Courses)
   {
      if (c.getGradingType() != Course::Letter)
         continue;  // skip

      // course had letter grades, so include in computation:
      all.NumA += c.NumA;
      all.NumB += c.NumB;
      all.NumC += c.NumC;
      all.NumD += c.NumD;
      all.NumF += c.NumF;
   }

   //
   // and compute:
   //
   return GetGradeDistribution(all);
}

GradeStats GetGradeDistribution(const College& college)
{
   //
   // let's collect all the data in a course object, then we can call
   // getGradeDistribution to do the work:
   //
   Course  all;

   //
   // loop through every dept in the college, and for every dept, loop
   // through the courses and accumulate all the grades:
   //
   for (const Dept& dept : college.Depts)
   {
      for (const Course& c : dept.Courses)
      {
         if (c.getGradingType() != Course::Letter)
            continue;  // skip

         // course had letter grades, so include in computation:
         all.NumA += c.NumA;
         all.NumB += c.NumB;
         all.NumC += c.NumC;
         all.NumD += c.NumD;
         all.NumF += c.NumF;
      }
   }

   //
   // and compute:
   //
   return GetGradeDistribution(all);
}


//
// FindCourses(dept, courseNumber)
//
// Searches the courses in the department for those that match
// the given course number.  If none are found, then the returned
// vector is empty.  If one or more courses are found, copies of
// the course objects are returned in a vector, with the courses
// appearing in ascending order by section number.
//
vector<Course> FindCourses(const Dept& dept, int courseNumber)
{
   vector<Course>  courses;

   //
   // looking for 1 or more courses that match the course number:
   //
   for(const Course& course : dept.Courses)
   {
      if (course.Number == courseNumber)  // match:
      {
         courses.push_back(course);
      }
   }

   //
   // sort the vector, first by course number then by section number:
   //
   if (courses.size() > 1)  // not required, just a tiny optimization:
   {
      sort(courses.begin(), courses.end(),
           [](const Course& c1, const Course& c2)
           {
              return (c1.Section < c2.Section);
           }
           );
   }

   return courses;
}


//
// FindCourses(dept, instructorPrefix)
//
// Searches the courses in the department for those whose
// instructor name starts with the given instructor prefix.
// For example, the prefix "Re" would match instructors "Reed"
// and "Reynolds".
//
// If none are found, then the returned vector is empty.  If
// one or more courses are found, copies of the course objects
// are returned in a vector, with the courses appearing in
// ascending order by course number.  If two courses have the
// same course number, they are given in ascending order by
// section number.  Note that courses are NOT sorted by instructor
// name.
//
vector<Course> FindCourses(const Dept& dept, string instructorPrefix)
{
   vector<Course>  courses;

   //
   // looking for 1 or more courses that match --- or start with ---
   // the given instrutor name:
   //
   size_t len = instructorPrefix.length();

   for(const Course& course : dept.Courses)
   {
      if (course.Instructor.substr(0, len) == instructorPrefix)  // match:
      {
         courses.push_back(course);
      }
   }

   //
   // sort the vector, first by course number then by section number:
   //
   if (courses.size() > 1)  // not required, just a tiny optimization:
   {
      sort(courses.begin(), courses.end(),
           [](const Course& c1, const Course& c2)
           {
              if (c1.Number < c2.Number)
                 return true;
              else if (c1.Number > c2.Number)
                 return false;
              else // equal, determine by section number:
                 if (c1.Section < c2.Section)
                    return true;
                 else
                    return false;
           }
           );
   }

   return courses;
}


//
// FindCourses(college, courseNumber)
//
// Searches for all courses in the college for those that match
// the given course number.  If none are found, then the returned
// vector is empty.  If one or more courses are found, copies of
// the course objects are returned in a vector, with the courses
// appearing in ascending order by department, then course number,
// and then section number.
//
vector<Course> FindCourses(const College& college, int courseNumber)
{
   vector<Course>  courses;

   //
   // For each college, search for courses that match and collect them all
   // in a single vector:
   //
   for(const Dept& dept : college.Depts)
   {
      vector<Course> onedept = FindCourses(dept, courseNumber);

      for(const Course& c : onedept)
      {
         courses.push_back(c);
      }
   }

   //
   // now sort the courses (if any) by dept, course #, and section #:
   //
   if (courses.size() > 1)  // not required, just a tiny optimization:
   {
      sort(courses.begin(), courses.end(),
           [](const Course& c1, const Course& c2)
           {
              if (c1.Dept < c2.Dept)
                 return true;
              else if (c1.Dept > c2.Dept)
                 return false;
              else // same dept, look at course #:
                 if (c1.Number < c2.Number)
                    return true;
                 else if (c1.Number > c2.Number)
                    return false;
                 else // same course #, look at section #:
                    if (c1.Section < c2.Section)
                       return true;
                    else
                       return false;
           }
           );
   }

   return courses;
}


//
// FindCourses(college, instructorPrefix)
//
// Searches all the courses in the college for those whose
// instructor name starts with the given instructor prefix.
// For example, the prefix "Re" would match instructors "Reed"
// and "Reynolds".  If none are found, then the returned
// vector is empty.  If one or more courses are found, copies of
// the course objects are returned in a vector, with the courses
// appearing in ascending order by department,
// then course number, and then section number.
//
vector<Course> FindCourses(const College& college, string instructorPrefix)
{
   vector<Course>  courses;

   //
   // For each college, search for courses that match and collect them all
   // in a single vector:
   //
   for(const Dept& dept : college.Depts)
   {
      vector<Course> onedept = FindCourses(dept, instructorPrefix);

      for(const Course& c : onedept)
      {
         courses.push_back(c);
      }
   }

   //
   // now sort the courses (if any) by dept, course #, and section #:
   //
   if (courses.size() > 1)
   {
      sort(courses.begin(), courses.end(),
           [](const Course& c1, const Course& c2)
           {
              if (c1.Dept < c2.Dept)
                 return true;
              else if (c1.Dept > c2.Dept)
                 return false;
              else // same dept, look at course #:
                 if (c1.Number < c2.Number)
                    return true;
                 else if (c1.Number > c2.Number)
                    return false;
                 else // same course #, look at section #:
                    if (c1.Section < c2.Section)
                       return true;
                    else
                       return false;
           }
           );
   }

   return courses;
}
