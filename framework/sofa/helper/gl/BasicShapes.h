#ifndef SOFA_HELPER_GL_BASICSHAPES_H
#define SOFA_HELPER_GL_BASICSHAPES_H

#include <sofa/helper/gl/template.h>

namespace sofa
{

namespace helper
{

namespace gl
{

using namespace sofa::defaulttype;


template <typename V>
void drawCone(const typename V& p1, const typename V& p2, const float& radius1, const float& radius2, const int subd=8)
{
    V tmp = p2-p1;

    /* create Vectors p and q, co-planar with the cylinder's cross-sectional disk */
    V p=tmp;
    if (p[0] == 0.0 && p[1] == 0.0)
        p[0] += 1.0;
    else
        p[2] += 1.0;
    V q;
    q = p.cross(tmp);
    p = tmp.cross(q);
    /* do the normalization outside the segment loop */
    p.normalize();
    q.normalize();

    int i2;
    float theta, st, ct;
    /* build the cylinder from rectangular subd */
    glBegin(GL_QUAD_STRIP);
    for (i2=0 ; i2<=subd ; i2++)
    {
        /* sweep out a circle */
        theta =  i2 * 2.0 * 3.14 / subd;
        st = sin(theta);
        ct = cos(theta);
        /* construct normal */
        tmp = p*ct+q*st;
        /* set the normal for the two subseqent points */
        helper::gl::glNormalT(tmp);
        /* point on disk 1 */
        V w(p1);
        w += tmp*radius1;
        helper::gl::glVertexT(w);
        /* point on disk 2 */
        w=p2;
        w += tmp*radius2;
        helper::gl::glVertexT(w);
    }
    glEnd();
}


template <typename V>
void drawCylinder(const V& p1, const V& p2, const float& rad, const int subd=8)
{
    drawCone( p1,p2,rad,rad,subd);
}


template <typename V>
void drawArrow(const V& p1, const V& p2, const float& rad, const int subd=8)
{
    V p3 = p1*.2+p2*.8;
    drawCylinder( p1,p3,rad,subd);
    drawCone( p3,p2,rad*2.5,0,subd);
}


template <typename V>
void drawSphere(const V& center, const float& rad, const int subd1=8, const int subd2=8)
{
    GLUquadricObj*	sphere = gluNewQuadric();
    gluQuadricDrawStyle(sphere, GLU_FILL);
    gluQuadricOrientation(sphere, GLU_OUTSIDE);
    gluQuadricNormals(sphere, GLU_SMOOTH);
    glPushMatrix();
    helper::gl::glTranslateT( center );
    gluSphere(sphere,2.0*rad,subd1,subd2);
    glPopMatrix();
// 		delete sphere;
}


}
}
}
#endif
