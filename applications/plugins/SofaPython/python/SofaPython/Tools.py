import Sofa

import os.path

def listToStr(x):
    """ concatenate lists for use with data.
    """
    return ' '.join(map(str, x))

def listListToStr(xx):
    """ concatenate lists of list for use with data.
    """
    str_xx=""
    for x in xx:
        str_xx += listToStr(x)
    return str_xx

def strToListFloat(s):
    """ Convert a string to a list of float
    """
    return map(float,s.split())

def strToListInt(s):
    """ Convert a string to a list of float
    """
    return map(int,s.split())

def getObjectPath(obj):
    """ Return the path of this object
    """
    return obj.getContext().getPathName()+"/"+obj.name

def getNode(rootNode, path):
    """ Return node at path or None if not found
    """
    currentNode = rootNode
    pathComponents = path.split('/')
    for c in pathComponents:
        if len(c)==0: # for leading '/' and in case of '//'
            continue
        currentNode = currentNode.getChild(c)
        if currentNode is None:
            print "SofaPython.Tools.findNode: can't find node at", path
            return None
    return currentNode

def meshLoader(parentNode, filename, name=None, **args):
    """ Insert the correct MeshLoader based on the filename extension
    """
    ext = os.path.splitext(filename)[1]
    if name is None:
        _name="loader_"+os.path.splitext(os.path.basename(filename))[0]
    else:
        _name=name
    if ext == ".obj":
        return parentNode.createObject('MeshObjLoader', filename=filename, name=_name, **args)
    elif ext == ".vtu" or ext == ".vtk":
        return parentNode.createObject('MeshVTKLoader', filename=filename, name=_name, **args)
    else:
        print "ERROR SofaPython.Tools.meshLoader: unknown mesh extension:", ext
        return None
