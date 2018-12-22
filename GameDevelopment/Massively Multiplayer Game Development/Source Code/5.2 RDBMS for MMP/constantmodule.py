'''
constantmodule.py - illustrates the technique of building a module of variables that are
                    used as constants from data retrieved from a relational database.
'''

import sys
import imp
import string

def _BuildInMemoryConstantModule(moduleName, dbData):
  '''
  Builds an in memory constant module from the specified args.  To use
  the module, import in within your code.  Note however that the module
  must have been built prior to the processing of the import statement

  moduleName - name of module to generate
  dbData     - list of tuples of data retrieved from database, the data would look
               like the following, with a tuple values for each row
               [ (1, 'Global'), (2, 'Radial'), (3, 'Whisper') ]
  '''  
  
  # assumption is that the module only gets built once, so we're not checking
  # that it previously exists
  mod = imp.new_module(moduleName)
  sys.modules[moduleName] = mod
  
  for row in dbData:
    # assumption is that every table used to generate a constant module will have
    # an id and description as its first 2 columns
    (constantValue, constantName) = row[0:2]
    
    # modify the constant name to a constant form, making all letters upper
    # case and replacing all spaces with underscores
    constantName = string.upper(string.replace(constantName, ' ', '_'))
    
    # insert it into the module
    setattr(mod, constantName, constantValue)
    
    
    
def _BuildOnDiskConstantModule(filePath, dbData):
  '''
  Builds a constant module to an on disk file given the specified args.  To use it,
  simply import it from within your code.

  filePath   - file path in form '[path]/[moduleName].py', e.g. 'C:/mycode/chattype.py'
  dbData     - list of tuples of data retrieved from database, the data would look
               like the following, with a tuple values for each row
               [ (1, 'Global'), (2, 'Radial'), (3, 'Whisper') ]
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
    
    for row in dbData:
      # assumption is that every table used to generate a constant module will have
      # an id and description as its first 2 columns
      (constantValue, constantName) = row[0:2]
      
      # modify the constant name to a constant form, making all letters upper
      # case and replacing all spaces with underscores
      constantName = string.upper(string.replace(constantName, ' ', '_'))
     
      # write the assignement statement out to the file, with a newline character to end
      fstream.write(constantName)
      fstream.write(' = ')
      fstream.write(str(constantValue))
      fstream.write('\n') 
  
     
def Test():
  '''
  Demonstrates the creation of contant modules
  '''

  # create contant module on disk  
  dbData = [ (1, 'Global'), (2, 'Radial'), (3, 'Whisper') ]
  
  _BuildOnDiskConstantModule('chattype1.py', dbData)
  import chattype1
  
  print 'Imported module from disk:'
  print 'value of GLOBAL = %s' % chattype1.GLOBAL
  print 'value of RADIAL = %s' % chattype1.RADIAL
  print 'value of WHISPER = %s' % chattype1.WHISPER
  del sys.modules['chattype1']
  print '\n'
  
  # create constant module in memory
  dbData = [ (5, 'Global'), (6, 'Radial'), (7, 'Whisper') ]
  
  _BuildInMemoryConstantModule('chattype2', dbData)
  import chattype2
  
  print 'Imported in memory module:'
  print 'value of GLOBAL = %s' % chattype2.GLOBAL
  print 'value of RADIAL = %s' % chattype2.RADIAL
  print 'value of WHISPER = %s' % chattype2.WHISPER
  
if __name__ == '__main__':
  Test()