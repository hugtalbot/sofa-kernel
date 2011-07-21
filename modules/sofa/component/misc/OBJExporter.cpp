/*
 * OBJExporter.cpp
 *
 *  Created on: 9 sept. 2009
 *      Author: froy
 */

#include "OBJExporter.h"

#include <sstream>

#include <sofa/core/ObjectFactory.h>

#include <sofa/core/objectmodel/Event.h>
#include <sofa/simulation/common/AnimateBeginEvent.h>
#include <sofa/simulation/common/AnimateEndEvent.h>
#include <sofa/simulation/common/ExportOBJVisitor.h>
#include <sofa/core/objectmodel/KeypressedEvent.h>
#include <sofa/core/objectmodel/KeyreleasedEvent.h>

namespace sofa
{

namespace component
{

namespace misc
{

SOFA_DECL_CLASS(OBJExporter)

int OBJExporterClass = core::RegisterObject("Export under Wavefront OBJ format")
        .add< OBJExporter >();

OBJExporter::OBJExporter()
    : stepCounter(0)
    , outfile(NULL)
    , mtlfile(NULL)
    , objFilename( initData(&objFilename, "filename", "output OBJ file name"))
    , exportEveryNbSteps( initData(&exportEveryNbSteps, (unsigned int)0, "exportEveryNumberOfSteps", "export file only at specified number of steps (0=disable)"))
    , exportAtBegin( initData(&exportAtBegin, false, "exportAtBegin", "export file at the initialization"))
    , exportAtEnd( initData(&exportAtEnd, false, "exportAtEnd", "export file when the simulation is finished"))
    , activateExport(false)
{
    this->f_listening.setValue(true);
}

OBJExporter::~OBJExporter()
{
    if (outfile)
        delete outfile;
    if (mtlfile)
        delete mtlfile;
}

void OBJExporter::init()
{
    context = this->getContext();
    maxStep = exportEveryNbSteps.getValue();
}

void OBJExporter::writeOBJ()
{
    if (!maxStep) return;

    std::string filename = objFilename.getFullPath();
    std::ostringstream oss;
    oss.width(5);
    oss.fill('0');
    oss << stepCounter / maxStep;
    filename += oss.str() + ".obj";
    outfile = new std::ofstream(filename.c_str());

    std::string mtlfilename = objFilename.getFullPath();
    mtlfilename = mtlfilename + ".mtl";
    mtlfile = new std::ofstream(mtlfilename.c_str());
    sofa::simulation::ExportOBJVisitor exportOBJ(core::ExecParams::defaultInstance(),outfile, mtlfile);
    context->executeVisitor(&exportOBJ);
    outfile->close();
    mtlfile->close();

    std::cout << "Exporting OBJ as: " << filename.c_str() << " with MTL file: " << mtlfilename.c_str() << std::endl;
}

void OBJExporter::handleEvent(sofa::core::objectmodel::Event *event)
{
    if (sofa::core::objectmodel::KeypressedEvent* ev = dynamic_cast<sofa::core::objectmodel::KeypressedEvent*>(event))
    {
        switch(ev->getKey())
        {

        case 'E':
        case 'e':
        {
            writeOBJ();
            break;
        }

        case 'P':
        case 'p':
        {
            if (!activateExport)
                std::cout << "Starting OBJ sequece export..." << std::endl;
            else
                std::cout << "Ending OBJ sequece export..." << std::endl;
            activateExport = !activateExport;
            break;
        }
        }
    }

    if ( /*simulation::AnimateEndEvent* ev =*/  dynamic_cast<simulation::AnimateEndEvent*>(event))
    {
        if (maxStep == 0 || !activateExport) return;

        stepCounter++;
        if(stepCounter % maxStep == 0)
        {
            writeOBJ();
        }
    }
}

void OBJExporter::cleanup()
{
    if (exportAtEnd.getValue())
        writeOBJ();

}

void OBJExporter::bwdInit()
{
    if (exportAtBegin.getValue())
        writeOBJ();
}

}

}

}
