//
// Copyright 2004 Bae,Hyun-jik.  All rights reserved.
//
// Permission is granted to use this code for any purpose, as long as this
// copyright message remains intact.

#include "StdAfx.h"
#include ".\specification.h"
#include <antlr/SemanticException.hpp>

CSpecification::CSpecification(void)
{
}

CSpecification::~CSpecification(void)
{
}

void CSpecification::SetConfigs(CConfigurations *configs)
{
	m_configurations=*configs;
	m_configurations.m_incID=0;
}

void CSpecification::AddFunction(CFunction* func)
{
	m_functions.push_back(*func);
	char incIDStr[100];
	sprintf(incIDStr,"%d",m_configurations.m_incID);
	m_functions.back().m_ID=string("(") + m_configurations.m_startID + string("+") + string(incIDStr) + string(")") ;
	m_configurations.m_incID++;
}

CConfigurations::CConfigurations()
{
	m_incID=0;
	m_startID="1";
}

void CConfigurations::AddConfig(CConfiguration *config)
{
	if(config->m_name=="StartID")
	{
		m_startID=config->m_value;
	}
	else
	{
		throw SemanticException("Unidentified config variable");
	}
}

void CConfiguration::SetName(string name)
{
	m_name=name;
}

void CConfiguration::SetValue(string value)
{
	m_value=value;
}

void CFunction::SetName(string name)
{
	m_name=name;
}
void CFunction::AddParameter(CParameter* param)
{
	m_params.push_back(*param);
}

void CParameter::SetType(string type)
{
	m_type=type;
}
void CParameter::SetName(string name)
{
	m_name=name;
}