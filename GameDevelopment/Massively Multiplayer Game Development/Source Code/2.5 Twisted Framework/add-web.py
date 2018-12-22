from twisted.web.server import Site
from twisted.web.static import File
from twisted.web.script import ResourceScript
f = File('.')
f.processors = { '.rpy': ResourceScript }
app.listenTCP(8080, Site(f))
