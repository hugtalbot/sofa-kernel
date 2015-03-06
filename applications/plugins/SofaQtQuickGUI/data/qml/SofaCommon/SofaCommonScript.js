.pragma library

function InstanciateComponent(url, parent, properties) {
    if(!parent)
        parent = null;

    if(!properties)
        properties = null;

    var component = Qt.createComponent(url);
    if(3 === component.status)
        return null;

    var incubator = component.incubateObject(parent, properties);
    incubator.forceCompletion();

    return incubator.object;
}
