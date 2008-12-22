
#include <sofa/component/topology/SparseGridMultipleTopology.h>
#include <sofa/core/ObjectFactory.h>
#include <sofa/helper/system/FileRepository.h>

namespace sofa
{

namespace component
{

namespace topology
{



SOFA_DECL_CLASS(SparseGridMultipleTopology)

int SparseGridMultipleTopologyClass = core::RegisterObject("Sparse grid in 3D")
        .addAlias("SparseGridMultiple")
        .add< SparseGridMultipleTopology >()
        ;


void SparseGridMultipleTopology::buildAsFinest(  )
{
// 		  serr<<"SparseGridMultipleTopology::buildAsFinest"<<sendl;

    if( _dataStiffnessCoefs.getValue().size() < _fileTopologies.getValue().size() )
    {
        serr<<"WARNING: SparseGridMultipleTopology: not enough stiffnessCoefs"<<sendl;
        for(unsigned i=_dataStiffnessCoefs.getValue().size(); i<_fileTopologies.getValue().size(); ++i)
            _dataStiffnessCoefs.beginEdit()->push_back( 1.0 );
//           return;
    }
    if( _dataMassCoefs.getValue().size() < _fileTopologies.getValue().size() )
    {
        serr<<"WARNING: SparseGridMultipleTopology: not enough massCoefs\n";
        for(unsigned i=_dataMassCoefs.getValue().size(); i<_fileTopologies.getValue().size(); ++i)
            _dataMassCoefs.beginEdit()->push_back( 1.0 );
// 			return;
    }

    unsigned regularGridsSize = _regularGrids.size();
    if (regularGridsSize < _fileTopologies.getValue().size())
    {
        for (unsigned int i=0; i<_fileTopologies.getValue().size()-regularGridsSize; ++i)
        {
            _regularGrids.push_back(new RegularGridTopology());
        }
    }
    else
    {
        for (unsigned int i=0; i<regularGridsSize-_fileTopologies.getValue().size(); ++i)
        {
            delete _regularGrids[i+_regularGrids.size()];
        }
        _regularGrids.resize(_fileTopologies.getValue().size());
    }
    _regularGridTypes.resize(  _fileTopologies.getValue().size() );



    helper::vector< helper::io::Mesh*> meshes(_fileTopologies.getValue().size());


    SReal xMing=99999999, xMaxg=-99999999, yMing=99999999, yMaxg=-99999999, zMing=99999999, zMaxg=-99999999;


    for(unsigned i=0; i<_fileTopologies.getValue().size(); ++i)
    {

        std::string filename = _fileTopologies.getValue()[i];

        serr<<"SparseGridMultipleTopology open "<<filename<<sendl;

        if (! sofa::helper::system::DataRepository.findFile ( filename ))
            continue;

        if( (filename.length() > 4 && filename.compare(filename.length()-4, 4, ".obj")==0) || (filename.length() > 6 && filename.compare(filename.length()-6, 6, ".trian")==0))
        {
            meshes[i] = helper::io::Mesh::Create(filename.c_str());

            if(meshes[i] == NULL)
            {
                serr << "SparseGridTopology: loading mesh " << filename << " failed." <<sendl;
                return;
            }


            SReal xMin, xMax, yMin, yMax, zMin, zMax;
            computeBoundingBox(meshes[i]->getVertices(), xMin, xMax, yMin, yMax, zMin, zMax);


            if( xMin < xMing ) xMing=xMin;
            if( yMin < yMing ) yMing=yMin;
            if( zMin < zMing ) zMing=zMin;
            if( xMax > xMaxg ) xMaxg=xMax;
            if( yMax > yMaxg ) yMaxg=yMax;
            if( zMax > zMaxg ) zMaxg=zMax;
        }
    }

    if( _min.getValue()== Vector3() && _max.getValue()== Vector3())
    {
        // increase the box a little
        Vector3 diff ( xMaxg-xMing, yMaxg - yMing, zMaxg - zMing );
        diff /= 100.0;

        _min.setValue(Vector3( xMing - diff[0], yMing - diff[1], zMing - diff[2] ));
        _max.setValue(Vector3( xMaxg + diff[0], yMaxg + diff[1], zMaxg + diff[2] ));
    }

    for(unsigned i=0; i<_fileTopologies.getValue().size(); ++i)
    {
        if(meshes[i]) buildFromTriangleMesh(meshes[i],i);
    }


    helper::vector<Type> regularGridTypes; // to compute filling types (OUTSIDE, INSIDE, BOUNDARY)
    helper::vector< float > regularStiffnessCoefs,regularMassCoefs;

    assembleRegularGrids( regularGridTypes, regularStiffnessCoefs, regularMassCoefs );

    buildFromRegularGridTypes(_regularGrid, regularGridTypes);

    _stiffnessCoefs.resize( this->getNbHexas());
    _massCoefs.resize( this->getNbHexas());
    for(int i=0; i<this->getNbHexas(); ++i)
    {
        _stiffnessCoefs[i] = regularStiffnessCoefs[ this->_indicesOfCubeinRegularGrid[i] ];
        _massCoefs[i] = regularMassCoefs[ this->_indicesOfCubeinRegularGrid[i] ];
    }

    if(_computeRamifications.getValue())
    {

        if( _finestConnectivity.getValue() || this->isVirtual || _nbVirtualFinerLevels.getValue() > 0 )
        {
            // find the connexion graph between the finest hexas
            findConnexionsAtFinestLevel();
        }

        if( _finestConnectivity.getValue() )
        {

            buildRamifiedFinestLevel();
        }
    }
}


void SparseGridMultipleTopology::buildFromTriangleMesh(helper::io::Mesh* mesh, unsigned fileIdx)
{
    _regularGrids[fileIdx]->setSize(this->getNx(),this->getNy(),this->getNz());
    _regularGrids[fileIdx]->setPos(this->getXmin(),this->getXmax(),this->getYmin(),this->getYmax(),this->getZmin(),this->getZmax());

    voxelizeTriangleMesh(mesh, *_regularGrids[fileIdx], _regularGridTypes[fileIdx]);

    delete mesh;
}



void SparseGridMultipleTopology::assembleRegularGrids(helper::vector<Type>& regularGridTypes,helper::vector< float >& regularStiffnessCoefs,helper::vector< float >& regularMassCoefs)
{
    _regularGrid.setSize(getNx(),getNy(),getNz());
    _regularGrid.setPos(getXmin(),getXmax(),getYmin(),getYmax(),getZmin(),getZmax());

    regularGridTypes.resize( _regularGridTypes[0].size() );
    regularGridTypes.fill(OUTSIDE);
    regularStiffnessCoefs.resize( _regularGridTypes[0].size() );
    regularMassCoefs.resize( _regularGridTypes[0].size() );

    for(unsigned i=0; i<_regularGrids.size(); ++i)
    {
        for(int w=0; w<_regularGrids[i]->getNbHexas(); ++w)
        {
            if( _regularGridTypes[i][w] == INSIDE || (_regularGridTypes[i][w] == BOUNDARY && !this->_fillWeighted.getValue()) )
            {
                regularGridTypes[w] = INSIDE;
                regularStiffnessCoefs[w] = _dataStiffnessCoefs.getValue()[i];
                regularMassCoefs[w] = _dataMassCoefs.getValue()[i];
            }
            else if(  _regularGridTypes[i][w] == BOUNDARY && this->_fillWeighted.getValue() )
            {
                if( regularGridTypes[w] != INSIDE ) regularGridTypes[w] = BOUNDARY;
                regularStiffnessCoefs[w] = (regularStiffnessCoefs[w]+_dataStiffnessCoefs.getValue()[i]) * .5;
                regularMassCoefs[w] = (regularMassCoefs[w]+_dataMassCoefs.getValue()[i]) * .5;
            }
        }
    }
}




void SparseGridMultipleTopology::buildVirtualFinerLevels()
{
    int nb = _nbVirtualFinerLevels.getValue();

    _virtualFinerLevels.resize(nb);

    int newnx=n.getValue()[0],newny=n.getValue()[1],newnz=n.getValue()[2];
    for( int i=0; i<nb; ++i)
    {
        newnx = (newnx-1)*2+1;
        newny = (newny-1)*2+1;
        newnz = (newnz-1)*2+1;
    }

    SparseGridMultipleTopology* sgmt = new SparseGridMultipleTopology(true);;

    _virtualFinerLevels[0] = sgmt;
    _virtualFinerLevels[0]->setNx( newnx );
    _virtualFinerLevels[0]->setNy( newny );
    _virtualFinerLevels[0]->setNz( newnz );
    _virtualFinerLevels[0]->setMin( _min.getValue() );
    _virtualFinerLevels[0]->setMax( _max.getValue() );
    std::stringstream nameg; nameg << "virtual grid "<< 0;
    _virtualFinerLevels[0]->setName( nameg.str().c_str() );
    _virtualFinerLevels[0]->setContext( this->getContext() );

    _virtualFinerLevels[0]->load(this->fileTopology.getValue().c_str());
    sgmt->_fileTopologies.setValue(this->_fileTopologies.getValue());
    sgmt->_dataStiffnessCoefs.setValue(this->_dataStiffnessCoefs.getValue());
    sgmt->_dataMassCoefs.setValue(this->_dataMassCoefs.getValue());
    sgmt->_finestConnectivity.setValue( _finestConnectivity.getValue() );
    _virtualFinerLevels[0]->init();

    serr<<"SparseGridTopology "<<getName()<<" buildVirtualFinerLevels : ";
    serr<<"("<<newnx<<"x"<<newny<<"x"<<newnz<<") -> "<< _virtualFinerLevels[0]->getNbHexas() <<" elements , ";

    for(int i=1; i<nb; ++i)
    {
        _virtualFinerLevels[i] = new SparseGridMultipleTopology(true);

        std::stringstream nameg; nameg << "virtual grid "<< i;
        _virtualFinerLevels[i]->setName( nameg.str().c_str() );


        _virtualFinerLevels[i]->setFinerSparseGrid(_virtualFinerLevels[i-1]);

        _virtualFinerLevels[i]->init();


        serr<<"("<<_virtualFinerLevels[i]->getNx()<<"x"<<_virtualFinerLevels[i]->getNy()<<"x"<<_virtualFinerLevels[i]->getNz()<<") -> "<< _virtualFinerLevels[i]->getNbHexas() <<" elements , ";
    }

    serr<<sendl;

    this->setFinerSparseGrid(_virtualFinerLevels[nb-1]);


}



}
}
}
