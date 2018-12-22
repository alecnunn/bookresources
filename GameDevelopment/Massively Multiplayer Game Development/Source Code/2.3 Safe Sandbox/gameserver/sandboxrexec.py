# sandboxrexec.py
#
# Implement the restricted execution functionality
# that makes the SafeSandbox "safe". Extends the 
# standard Python RExec functionality to meet the 
# specific needs of our game.
#


import rexec
import os
import os.path
import string

def _GetSandboxPath():
  """
  Return a list of Python path entries (i.e. sys.path)
  that are valid to search for legal modules.
  """
  # load this from some kind of config file, preferably
  cwd = os.getcwd()
  sandboxpath = [
    './',
  ]	 # our server's area

  for ix in range(len(sandboxpath)):
    pathitem = sandboxpath[ix]
    newpath = os.path.normpath(os.path.join(cwd, pathitem))
    sandboxpath[ix] = newpath
  #print sandboxpath
  return sandboxpath


class SandboxRExec(rexec.RExec):
  """
  Responsible for defining the Python features allowed 
  to be used within the SafeSandbox environment.
  """
  #
  # Class-level attributes define the restricted environment.
  #

  # Extend Python's list of builtin functions we do not allow
  nok_builtin_names = rexec.RExec.nok_builtin_names + \
    ('compile', 'delattr', 'execfile', 'globals', 
     'input', 'locals', 'raw_input', 'vars')
  
  ok_builtin_modules = ('math', 'operator', 'time')
  ok_path = _GetSandboxPath()	# load the valid path
  ok_posix_names = ()           # no os module access
  ok_sys_names = ()             # no sys module access

  # Python library modules that are okay to import - 
  # may be redundant with certain modules in ok_builtin_modules
  ok_library_modules = \
    ('types', 'operator', 'copy', 'string', 'math', 'cmath', 'random', 'time')

  # our server modules that are okay to import
  ok_server_modules = \
    ('gameserver.gameevents', 'gameeventkeys')

  # set of all okay modules
  ok_modules = ok_builtin_modules + ok_library_modules + ok_server_modules

  # server packages below which it is okay to import modules
  ok_packages = ('gameserver.sandboxmodules',)


  # 
  # Override the rexec.RExec interface
  #

  def r_open(self, filename, mode=None, bufsize=None):
    """
    Prevent all access to the filesystem by disallowing calls to open().
    """
    raise IOError, 'No access to filesystem is allowed.'


  def r_import(self, modulename, globals={}, locals={}, fromlist=[]):
    """
    Verify that module being imported is among the list of 
    modules and packages that are allowed, and return a 
    reference to it if so.
    """
    okToImport = 0

    if modulename in self.ok_modules:
      okToImport = 1
    else:
      # special test to see if module is a submodule of an allowed package
      for pkg in self.ok_packages:
        if len(modulename) >= len(pkg):
          if modulename[:len(pkg)] == pkg:
            okToImport = 1

    if okToImport:
      mod = rexec.RExec.r_import(self, modulename, globals, locals, fromlist)
      components = string.split(modulename, '.')
      for comp in components[1:]:
        mod = getattr(mod, comp)
      return mod

    raise ImportError, 'Cannot import %s in restricted environment' % (modulename,)



