
def printResult(x):
    print 'result:',x

# remote method call
remoteObject.callRemote("test").addCallback(printResult)

# database interaction
dbInterface.runQuery("select * from random_table").addCallback(printResult)

# threadpool execution
def myMethod(a, b, c):
    return a + b + c
deferToThread(myMethod, 1, 2, 3).addCallback(printResult)

# Remote method calls that invoke a database connection and wait until the
# database is done returning its query before turning the query into a string
# and then sending the string as the result of the remote method call.

class MyClass(Referenceable):
    def remote_doIt(self):
        return self.databaseConnector.runQuery(
            "select * from foo").addCallback(str)
