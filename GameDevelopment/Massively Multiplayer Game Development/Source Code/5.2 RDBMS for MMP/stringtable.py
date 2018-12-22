'''
stringtable.py - demonstrates a simple implementation of string table functionality
'''

import lookup


def Lookup(stringId):
  '''
  Returns the value of the specified stringid if found, None otherwise
  '''
  import stringtabledata
  if stringtabledata.stringDict.has_key(stringId):
    return stringtabledata.stringDict[stringId][0]
  return None

def Test():
  # build an in memory stringtable lookup, then use it
  dbData = [(35, '/gamedata/audio/footsteps.wav', 3), (36, '/gamedata/animation/fly.ani', 1)]
  stringData = lookup._CreateDict(dbData)
  lookup._BuildInMemoryDictLookup('stringtabledata', stringData, 'stringDict')
  import stringtabledata
  
  print '\n'
  print 'The contents of stringtabledata.stringDict are:' 
  print '%s' % stringtabledata.stringDict
  print '\n'
  print 'Looking up id 35 returns: %s' % Lookup(35)
  
if __name__ == '__main__':
  Test()
