'''
lookup.py - provides several utility functions that can be used to build in memory and on disk lookups
						of various types
'''

import pprint
import sys
import imp

def _CreateDict(dbData):
	'''
	Given dbData from the db that has a single column key, return a dictionary with
	the first column as the key, as the value as the remaining column as the value.
	Given the following data: [(1, 'Test1', 1), (2, 'Test2', 1)], the result will be { 1 : ('Test1', 1), 2 : ('Test2', 1) }
	'''
	tempDict = {}
	for row in dbData:
		key = row[0]
		value = row[1:]
		# handle case where table only has 1 column, make it the value as well
		if len(row) == 1:
			value = [1]
		tempDict[key] = value
		
	return tempDict
	

def _CreateMultiWayDict(dbData):
	'''
	Given dbData from the db, return a dictionary with the first column as the key, and the value
	as a list of 1 to n items that have a matching key.  That is,  given the following data: 
	[(1, 'Test1', 1), (1, 'Test2', 1)], the result will be { 1 : [('Test1', 1), ('Test2', 1)] }
	'''
	
	tempDict = {}
	for row in dbData:
		key = row[0]
		value = row[1:]
		if not tempDict.has_key(key):
			tempDict[key] = []
		tempDict[key].append(value)
		
	return tempDict

def _CreateMultiWayKeyDict(dbData, keyCount):
	'''
	Given dbData from the db t, return a dictionary with the first keyCount columns as the tuple- 
	based key, and the value a list of 1 to n items that have a matching key.  That is, given the 
	following data and a keyCount of 2: 
	[(1, 117, 'Test1', 1), (1, 117, 'Test2', 1), (2, 134, 'Test3', 0)], the result will be { (1, 117) : [('Test1', 1), ('Test2', 0)], (2, 134) : [('Test3', 0)] }
	'''
		
	assert keyCount > 0, 'keyCount must be at least 1'
	assert keyCount < len(dbData[0]), 'keyCount must be less than number of columns in returned row'
	
	tempDict = {}
	for row in dbData:
		key = row[:keyCount]
		value = row[keyCount:]
		if not tempDict.has_key(key):
			tempDict[key] = []
		tempDict[key].append(value)
		
	return tempDict

	
def _BuildInMemoryDictLookup(moduleName, dict, varName = 'lookup'):
	'''
	Creates or accesses an in memory module of the name moduleName, and 
	populates it with an attribute named varName, and the contents of dict

	moduleName - name of module to generate/update with dictionary variable
	dict       - a valid Python dictionary with the desired contents
	varName    - the variable name to give the dictionary in the module
	'''  
	
	mod = None
	if sys.modules.has_key(moduleName):
		mod = sys.modules[moduleName]
	else:
		mod = imp.new_module(moduleName)
		sys.modules[moduleName] = mod
		
	setattr(mod, varName, dict)
		
		
		
def _GenerateOnDiskDictLookup(filePath, dict, varName = 'lookup'):
	'''
	Generates on on disk file with the name specified in filePath, and
	populates it with an attribute named varName, and the contents of dict

	filePath   - file path in form '[path]/[moduleName].py', e.g. 'C:/mycode/chattype.py'
	dict       - a valid Python dictionary with the desired contents
	varName    - the variable name to give the dictionary in the module
	'''  
	
	try:
		fstream = open(filePath, 'w')
	except IOError:
		print 'failed to open %s for write' % filePath
		return
	else:
		# write a header for the file
		fstream.write('# This file is auto generated from the database.\n')
		fstream.write('\n')
		
		# write the variable for the dictionary out
		fstream.write( '\n'+varName+' = \\\n' )

		# use the pprint module to 'pretty print' the contents of the dictionary to the file
		pprint.pprint(dict, fstream)

		fstream.close()    
		
		
def Test():
	'''
	Tests the various utilities provided in this module, check out the docs
	with the utilities for details on their functionality
	'''

	dbData = [(1, 'Test1', 1), (2, 'Test2', 1)]
	dict = _CreateDict(dbData)
	
	_GenerateOnDiskDictLookup('test1.py', dict, 'testVar1')
	import test1
	print '\n'
	print 'Contents of test 1 dict are: %s\n' % test1.testVar1
	
	
	_BuildInMemoryDictLookup('test2', dict, 'testVar2')
	import test2
	print 'Contents of test 2 dict are: %s\n' % test2.testVar2
			
			
	dbData = [(1, 'Test1', 1), (1, 'Test2', 1)]
	dict = _CreateMultiWayDict(dbData)
	
	_GenerateOnDiskDictLookup('test3.py', dict, 'testVar3')
	import test3
	print 'Contents of test 3 dict are: %s\n' % test3.testVar3  
	
	_BuildInMemoryDictLookup('test4', dict, 'testVar4')
	import test4
	print 'Contents of test 4 dict are: %s\n' % test4.testVar4
	
	
	dbData =  [(1, 117, 'Test1', 1), (1, 117, 'Test2', 1), (2, 134, 'Test3', 0)]
	dict = _CreateMultiWayKeyDict(dbData, 2)
	
	_GenerateOnDiskDictLookup('test5.py', dict, 'testVar5')
	import test5
	print 'Contents of test 5 dict are: %s\n' % test5.testVar5
	
	_BuildInMemoryDictLookup('test6', dict, 'testVar6')
	import test6
	print 'Contents of test 6 dict are: %s\n' % test6.testVar6
	
if __name__ == '__main__':	
  Test()
	