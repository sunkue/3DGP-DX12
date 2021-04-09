#include "Obj.h"


void OBJ::Power2ChildrenWM() {
	for (auto& prts : m_childrens) {
		prts->AddParentsWorldMat(WorldMat());
		prts->Power2ChildrenWM();
	}
}

void OBJ::ResetChildrenWM() {
	for (auto& prts : m_childrens) {
		prts->ResetParentsWorldMat();
		prts->ResetChildrenWM();
	}
}