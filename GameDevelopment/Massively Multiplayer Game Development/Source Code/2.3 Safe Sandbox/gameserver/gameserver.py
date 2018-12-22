# gameserver.py
#
# Trivial implementation of an asynchronous game server
# based on the Python asyncore.py and asynchat.py 
# modules.
#
# Author: Matthew Walker
#         mwalker@softhome.net
#

import asynchat
import asyncore
import socket
import string
import traceback
import sys
import request
import objmgr
import os
import deferred
import request
import re


# special object id for game server
ID_SERVER = -100  # so as not to collide with generated ids

class GameServer (asyncore.dispatcher):
  """
  Main server for game, center of the universe, as it were.
  This is based on the python asyncore module, which is a 
  really cool way to develop asynchronous servers very easily.
  Oh, and 42.
  """
  
  def __init__ (self, port):
    #print 'GameServer.__init__( %s )' % (port,)
    asyncore.dispatcher.__init__ (self)
    self.create_socket (socket.AF_INET, socket.SOCK_STREAM)
    self.set_reuse_addr()
    here = ('', port)
    self.bind (here)
    self.listen (5)
    self.clients = {}
    self.id = ID_SERVER
    objmgr.AddObject(self, self.id)

  def handle_accept (self):
    #print 'GameServer.handle_accept()'
    ClientConnection (self, self.accept())

  def request (self, id, data):
    #print 'GameServer.request( %s )' % (data,)
    requester = self.clients[id]

    # deferred call to request handler
    deferred.Call(request.ID_REQUEST, 'Handle', (id, data))

  def respond (self, result):
    requester = self.clients[result.clientid]
    requester.reply(str(result))
    if not requester.loggedin:
      self.close_client(result.clientid)

  def pre_close_client (self, clientid):
    requester = self.clients[clientid]
    requester.loggedin = 0

  def close_client (self, clientid):
    requester = self.clients[clientid]
    requester.handle_close()

  def add_connection (self, id, connection):
    #print 'add_connection( %s, %s )' % (id, connection)
    assert(not self.clients.get(id))
    self.clients[id] = connection

  def remove_connection (self, id, connection):
    #print 'remove_connection( %s, %s )' % (id, connection)
    del self.clients[id]

  def handle_error (self):
    print 'GameServer: ERROR'
    traceback.print_exc()

  def handle_close (self):
    objmgr.RemoveObject(self)


class ClientConnection (asynchat.async_chat):
  """
  Manages interactions with a single client.
  """
  conn_counter = 0    # class level counter, always increments
  loginString = "Please enter your name: "
  promptString = "Let's play!> "

  def __init__ (self, server, (conn, addr)):
    asynchat.async_chat.__init__ (self, conn)
    self.set_terminator (os.linesep)
    self.server = server
    self.id = ClientConnection.conn_counter
    self.server.add_connection(self.id, self)
    ClientConnection.conn_counter += 1
    self.buffer = ''
    self.loginchecked = 0
    self.check_login()
    #print 'ClientConnection[ %d ].__init__( %s, %s )' % (self.id, server, (conn, addr))

  def __del__(self):
    print 'ClientConnection.__del__()'

  def collect_incoming_data (self, data):
    #print 'ClientConnection[ %d ].collect_incoming_data()' % (self.id,)
    self.buffer = self.buffer + data
    # echo
    self.push(data)
    
  def found_terminator (self):
    #print 'ClientConnection[ %d ].found_terminator()' % (self.id,)
    data = self.buffer
    self.buffer = ''
    print '<== (%d) %s' % (self.id, repr(data))
    self.push(os.linesep)
    if self.check_login(data):
      self.server.request (self.id, data)

  def handle_close (self):
    #print 'ClientConnection[ %d ].handle_close()' % (self.id,)
    print 'Closing'
    self.server.remove_connection(self.id, self)
    self.connected = 0
    self.close()

  def handle_error (self):
    print 'ClientConnection[ %s ]: ERROR' % (self,)
    traceback.print_exc()

  def check_login (self, data=''):
    """ Login state machine."""
    if not self.loginchecked:
      self.loginchecked = 1
      self.loggedin = 0
      self.push(self.loginString)
      return 0
    elif not self.loggedin:
      # assume any data provided is the name
      self.loggedin = 1
      name = re.sub(r'\s+', '_', data)
      self.server.request (self.id, '_LOGIN %s' % (name,))
      return 0
    return 1

  def prompt (self):
    self.push(self.promptString)

  def reply (self, data):
    if data:
      data = re.sub(r'\n', os.linesep, data)  # replace newlines with linesep
      self.push(data + os.linesep)
    if self.loggedin:
      self.prompt()


