/* Copyright (C) Mason McCuskey, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Mason McCuskey, 2000"
 */
//////////////////////////////////////////////////////////////////////////////
//
// fuzzy.cpp : A program that demonstrates simple fuzzy logic techniques.
// 
// DISCLAIMER: I wrote this using as little C++ as possible, my intention being
// to teach fuzzy logic, not C++ programming tricks.  A more robust implementation
// of fuzzy logic would probably be littered with try/catch blocks, C++ style casts,
// etc.  But in the interest of the lowest common denominator, I have ignored all
// that and reverted back to the old-school ways of doing things (fabs, 
// printf/scanf, etc.)
//
// Written By: Mason McCuskey, Spin Studios, mason@spin-studios.com
// Release   : 1.00 on 4/30/00
//
//////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#ifdef _WIN32
#include <conio.h>
#else
#include <f2c.h>
#endif

//////////////////////////////////////////////////////////////////////////////
//
// CFuzzyRule: A Simple class to encapsulate a fuzzy logic rule.
//
// Each fuzzy logic rule is composed of four points: 
// min 0%, min 100%, max 100%, and max 0%.  Together, these four points form
// a sloped graph, which starts out at min 0%, hits 100% at min100%, continues
// at 100% until max 100%, then tapers off, finally hitting 0% again at max
// 0%.  So, if you were to graph these on a x/y plane, with y ranging from
// 0 to 1 and x ranging from min0 to max0, you'd get a shape resembling a
// trapezoid (or a triangle, if min100% = max100%).
//
// Yes, this is a relatively simplistic way to represent a fuzzy set.  Real
// fuzzy set classes would be able to accurately model discrete points, 
// and could have any number of key points on them.
// 
// but, the more complex your graph of a fuzzy set, the more complex your
// calculations for trueness.  For the purposes of illustration, I've opted
// to go with what I feel is the best compromise between simplicity and
// flexibility.
//
//////////////////////////////////////////////////////////////////////////////
class CFuzzyRule
{
public:
  CFuzzyRule() { Init(); }
  CFuzzyRule(char *name, float min0, float min100, float max100, float max0) {
    Init();
    Setup(name, min0, min100, max100, max0);
  }

  void Setup(char *name, float min0, float min100, float max100, float max0) {
    Init();
    m_min0percent   = min0;
    m_min100percent = min100;
    m_max0percent   = max0;
    m_max100percent = max100;
    if (strlen(name) < sizeof(m_name)) { strcpy(m_name, name); }
  }
    
  ~CFuzzyRule() { }
  float IsTrueToWhatDegree(float datapoint);
  char *GetName(void) { return(m_name); }

private:
  void Init(void) { 
    m_min100percent = m_max100percent = m_min0percent = m_max0percent = 0.0f; 
    memset(m_name, 0, sizeof(m_name));
  }
  char  m_name[256];     // name of this fuzzy rule
  float m_min100percent; // smallest number considered 100% true
  float m_max100percent; // biggest number considered 100% true                                         
  float m_min0percent;   // smallest number still considered within fuzzy rule
  float m_max0percent;   // biggest number still considered within fuzzy rule
};

/****************************************************************************

 IsTrueToWhatDegree: determines to what degree a datapoint is "true" for this
 fuzzy rule.  A value of 1.0 means completely true, and a value of 0.0 means
 completely false.  Everything in between is "fuzzy."

 ****************************************************************************/
float CFuzzyRule::IsTrueToWhatDegree(float datapoint)
{
  // four possibilities here:
  // 1) it is completely outside the rule (0%)
  // 2) it is completely inside the rule (100%)
  // 3) it is partially inside the rule, on the min side
  // 4) it is partially inside the rule, on the max side

  if (datapoint < m_min0percent || datapoint > m_max0percent) {
    // it's completely outside the rule!
    return(0.0f);
  }

  if (datapoint >= m_min100percent && datapoint <= m_max100percent) {
    // it's completely true!
    return(1.0f);
  }

  if (datapoint >= m_min0percent && datapoint <= m_min100percent) {
    // it's partially true, on the minimum side.
    // evaluate the degree of "trueness."  note that this is a simple
    // "scale" evaluation for the purposes of example only; your real fuzzy
    // code would probably be more involved than this.

    // assuming m_min0percent is zero and m_min100percent is 1, figure out 
    // where datapoint is.

    // figure out the "width" of our "grey area."
    float widthofgreyarea = (float)fabs(m_min0percent - m_min100percent);

    // determine what percentage of that width is "behind" (<) datapoint.
    float relativedatapoint = datapoint-m_min0percent;

    return((float)fabs(relativedatapoint / widthofgreyarea));
  }

  // note: this is cut/pasted to demonstrate the logic flow more clearly.
  // eliminating this code duplication is left as an excercise for the reader. :)

  if (datapoint >= m_max100percent && datapoint <= m_max0percent) {
    // it's partially true, on the maximum side.
    // evaluate the degree of "trueness."  note that this is a simple
    // "scale" evaluation for the purposes of example only; your real fuzzy
    // code would probably be more involved than this.

    // assuming m_max0percent is zero and m_max100percent is 1, figure out 
    // where datapoint is.

    // figure out the "width" of our "grey area."
    float widthofgreyarea = (float)fabs(m_max0percent - m_max100percent);

    // determine what percentage of that width is "behind" (<) datapoint.
    float relativedatapoint = datapoint-m_max0percent;

    return((float)fabs(relativedatapoint / widthofgreyarea));
  }

  // we should never get here!
  assert(0);
  return(0);
}

/****************************************************************************

 FuzzyAND: ANDs two fuzzy datapoints / rules together.

 ****************************************************************************/
float FuzzyAND(CFuzzyRule &rule1, float data1, CFuzzyRule &rule2, float data2)
{
  // a fuzzy and operation is only as true as the minimum trueness of both pairs.
  // in other words, a fuzzy logic operation is only as true as its weakest trueness.
  float true1 = rule1.IsTrueToWhatDegree(data1);
  float true2 = rule2.IsTrueToWhatDegree(data2);

#ifdef _WIN32
  return(__min(true1, true2));
#else
  return(min(true1, true2));
#endif
}

/****************************************************************************

 main: program execution starts here.  prompts for distance and distance
 delta, then spits out correct course of action (speed up, slow down, etc.)

 ****************************************************************************/
int main(int argc, char* argv[])
{
  float distance = 0.0f;
  float distancedelta = 0.0f;

	printf("An Introduction to Fuzzy Logic, Sample Program\n");
  printf("Mason McCuskey, mason@spin-studios.com, www.spin-studios.com\n");
  printf("============================================================\n\n\n");

  printf("\nThis is a fuzzy logic simulator for a safe driver.  Given a");
  printf("\ndistance and distance delta, we wil calculate an appropriate");
  printf("\naction, using fuzzy control.\n\n"); 
  printf("\nEnter a value for distance, in car lengths: ");
  scanf("%f", &distance);
  printf("\nEnter a value for distance delta, from -1 to 1, with");
  printf("\n  -1 = shrinking fast, 1 = growing fast: ");
  scanf("%f", &distancedelta);

  printf("\n Here goes...");

  // set up our distance fuzzy rules.
  CFuzzyRule distancerule[5];

  distancerule[0].Setup("Very Small", -999999.0f, -999999.0f, 0.5f, 1.0f);
  distancerule[1].Setup("Small", 0.5f, 1.0f, 1.0f, 2.0f);
  distancerule[2].Setup("Perfect", 1.0f, 2.0f, 2.0f, 3.0f);
  distancerule[3].Setup("Big", 2.0f, 3.0f, 3.0f, 4.0f);
  distancerule[4].Setup("Very Big", 3.0, 5.0, 999999.0, 999999.0);

  CFuzzyRule distancedeltarule[5];

  distancedeltarule[0].Setup("Shrinking Fast", -999999.0f, -1.0f, -1.0f, -0.5f);
  distancedeltarule[1].Setup("Shrinking", -1.0f, -0.5f, -0.5f, 0.0f);
  distancedeltarule[2].Setup("Stable", -0.5f, 0.0f, 0.0f, 0.5f);
  distancedeltarule[3].Setup("Growing", 0.0f, 0.5f, 0.5f, 1.0f);
  distancedeltarule[4].Setup("Growing Fast", 0.5f, 1.0f, 1.0f, 999999.0f);

  printf("\nDetermining to what degree your datapoints belong to all fuzzy sets:\n");
  
  int loop;
  for (loop=0; loop < 5; loop++) {
    CFuzzyRule &currule = distancerule[loop];

    printf("\nDegree to which distance %.02f belongs to set \"%15s\": %.02f", distance, currule.GetName(),
      currule.IsTrueToWhatDegree(distance));
  }
  
  printf("\n");
  for (loop=0; loop < 5; loop++) {
    CFuzzyRule &currule = distancedeltarule[loop];

    printf("\nDegree to which distance delta %.02f belongs to set \"%15s\": %.02f", distancedelta, currule.GetName(),
      currule.IsTrueToWhatDegree(distancedelta));
  }

  printf("\nDetermining course of action...");

  // fill in our course of action matrix.  The action matrix is Table 1 in 
  // the Intro To Fuzzy Logic Gem.
  
  float actionmatrix[5][5];
  char actionmatrixname[5][5][32];
  
  // the available courses of action.
  char *brake_hard     = "Brake Hard!";
  char *slow_down      = "Slow Down!";
  char *maintain_speed = "Maintain Speed!";
  char *speed_up       = "Speed up!";
  char *floor_it       = "Floor it!";

  // for each course of action, the sum of all the scores of all spaces on the action matrix
  // which have that action.
  float actionscore[5];
  
  // set up the names for each space in the action matrix.  These names act as the
  // key values between the table, and the scores for each of the 5 possible actions.
  // Yes, we could have just as easily used a pointer to a float or something here.

  strcpy(actionmatrixname[0][0], brake_hard);
  strcpy(actionmatrixname[0][1], brake_hard);
  strcpy(actionmatrixname[0][2], slow_down);
  strcpy(actionmatrixname[0][3], slow_down);
  strcpy(actionmatrixname[0][4], maintain_speed);
  
  strcpy(actionmatrixname[1][0], brake_hard);
  strcpy(actionmatrixname[1][1], slow_down);
  strcpy(actionmatrixname[1][2], slow_down);
  strcpy(actionmatrixname[1][3], maintain_speed);
  strcpy(actionmatrixname[1][4], speed_up);
  
  strcpy(actionmatrixname[2][0], slow_down);
  strcpy(actionmatrixname[2][1], slow_down);
  strcpy(actionmatrixname[2][2], maintain_speed);
  strcpy(actionmatrixname[2][3], speed_up);
  strcpy(actionmatrixname[2][4], speed_up);

  strcpy(actionmatrixname[3][0], slow_down);
  strcpy(actionmatrixname[3][1], maintain_speed);
  strcpy(actionmatrixname[3][2], speed_up);
  strcpy(actionmatrixname[3][3], speed_up);
  strcpy(actionmatrixname[3][4], floor_it);

  strcpy(actionmatrixname[4][0], maintain_speed);
  strcpy(actionmatrixname[4][1], speed_up);
  strcpy(actionmatrixname[4][2], speed_up);
  strcpy(actionmatrixname[4][3], floor_it);
  strcpy(actionmatrixname[4][4], floor_it);
  
  // AND everything together to get the scores for each space on the action matrix
  int x;
  for (x=0; x < 5; x++) {
    for (int y=0; y < 5; y++) {
      actionmatrix[x][y] = FuzzyAND(distancerule[x], distance, distancedeltarule[y], distancedelta);
    }
  }

  printf("\nThe Action Matrix:");
  // print out the action matrix
  for (int y=0; y < 5; y++) {
    printf("\n%.02f  %.02f  %.02f  %.02f  %.02f", 
      actionmatrix[0][y], actionmatrix[1][y], actionmatrix[2][y], actionmatrix[3][y], actionmatrix[4][y]);
  }

  // clear the scores
  for (x=0; x < 5; x++) { actionscore[x] = 0; }

  // score each of the 5 available actions the name as the key:
  for (x=0; x < 5; x++) {
    for (int y=0; y < 5; y++) {
      int addtoindex = 0;
      if (!strcmp(actionmatrixname[x][y], brake_hard))     { addtoindex = 0; } 
      if (!strcmp(actionmatrixname[x][y], slow_down))      { addtoindex = 1; } 
      if (!strcmp(actionmatrixname[x][y], maintain_speed)) { addtoindex = 2; } 
      if (!strcmp(actionmatrixname[x][y], speed_up))       { addtoindex = 3; } 
      if (!strcmp(actionmatrixname[x][y], floor_it))       { addtoindex = 4; } 

      actionscore[addtoindex] += actionmatrix[x][y];
    }
  }

  printf("\nTotal Scores:");
  // print out each of the scores
  printf("\nTotal score for \"%15s\": %.02f", brake_hard,     actionscore[0]);
  printf("\nTotal score for \"%15s\": %.02f", slow_down,      actionscore[1]);
  printf("\nTotal score for \"%15s\": %.02f", maintain_speed, actionscore[2]);
  printf("\nTotal score for \"%15s\": %.02f", speed_up,       actionscore[3]);
  printf("\nTotal score for \"%15s\": %.02f", floor_it,       actionscore[4]);

  // print out the winning score (final course of action).
  // for the sake of this simple example, we de-fuzzify using the simplest of all methods:
  // the action with the highest score is the one we go with.
  // your app will probably need more advanced logic than this, like center-of-mass equations
  // or something.

  int maxindex = 0; float maxvalue = 0.0f;
  for (x=0; x < 5; x++) { if (actionscore[x] > maxvalue) { maxindex = x; maxvalue = actionscore[x]; } }

  switch(maxindex) {
    case 0: printf("\n\nFinal course of action: BRAKE HARD!"); break;
    case 1: printf("\n\nFinal course of action: Slow down!"); break;
    case 2: printf("\n\nFinal course of action: Maintain speed."); break;
    case 3: printf("\n\nFinal course of action: Speed up!"); break;
    case 4: printf("\n\nFinal course of action: FLOOR IT!"); break;
    default: assert(0); break;
  }

#ifdef _WIN32
  // this getch makes the console stay up in debug mode until you hit a key
  // it's a patch for MSVC's irritating "feature" of automatically closing 
  // the window when it's finished
  printf("\nPress a key to exit...");
  getch();
#else
  printf("\n");
#endif
	return 0;
}

