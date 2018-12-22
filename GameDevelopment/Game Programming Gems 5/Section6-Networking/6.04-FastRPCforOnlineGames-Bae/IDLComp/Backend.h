// IDL compiler backend 
//
// Copyright 2004 Bae,Hyun-jik.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#pragma once

#include "Specification.h"

void WriteHeaderStart(ostream &os);
void WriteFunctionIDs(ostream &os,CSpecification* spec);
void WriteFunctionDecls(ostream &os,CSpecification* spec);
void WriteFunctionDecl(ostream &os,CFunction* func);
void WriteParametersWithComma(ostream &os,CFunction* func);
void WriteFunctionProxies(ostream &os,CSpecification* spec);
void WriteStubDecl(ostream &os,CSpecification* spec);
void WriteStubImpl(ostream &os,CSpecification* spec);
void WriteParametersWithCommaAndWithoutTypes(ostream &os,CFunction* func);