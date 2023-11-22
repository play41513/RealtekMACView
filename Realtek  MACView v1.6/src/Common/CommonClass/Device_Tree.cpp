// Module:
// Notices: Copyright(c) 2015 Robbin Hsiao.
// Purpose:
// Compiler: Embarcadero RAD Studio XE8
// OS: Windows 8.1 x64
// Version: 0.0.0.1
//---------------------------------------------------------------------------

#pragma hdrstop

#include "Device_Tree.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
Device_Tree::Device_Tree():
tinyxml2::XMLDocument()
{
	this->SetBOM(true);
}
//---------------------------------------------------------------------------
Device_Tree::~Device_Tree(){}
//---------------------------------------------------------------------------
void Device_Tree::Clear(){
	tinyxml2::XMLDocument::Clear();
}
//---------------------------------------------------------------------------
