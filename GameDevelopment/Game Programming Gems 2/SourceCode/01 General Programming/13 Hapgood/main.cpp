/* Copyright (C) Bryon Hapgood, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bryon Hapgood, 2001"
 */
static void search(SAFE&safe,void*v){
    if(<meets_requirement>)
	SafeExit(safe);
    // do stuff
	search(safe,v);
    return;	
}

int main(){
    SAFE safe;
    SafeEnter(
	safe,
	search,
	<some_pointer>)
    ;
}
