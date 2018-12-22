//constants.h
//this is where the global constants will go for access by all the classes.

#ifndef CONSTANTS_H
#define CONSTANTS_H

const int STDWALL = 2;
const int sigDigits = 10;
const double pi = 3.141592654;
const int NUMROOMS = 200;

const double BOXROOM_LENGTH_MIN		= 20;
const double BOXROOM_LENGTH_MAX		= 200;
const double BOXROOM_WIDTH_MIN		= 20;
const double BOXROOM_WIDTH_MAX		= 200;
const double BOXROOM_HEIGHT_MIN		= 20;
const double BOXROOM_HEIGHT_MAX		= 200;

//recommend that the sphere radii min and max be within 50% of one another
const double SPHERE_RADIUS_MIN		= 60;
const double SPHERE_RADIUS_MAX		= 120;


//recommend the cylinders have no less than 15 sides - otherwise use gengons
const double CYLINDER_SIDES_MIN		= 15;
const double CYLINDER_SIDES_MAX		= 40;
const double CYLINDER_HEIGHT_MIN	= 170;
const double CYLINDER_HEIGHT_MAX	= 300;
const double CYLINDER_RADIUS_MIN	= 50;
const double CYLINDER_RADIUS_MAX	= 200;

const double GENGON_SIDES_MIN		= 4;  //recommended no less than 4
const double GENGON_SIDES_MAX		= 7;
const double GENGON_RADIUS_MIN		= 50;
const double GENGON_RADIUS_MAX		= 200;
const double GENGON_FILLET_MIN		= 0;
const double GENGON_FILLET_MAX		= 10;
const double GENGON_HEIGHT_MIN		= 20;
const double GENGON_HEIGHT_MAX		= 200;

#endif