'''
localization.py - demonstrates the use of lookups and contant modules to implement a solution
                  for localization
'''

import lookup
import constantmodule

currentLanguage = 1

def SetCurrentLanguage(newLanguage):
  '''
  Allows changing of language by passing one of the contants from the
  language module as the newLangauge arg
  '''
  import language
  assert language.lookup.has_key(newLanguage), 'Not a know language!'
  
  global currentLanguage
  currentLanguage = newLanguage
  
def Lookup(localizedItemId):
  '''
  Returns the unicode text for the specified localized item, in the
  current language
  '''
  global currentLanguage
  import localizedtext
  if localizedtext.lookup.has_key((currentLanguage, localizedItemId)):
    return localizedtext.lookup[(currentLanguage, localizedItemId)][0]
  else:
    return U'Item not translated yet'

def Test():
  '''
  Demonstrates the localization functionality, including switching
  language on the fly
  '''
  
  # setup the data, including unicode strings for translated text
  languageData = [ (1, 'English'), (2, 'French'), (3, 'Korean') ]
  localizedItemData = [ (1, 'Button OK'), (2, 'Button Cancel') ]
  localizedTextData = [ (1, 1, U'OK'), (1, 2, U'Cancel'), (2, 1, U'Oui'), (2, 2, U'Non')]
  
  # build a localizeditem contatn module and import it
  constantmodule._BuildInMemoryConstantModule('localizeditem', localizedItemData)
  import localizeditem
  
  # build a language constant module and a lookup, then import it
  constantmodule._BuildInMemoryConstantModule('language', languageData)
  tempDict = lookup._CreateDict(languageData)
  lookup._BuildInMemoryDictLookup('language', tempDict)
  import language
  
  # build a localized text lookup
  tempDict = lookup._CreateMultiWayKeyDict(localizedTextData, 2)
  lookup._BuildInMemoryDictLookup('localizedtext', tempDict)
  import localizedtext
  
  print'\n'
  print 'Current language is %s' % language.lookup[currentLanguage]
  print 'The word for OK is %s' % Lookup(localizeditem.BUTTON_OK)
  print 'The word for Cancel is %s' % Lookup(localizeditem.BUTTON_CANCEL)
  
  print '\nChanging language\n'
  SetCurrentLanguage(language.FRENCH)
  
  print 'Current language is %s' % language.lookup[currentLanguage]
  print 'The word for OK is %s' % Lookup(localizeditem.BUTTON_OK)
  print 'The word for Cancel is %s' % Lookup(localizeditem.BUTTON_CANCEL)
  
if __name__ == '__main__':
  Test()
    
  