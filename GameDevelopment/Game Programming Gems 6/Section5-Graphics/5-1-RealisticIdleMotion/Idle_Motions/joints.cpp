#include "joints.h"

using namespace pcjoints;


jointTp pcjoints::getJointType(const std::string& jointName) {
	if (jointName == "root")
		return root;
	else if (jointName =="l_hip")
		return l_hip;
	else if (jointName =="l_knee")
		return l_knee;
	else if (jointName =="l_ankle")
		return l_ankle;
	else if (jointName =="l_subtalar")
		return l_subtalar;
	else if (jointName =="r_hip")
		return r_hip;
	else if (jointName =="r_knee")
		return r_knee;
	else if (jointName =="r_ankle")
		return r_ankle;
	else if (jointName =="r_subtalar")
		return r_subtalar;
	else if (jointName =="vl5")
		return vl5;
	else if (jointName =="vl3")
		return vl3;
	else if (jointName =="vl1")
		return vl1;
	else if (jointName =="vt10")
		return vt10;
	else if (jointName =="vt1")
		return vt1;
	else if (jointName =="vc4")
		return vc4;
	else if (jointName =="vc2")
		return vc2;
	else if (jointName =="skullbase")
		return skullbase;
	else if (jointName =="l_sternoclavicular")
		return l_sternoclavicular;
	else if (jointName =="l_shoulder")
		return l_shoulder;
	else if (jointName =="l_elbow")
		return l_elbow;
	else if (jointName =="l_wrist")
		return l_wrist;
	else if (jointName =="r_sternoclavicular")
		return r_sternoclavicular;
	else if (jointName =="r_shoulder")
		return r_shoulder;
	else if (jointName =="r_elbow")
		return r_elbow;
	else if (jointName =="r_wrist")
		return r_wrist;
	else if (jointName =="sacroiliac")
		return sacroiliac;
	else if (jointName =="l_midtarsal")
		return l_midtarsal;
	else if (jointName =="l_metatarsal")
		return l_metatarsal;
	else if (jointName =="r_midtarsal")
		return r_midtarsal;
	else if (jointName =="r_metatarsal")
		return r_metatarsal;
	else if (jointName =="vl4")
		return vl4;
	else if (jointName =="vl2")
		return vl2;
	else if (jointName =="vt12")
		return vt12;
	else if (jointName =="vt11")
		return vt11;
	else if (jointName =="vt9")
		return vt9;
	else if (jointName =="vt8")
		return vt8;
	else if (jointName =="vt7")
		return vt7;
	else if (jointName =="vt6")
		return vt6;
	else if (jointName =="vt5")
		return vt5;
	else if (jointName =="vt4")
		return vt4;
	else if (jointName =="vt3")
		return vt3;
	else if (jointName =="vt2")
		return vt2;
	else if (jointName =="vc7")
		return vc7;
	else if (jointName =="vc6")
		return vc6;
	else if (jointName =="vc5")
		return vc5;
	else if (jointName =="vc3")
		return vc3;
	else if (jointName =="vc1")
		return vc1;
	else if (jointName =="l_eyelid_joint")
		return l_eyelid_joint;
	else if (jointName =="r_eyelid_joint")
		return r_eyelid_joint;
	else if (jointName =="l_eyeball_joint")
		return l_eyeball_joint;
	else if (jointName =="r_eyeball_joint")
		return r_eyeball_joint;
	else if (jointName =="l_eyebrow_joint")
		return l_eyebrow_joint;
	else if (jointName =="r_eyebrow_joint")
		return r_eyebrow_joint;
	else if (jointName =="temporomandibular")
		return temporomandibular;
	else if (jointName =="l_acromioclavicular")
		return l_acromioclavicular;
	else if (jointName =="r_acromioclavicular")
		return r_acromioclavicular;
	else if (jointName =="l_thumb1")
		return l_thumb1;
	else if (jointName =="l_thumb2")
		return l_thumb2;
	else if (jointName =="l_thumb3")
		return l_thumb3;
	else if (jointName =="l_index0")
		return l_index0;
	else if (jointName =="l_index1")
		return l_index1;
	else if (jointName =="l_index2")
		return l_index2;
	else if (jointName =="l_index3")
		return l_index3;
	else if (jointName =="l_middle0")
		return l_middle0;
	else if (jointName =="l_middle1")
		return l_middle1;
	else if (jointName =="l_middle2")
		return l_middle2;
	else if (jointName =="l_middle3")
		return l_middle3;
	else if (jointName =="l_ring0")
		return l_ring0;
	else if (jointName =="l_ring1")
		return l_ring1;
	else if (jointName =="l_ring2")
		return l_ring2;
	else if (jointName =="l_ring3")
		return l_ring3;
	else if (jointName =="l_pinky0")
		return l_pinky0;
	else if (jointName =="l_pinky1")
		return l_pinky1;
	else if (jointName =="l_pinky2")
		return l_pinky2;
	else if (jointName =="l_pinky3")
		return l_pinky3;
	else if (jointName =="r_thumb1")
		return r_thumb1;
	else if (jointName =="r_thumb2")
		return r_thumb2;
	else if (jointName =="r_thumb3")
		return r_thumb3;
	else if (jointName =="r_index0")
		return r_index0;
	else if (jointName =="r_index1")
		return r_index1;
	else if (jointName =="r_index2")
		return r_index2;
	else if (jointName =="r_index3")
		return r_index3;
	else if (jointName =="r_middle0")
		return r_middle0;
	else if (jointName =="r_middle1")
		return r_middle1;
	else if (jointName =="r_middle2")
		return r_middle2;
	else if (jointName =="r_middle3")
		return r_middle3;
	else if (jointName =="r_ring0")
		return r_ring0;
	else if (jointName =="r_ring1")
		return r_ring1;
	else if (jointName =="r_ring2")
		return r_ring2;
	else if (jointName =="r_ring3")
		return r_ring3;
	else if (jointName =="r_pinky0")
		return r_pinky0;
	else if (jointName =="r_pinky1")
		return r_pinky1;
	else if (jointName =="r_pinky2")
		return r_pinky2;
	else if (jointName =="r_pinky3")
		return r_pinky3;
	else
		return unknown_joint;
  
}

std::string pcjoints::getJointString(const jointTp& joint) {

	switch(joint) {
		case root: return "root"; break;
		case l_hip: return "l_hip"; break;
		case l_knee: return "l_knee"; break;
		case l_ankle: return "l_ankle"; break;
		case l_subtalar: return "l_subtalar"; break;
		case r_hip: return "r_hip"; break;
		case r_knee: return "r_knee"; break;
		case r_ankle: return "r_ankle"; break;
		case r_subtalar: return "r_subtalar"; break;
		case vl5: return "vl5"; break;
		case vl3: return "vl3"; break;
		case vl1: return "vl1"; break;
		case vt10: return "vt10"; break;
		case vt1: return "vt1"; break;
		case vc4: return "vc4"; break;
		case vc2: return "vc2"; break;
		case skullbase: return "skullbase"; break;
		case l_sternoclavicular: return "l_sternoclavicular"; break;
		case l_shoulder: return "l_shoulder"; break;
		case l_elbow: return "l_elbow"; break;
		case l_wrist: return "l_wrist"; break;
		case r_sternoclavicular: return "r_sternoclavicular"; break;
		case r_shoulder: return "r_shoulder"; break;
		case r_elbow: return "r_elbow"; break;
		case r_wrist: return "r_wrist"; break;
		case sacroiliac: return "sacroiliac"; break;
		case l_midtarsal: return "l_midtarsal"; break;
		case l_metatarsal: return "l_metatarsal"; break;
		case r_midtarsal: return "r_midtarsal"; break;
		case r_metatarsal: return "r_metatarsal"; break;
		case vl4: return "vl4"; break;
		case vl2: return "vl2"; break;
		case vt12: return "vt12"; break;
		case vt11: return "vt11"; break;
		case vt9: return "vt9"; break;
		case vt8: return "vt8"; break;
		case vt7: return "vt7"; break;
		case vt6: return "vt6"; break;
		case vt5: return "vt5"; break;
		case vt4: return "vt4"; break;
		case vt3: return "vt3"; break;
		case vt2: return "vt2"; break;
		case vc7: return "vc7"; break;
		case vc6: return "vc6"; break;
		case vc5: return "vc5"; break;
		case vc3: return "vc3"; break;
		case vc1: return "vc1"; break;
		case l_eyelid_joint: return "l_eyelid_joint"; break;
		case r_eyelid_joint: return "r_eyelid_joint"; break;
		case l_eyeball_joint: return "l_eyeball_joint"; break;
		case r_eyeball_joint: return "r_eyeball_joint"; break;
		case l_eyebrow_joint: return "l_eyebrow_joint"; break;
		case r_eyebrow_joint: return "r_eyebrow_joint"; break;
		case temporomandibular: return "temporomandibular"; break;
		case l_acromioclavicular: return "l_acromioclavicular"; break;
		case r_acromioclavicular: return "r_acromioclavicular"; break;
		case l_thumb1: return "l_thumb1"; break;
		case l_thumb2: return "l_thumb2"; break;
		case l_thumb3: return "l_thumb3"; break;
		case l_index0: return "l_index0"; break;
		case l_index1: return "l_index1"; break;
		case l_index2: return "l_index2"; break;
		case l_index3: return "l_index3"; break;
		case l_middle0: return "l_middle0"; break;
		case l_middle1: return "l_middle1"; break;
		case l_middle2: return "l_middle2"; break;
		case l_middle3: return "l_middle3"; break;
		case l_ring0: return "l_ring0"; break;
		case l_ring1: return "l_ring1"; break;
		case l_ring2: return "l_ring2"; break;
		case l_ring3: return "l_ring3"; break;
		case l_pinky0: return "l_pinky0"; break;
		case l_pinky1: return "l_pinky1"; break;
		case l_pinky2: return "l_pinky2"; break;
		case l_pinky3: return "l_pinky3"; break;
		case r_thumb1: return "r_thumb1"; break;
		case r_thumb2: return "r_thumb2"; break;
		case r_thumb3: return "r_thumb3"; break;
		case r_index0: return "r_index0"; break;
		case r_index1: return "r_index1"; break;
		case r_index2: return "r_index2"; break;
		case r_index3: return "r_index3"; break;
		case r_middle0: return "r_middle0"; break;
		case r_middle1: return "r_middle1"; break;
		case r_middle2: return "r_middle2"; break;
		case r_middle3: return "r_middle3"; break;
		case r_ring0: return "r_ring0"; break;
		case r_ring1: return "r_ring1"; break;
		case r_ring2: return "r_ring2"; break;
		case r_ring3: return "r_ring3"; break;
		case r_pinky0: return "r_pinky0"; break;
		case r_pinky1: return "r_pinky1"; break;
		case r_pinky2: return "r_pinky2"; break;
		case r_pinky3: return "r_pinky3"; break;
		default: return "unknown_joint";
	}
}

void pcjoints::initJointMap_global(std::vector<jointTp>& jointMap) {
	jointMap.clear();
	jointMap.push_back(l_hip);
	jointMap.push_back(l_knee);
	jointMap.push_back(l_ankle);
	jointMap.push_back(l_subtalar);
	jointMap.push_back(r_hip);
	jointMap.push_back(r_knee);
	jointMap.push_back(r_ankle);
	jointMap.push_back(r_subtalar);
	jointMap.push_back(vl5);
	jointMap.push_back(vl3);
	jointMap.push_back(vl1);
	jointMap.push_back(vt10);
	jointMap.push_back(vt1);
	jointMap.push_back(vc4);
	jointMap.push_back(vc2);
	jointMap.push_back(skullbase);
	jointMap.push_back(l_sternoclavicular);
	jointMap.push_back(l_shoulder);
	jointMap.push_back(l_elbow);
	jointMap.push_back(l_wrist);
	jointMap.push_back(r_sternoclavicular);
	jointMap.push_back(r_shoulder);
	jointMap.push_back(r_elbow);
	jointMap.push_back(r_wrist);
	jointMap.push_back(root);
}

int pcjoints::getPCIndex(const std::vector<jointTp>& jointMap, const jointTp& joint) {
	for (int i=0; i<jointMap.size(); i++)
	if (jointMap[i] == joint)
		return i;
	return -1;
}


jointTp pcjoints::getJoint(const std::vector<jointTp>& jointMap, const int& pcIndex) {
	if (pcIndex<0 || pcIndex>jointMap.size())
		return unknown_joint;
	else
		return jointMap[pcIndex];
}

int pcjoints::getJointNumber(const jointTp& joint) {
	switch(joint) {
		case l_hip: return 0; break;
		case l_knee: return 1; break;
		case l_ankle: return 2; break;
		case l_subtalar: return 3; break;
		case r_hip: return 4; break;
		case r_knee: return 5; break;
		case r_ankle: return 6; break;
		case r_subtalar: return 7; break;
		case vl5: return 8; break;
		case vl3: return 9; break;
		case vl1: return 10; break;
		case vt10: return 11; break;
		case vt1: return 12; break;
		case vc4: return 13; break;
		case vc2: return 14; break;
		case skullbase: return 15; break;
		case l_sternoclavicular: return 16; break;
		case l_shoulder: return 17; break;
		case l_elbow: return 18; break;
		case l_wrist: return 19; break;
		case r_sternoclavicular: return 20; break;
		case r_shoulder: return 21; break;
		case r_elbow: return 22; break;
		case r_wrist: return 23; break;
		case root: return 24; break;
		case sacroiliac: return 25; break;
		case l_midtarsal: return 26; break;
		case l_metatarsal: return 27; break;
		case r_midtarsal: return 28; break;
		case r_metatarsal: return 29; break;
		case vl4: return 30; break;
		case vl2: return 31; break;
		case vt12: return 32; break;
		case vt11: return 33; break;
		case vt9: return 34; break;
		case vt8: return 35; break;
		case vt7: return 36; break;
		case vt6: return 37; break;
		case vt5: return 38; break;
		case vt4: return 39; break;
		case vt3: return 40; break;
		case vt2: return 41; break;
		case vc7: return 42; break;
		case vc6: return 43; break;
		case vc5: return 44; break;
		case vc3: return 45; break;
		case vc1: return 46; break;
		case l_eyelid_joint: return 47; break;
		case r_eyelid_joint: return 48; break;
		case l_eyeball_joint: return 49; break;
		case r_eyeball_joint: return 50; break;
		case l_eyebrow_joint: return 51; break;
		case r_eyebrow_joint: return 52; break;
		case temporomandibular: return 53; break;
		case l_acromioclavicular: return 54; break;
		case r_acromioclavicular: return 55; break;
		case l_thumb1: return 56; break;
		case l_thumb2: return 57; break;
		case l_thumb3: return 58; break;
		case l_index0: return 59; break;
		case l_index1: return 60; break;
		case l_index2: return 61; break;
		case l_index3: return 62; break;
		case l_middle0: return 63; break;
		case l_middle1: return 64; break;
		case l_middle2: return 65; break;
		case l_middle3: return 66; break;
		case l_ring0: return 67; break;
		case l_ring1: return 68; break;
		case l_ring2: return 69; break;
		case l_ring3: return 70; break;
		case l_pinky0: return 71; break;
		case l_pinky1: return 72; break;
		case l_pinky2: return 73; break;
		case l_pinky3: return 74; break;
		case r_thumb1: return 75; break;
		case r_thumb2: return 76; break;
		case r_thumb3: return 77; break;
		case r_index0: return 78; break;
		case r_index1: return 79; break;
		case r_index2: return 80; break;
		case r_index3: return 81; break;
		case r_middle0: return 82; break;
		case r_middle1: return 83; break;
		case r_middle2: return 84; break;
		case r_middle3: return 85; break;
		case r_ring0: return 86; break;
		case r_ring1: return 87; break;
		case r_ring2: return 88; break;
		case r_ring3: return 89; break;
		case r_pinky0: return 90; break;
		case r_pinky1: return 91; break;
		case r_pinky2: return 92; break;
		case r_pinky3: return 93; break;
		default: return -1;
	}
}

jointTp pcjoints::getJointFromNumber(const int& nr) {
	switch(nr) {
		case 0: return l_hip; break;
		case 1: return l_knee; break;
		case 2: return l_ankle; break;
		case 3: return l_subtalar; break;
		case 4: return r_hip; break;
		case 5: return r_knee; break;
		case 6: return r_ankle; break;
		case 7: return r_subtalar; break;
		case 8: return vl5; break;
		case 9: return vl3; break;
		case 10: return vl1; break;
		case 11: return vt10; break;
		case 12: return vt1; break;
		case 13: return vc4; break;
		case 14: return vc2; break;
		case 15: return skullbase; break;
		case 16: return l_sternoclavicular; break;
		case 17: return l_shoulder; break;
		case 18: return l_elbow; break;
		case 19: return l_wrist; break;
		case 20: return r_sternoclavicular; break;
		case 21: return r_shoulder; break;
		case 22: return r_elbow; break;
		case 23: return r_wrist; break;
		case 24: return root; break;
		case 25: return sacroiliac; break;
		case 26: return l_midtarsal; break;
		case 27: return l_metatarsal; break;
		case 28: return r_midtarsal; break;
		case 29: return r_metatarsal; break;
		case 30: return vl4; break;
		case 31: return vl2; break;
		case 32: return vt12; break;
		case 33: return vt11; break;
		case 34: return vt9; break;
		case 35: return vt8; break;
		case 36: return vt7; break;
		case 37: return vt6; break;
		case 38: return vt5; break;
		case 39: return vt4; break;
		case 40: return vt3; break;
		case 41: return vt2; break;
		case 42: return vc7; break;
		case 43: return vc6; break;
		case 44: return vc5; break;
		case 45: return vc3; break;
		case 46: return vc1; break;
		case 47: return l_eyelid_joint; break;
		case 48: return r_eyelid_joint; break;
		case 49: return l_eyeball_joint; break;
		case 50: return r_eyeball_joint; break;
		case 51: return l_eyebrow_joint; break;
		case 52: return r_eyebrow_joint; break;
		case 53: return temporomandibular; break;
		case 54: return l_acromioclavicular; break;
		case 55: return r_acromioclavicular; break;
		case 56: return l_thumb1; break;
		case 57: return l_thumb2; break;
		case 58: return l_thumb3; break;
		case 59: return l_index0; break;
		case 60: return l_index1; break;
		case 61: return l_index2; break;
		case 62: return l_index3; break;
		case 63: return l_middle0; break;
		case 64: return l_middle1; break;
		case 65: return l_middle2; break;
		case 66: return l_middle3; break;
		case 67: return l_ring0; break;
		case 68: return l_ring1; break;
		case 69: return l_ring2; break;
		case 70: return l_ring3; break;
		case 71: return l_pinky0; break;
		case 72: return l_pinky1; break;
		case 73: return l_pinky2; break;
		case 74: return l_pinky3; break;
		case 75: return r_thumb1; break;
		case 76: return r_thumb2; break;
		case 77: return r_thumb3; break;
		case 78: return r_index0; break;
		case 79: return r_index1; break;
		case 80: return r_index2; break;
		case 81: return r_index3; break;
		case 82: return r_middle0; break;
		case 83: return r_middle1; break;
		case 84: return r_middle2; break;
		case 85: return r_middle3; break;
		case 86: return r_ring0; break;
		case 87: return r_ring1; break;
		case 88: return r_ring2; break;
		case 89: return r_ring3; break;
		case 90: return r_pinky0; break;
		case 91: return r_pinky1; break;
		case 92: return r_pinky2; break;
		case 93: return r_pinky3; break;
		default: return unknown_joint;
	}
}


