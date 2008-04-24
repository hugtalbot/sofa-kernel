/*******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 1       *
*                (c) 2006-2007 MGH, INRIA, USTL, UJF, CNRS                     *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Contact information: contact@sofa-framework.org                              *
*                                                                              *
* Authors: J. Allard, P-J. Bensoussan, S. Cotin, C. Duriez, H. Delingette,     *
* F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza, M. Nesme, P. Neumann,        *
* and F. Poyer                                                                 *
*******************************************************************************/
#include <sofa/component/behaviormodel/eulerianfluid/Fluid2D.h>
#include <sofa/helper/gl/template.h>
#include <sofa/core/ObjectFactory.h>
#include <iostream>
#include <string.h>

namespace sofa
{

namespace component
{

namespace behaviormodel
{

namespace eulerianfluid
{

SOFA_DECL_CLASS(Fluid2D)

int Fluid2DClass = core::RegisterObject("Eulerian 2D fluid")
        .add< Fluid2D >()
        .addLicense("LGPL")
        .addAuthor("Jeremie Allard")
        ;

Fluid2D::Fluid2D()
    : nx(16), ny(16), cellwidth(1.0f),
      f_nx ( initDataPtr(&f_nx, &nx, "nx", "grid size along x axis") ),
      f_ny ( initDataPtr(&f_ny, &ny, "ny", "grid size along y axis") ),
      f_cellwidth ( initDataPtr(&f_cellwidth, &cellwidth, "cellwidth", "width of each cell") ),
      f_height ( initData(&f_height, 5.0f, "height", "initial fluid height") ),
      f_dir ( initData(&f_dir, vec2(0,1), "dir", "initial fluid surface normal") ),
      f_tstart ( initData(&f_tstart, 0.0f, "tstart", "starting time for fluid source") ),
      f_tstop ( initData(&f_tstop, 60.0f, "tstop", "stopping time for fluid source") )
{
    fluid = new Grid2D;
    fnext = new Grid2D;
    ftemp = new Grid2D;
}

Fluid2D::~Fluid2D()
{
    delete fluid;
    delete fnext;
    delete ftemp;
}

void Fluid2D::init()
{
    fluid->clear(nx,ny);
    fnext->clear(nx,ny);
    ftemp->clear(nx,ny);
    if (f_height.getValue() != 0)
    {
        //fluid->seed(f_height.getValue());
        fluid->seed(f_height.getValue(), f_dir.getValue());
        //fluid->seed(vec2(3.5,3.5), vec2(12.5,8.5));
    }
    fluid->t = -f_tstart.getValue();
    fluid->tend = f_tstop.getValue() - f_tstart.getValue();
}

void Fluid2D::reset()
{
    init();
}

void Fluid2D::updatePosition(double dt)
{
    fnext->step(fluid, ftemp, (real)dt);
    Grid2D* p = fluid; fluid=fnext; fnext=p;
}

void Fluid2D::drawVisual()
{
    glPushMatrix();
    glTranslatef(-(nx-1)*cellwidth/2,-(ny-1)*cellwidth/2,0.0f);
    glScalef(cellwidth,cellwidth,cellwidth);
    //if (getContext()->getShowBehaviorModels())
    {
        glDisable(GL_LIGHTING);
        glColor4f(1,1,1,1);
        glBegin(GL_LINES);
        glVertex2i(    0,    0 ); glVertex2i( nx-1,    0 );
        glVertex2i(    0, ny-1 ); glVertex2i( nx-1, ny-1 );

        glVertex2i(    0,    0 ); glVertex2i(    0, ny-1 );
        glVertex2i( nx-1,    0 ); glVertex2i( nx-1, ny-1 );
        glEnd();
    }
    if (getContext()->getShowBehaviorModels())
    {
        glDisable(GL_LIGHTING);
        const real s = (real)getContext()->getDt()*5;
        glBegin(GL_LINES);
        for (int y=0; y<ny; y++)
            for (int x=0; x<nx; x++)
            {
                vec2 u = fluid->get(x,y)->u;
                real r;
                r = u[0]*s;
                if (rabs(r) > 0.001f)
                {
                    if (r>0.9f) r=0.9f;
                    glColor4f(1,0,0,1);
                    glVertex2f((float)x-0.5f       , (float)y);
                    glVertex2f((float)x-0.5f+     r, (float)y);
                    glVertex2f((float)x-0.5f+     r, (float)y);
                    glVertex2f((float)x-0.5f+0.8f*r, (float)y+0.2f*r);
                    glVertex2f((float)x-0.5f+     r, (float)y);
                    glVertex2f((float)x-0.5f+0.8f*r, (float)y-0.2f*r);
                }
                r = u[1]*s;
                if (rabs(r) > 0.001f)
                {
                    if (r>0.9f) r=0.9f;
                    glColor4f(0,1,0,1);
                    glVertex2f((float)x, y-0.5f  );
                    glVertex2f((float)x, y-0.5f+r);
                    glVertex2f((float)x, y-0.5f+r);
                    glVertex2f((float)x+0.2f*r, y-0.5f+0.8f*r);
                    glVertex2f((float)x, y-0.5f+r);
                    glVertex2f((float)x-0.2f*(float)r, y-0.5f+0.8f*r);
                }
            }
        glEnd();
        glPointSize(3);
        glBegin(GL_POINTS);
        for (int y=0; y<ny; y++)
            for (int x=0; x<nx; x++)
            {
                real l = *fluid->getlevelset(x,y);
                if (rabs(l)>=5) continue;
                if (l<0)
                {
                    glColor4f(0,1+l/5,1+l/5,1);
                }
                else
                {
                    glColor4f(1-l/5,1-l/5,0,1);
                }
                glVertex2i(x,y);
            }
        glEnd();
        glPointSize(1);
    }
    if (getContext()->getShowVisualModels())
    {
        if (getContext()->getShowWireFrame())
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glEnable(GL_LIGHTING);

        static const float color[4] = { 0.0f, 1.0f, 1.0f, 1.0f};
        glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, color);
        static const float emissive[4] = { 0.0f, 0.0f, 0.0f, 0.0f};
        static const float specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f};
        glMaterialfv (GL_FRONT_AND_BACK, GL_EMISSION, emissive);
        glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, specular);
        glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 20);

        glBegin(GL_TRIANGLES);
        for (unsigned int i=0; i<facets.size(); i++)
        {
            for (int j=0; j<3; j++)
            {
                int idx = facets[i].p[j];
                glNormal3fv(points[idx].n.ptr());
                glVertex3fv(points[idx].p.ptr());
            }
        }
        glEnd();

        glDisable(GL_LIGHTING);
        if (getContext()->getShowWireFrame())
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glPopMatrix();
}

void Fluid2D::updateVisual()
{
    points.clear();
    facets.clear();

    const real* data = fluid->levelset;

    planes.resize(2*nx*ny);
    CubeData *P = &(planes[0]);
    CubeData *P0 = P;
    CubeData *P1 = P+(nx*ny);
    //P0 = planes.begin()+0;
    //P1 = planes.begin()+nx*ny;

    //////// MARCHING CUBE ////////

    const real isoval = 0.0f;
    const real iso = isoval;

    const int dx = 1;
    const int dy = nx;
    const int dz = 0; //nx*ny;
    const int nz = 2;
    const real data2 = 5;

    int x,y,z,i,mk;
    const int *tri;


    i=0;
    // First plane
    memset(P1,-1,nx*ny*sizeof(CubeData));
    {
        z=0;
        // First line
        {
            y=0;
            ++i;
            for(x=1; x<nx; x++)
            {
                if ((data[i]>=isoval)^(data[i-dx]>=isoval))
                {
                    P1[i].p[0] = addPoint<0>(x,y,z,data[i],data[i-dx],iso);
                }
                ++i;
            }
        }
        for(y=1; y<ny; y++)
        {
            // First column
            x=0;
            if ((data[i]>=isoval)^(data[i-dy]>=isoval))
            {
                P1[i].p[1] = addPoint<1>(x,y,z,data[i],data[i-dy],iso);
            }
            ++i;

            for(x=1; x<nx; x++)
            {
                if ((data[i]>=isoval)^(data[i-dx]>=isoval))
                {
                    P1[i].p[0] = addPoint<0>(x,y,z,data[i],data[i-dx],iso);
                }
                if ((data[i]>=isoval)^(data[i-dy]>=isoval))
                {
                    P1[i].p[1] = addPoint<1>(x,y,z,data[i],data[i-dy],iso);
                }
                ++i;
            }
        }
    }
    for (z=1; z<nz; z++)
    {
        i=0; data+=dz;

        { CubeData* p=P0; P0=P1; P1=p; }
        memset(P1,-1,nx*ny*sizeof(CubeData));

        int edgepts[12];
        int* base = &(P[0].p[0]);
        edgepts[0] = &(P0[-dy].p[0])-base;
        edgepts[1] = &(P0[0  ].p[1])-base;
        edgepts[2] = &(P0[0  ].p[0])-base;
        edgepts[3] = &(P0[-dx].p[1])-base;
        edgepts[4] = &(P1[-dy].p[0])-base;
        edgepts[5] = &(P1[0  ].p[1])-base;
        edgepts[6] = &(P1[0  ].p[0])-base;
        edgepts[7] = &(P1[-dx].p[1])-base;
        edgepts[8] =  &(P1[-dx-dy].p[2])-base;
        edgepts[9] =  &(P1[   -dy].p[2])-base;
        edgepts[10] = &(P1[0     ].p[2])-base;
        edgepts[11] = &(P1[-dx   ].p[2])-base;

        // First line
        {
            y=0;
            x=0;
            if ((data2/*data[i]*/>=isoval)^(data[i-dz]>=isoval))
            {
                P1[i].p[2] = addPoint<2>(x,y,z,data2/*data[i]*/,data[i-dz],iso);
            }
            ++i;
            for(x=1; x<nx; x++)
            {
                //if ((data[i]>=isoval)^(data[i-dx]>=isoval))
                //{
                //    P1[i].p[0] = addPoint<0>(x,y,z,data[i],data[i-dx],iso);
                //}
                if ((data2/*data[i]*/>=isoval)^(data[i-dz]>=isoval))
                {
                    P1[i].p[2] = addPoint<2>(x,y,z,data2/*data[i]*/,data[i-dz],iso);
                }
                ++i;
            }
        }
        for(y=1; y<ny; y++)
        {
            // First column
            x=0;
            //if ((data[i]>=isoval)^(data[i-dy]>=isoval))
            //{
            //    P1[i].p[1] = addPoint<1>(x,y,z,data[i],data[i-dy],iso);
            //}
            if ((data2/*data[i]*/>=isoval)^(data[i-dz]>=isoval))
            {
                P1[i].p[2] = addPoint<2>(x,y,z,data2/*data[i]*/,data[i-dz],iso);
            }
            ++i;

            for(x=1; x<nx; x++)
            {
                //if ((data[i]>=isoval)^(data[i-dx]>=isoval))
                //{
                //    P1[i].p[0] = addPoint<0>(x,y,z,data[i],data[i-dx],iso);
                //}
                //if ((data[i]>=isoval)^(data[i-dy]>=isoval))
                //{
                //    P1[i].p[1] = addPoint<1>(x,y,z,data[i],data[i-dy],iso);
                //}
                if ((data2/*data[i]*/>=isoval)^(data[i-dz]>=isoval))
                {
                    P1[i].p[2] = addPoint<2>(x,y,z,data2/*data[i]*/,data[i-dz],iso);
                }

                // All points should now be created

                if (data[i-dx-dy-dz]>=iso) mk = 1; else mk=0;
                if (data[i   -dy-dz]>=iso) mk|= 2;
                if (data[i      -dz]>=iso) mk|= 4;
                if (data[i-dx   -dz]>=iso) mk|= 8;
                if (data2/*data[i-dx-dy   ]*/>=iso) mk|= 16;
                if (data2/*data[i   -dy   ]*/>=iso) mk|= 32;
                if (data2/*data[i         ]*/>=iso) mk|= 64;
                if (data2/*data[i-dx      ]*/>=iso) mk|= 128;

                tri= component::mapping::MarchingCubeTriTable[mk];
                while (*tri>=0)
                {
                    int* b = base+3*i;
                    if (addFace(b[edgepts[tri[0]]],b[edgepts[tri[1]]],b[edgepts[tri[2]]])<0)
                    {
                        std::cerr << "  mk=0x"<<std::hex<<mk<<std::dec<<" p1="<<tri[0]<<" p2="<<tri[1]<<" p3="<<tri[2]<<std::endl;
                        for (int e=0; e<12; e++) std::cerr << "  e"<<e<<"="<<b[edgepts[e]];
                        std::cerr<<std::endl;
                        for (int ddz=-1; ddz<=0; ddz++)
                            for (int ddy=-1; ddy<=0; ddy++)
                                for (int ddx=-1; ddx<=0; ddx++)
                                {
                                    std::cerr << " val("<<x+ddx<<','<<y+ddy<<','<<z+ddz<<")="<<(double)data[i+ddx*dx+ddy*dy+ddz*dz]<<std::endl;
                                }
                    }
                    tri+=3;
                }
                ++i;
            }
        }
    }
    //std::cout << points.size() << " points, "<<facets.size() <<" faces"<<std::endl;

    for (unsigned int i=0; i<points.size(); i++)
    {
        points[i].n.clear();
    }

    for (unsigned int i=0; i<facets.size(); i++)
    {
        Vec3f n = cross(points[facets[i].p[1]].p-points[facets[i].p[0]].p,points[facets[i].p[2]].p-points[facets[i].p[0]].p);
        n.normalize();
        points[facets[i].p[0]].n += n;
        points[facets[i].p[1]].n += n;
        points[facets[i].p[2]].n += n;
    }

    for (unsigned int i=0; i<points.size(); i++)
        points[i].n.normalize();
}

bool Fluid2D::addBBox(Real_Sofa* minBBox, Real_Sofa* maxBBox)
{
    Real_Sofa size[3] = { (nx-1)*cellwidth, (ny-1)*cellwidth, cellwidth/2 };
    Real_Sofa pos[3] = { -size[0]/2, -size[1]/2, 0 };
    for (int c=0; c<3; c++)
    {
        if (minBBox[c] > pos[c]        ) minBBox[c] = pos[c];
        if (maxBBox[c] < pos[c]+size[c]) maxBBox[c] = pos[c]+size[c];
    }
    return true;
}

} // namespace eulerianfluid

} // namespace behaviormodel

} // namespace component

} // namespace sofa

