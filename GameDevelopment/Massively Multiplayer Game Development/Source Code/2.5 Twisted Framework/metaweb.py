from twisted.web import wmvc, microdom
from twisted.python import domhelpers

class MBuggyWorld(wmvc.WModel):
    def __init__(self, world, *args, **kw):
        wmvc.WModel.__init__(self, world, *args, **kw)
        self.world = world

class CBuggyWorld(wmvc.WController):
    pass

class VBuggyWorld(wmvc.WView):
    templateFile = "metaweb.html"
    def factory_bugDisplay(self, request, node):
        rowNode = domhelpers.locateNodes([node], "rowOf", "bugDisplay")[0]
        node.removeChild(rowNode)
        bugList = self.model.world.perspectives.values()
        bugList.sort(lambda a, b: a.angst < b.angst)
        rank = 0
        for bug in bugList:
            rank += 1
            rnode = rowNode.cloneNode(1)
            node.appendChild(rnode)
            colNodes = domhelpers.locateNodes([rnode],"columnOf","bugDisplay")
            bugDict = {"name": bug.perspectiveName,
                       "angst": bug.angst,
                       "rank": rank}
            for cn in colNodes:
                cn.appendChild(microdom.Text(str(bugDict[cn.getAttribute("columnName")])))
        return node


wmvc.registerViewForModel(VBuggyWorld, MBuggyWorld)
wmvc.registerControllerForModel(CBuggyWorld, MBuggyWorld)
