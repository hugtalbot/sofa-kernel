# helper functions for treating float lists as quaternions
# author: maxime.tournier@inria.fr

import sys

import Vec
import math

def id():
    return [0, 0, 0, 1]

def conj(q):
    return [-q[0], -q[1], -q[2], q[3]]

def inv(q):
    return Vec.scal(1 / Vec.norm2(q), conj(q) )

def re(q):
    return q[3]

def im(q):
    return q[:3]

# TODO optimize
def prod(a, b):
    return Vec.sum( Vec.sum( Vec.scal( re(a), im(b) ),
                             Vec.scal( re(b), im(a) )),
                    Vec.cross( im(a), im(b) ) ) + [ re(a) * re(b) - Vec.dot( im(a), im(b) ) ]

# TODO optimize
def rotate(q, x):
    # TODO assert q is unit
    return im( prod(q, prod( x + [0], conj(q))) )


def exp(v):
    theta = Vec.norm(v)
    s = math.sin(theta / 2)
    c = math.cos(theta / 2)

    return [ v[0] / theta * s,
             v[1] / theta * s,
             v[2] / theta * s,
             c ]
             
def axisToQuat(axis, phi):
    """ return the quaternion corresponding to rotation around vector axis with angle phi
    """
    axis_norm = Vec.norm(axis)
    if axis_norm < sys.float_info.epsilon:
        return id()
    axis = Vec.scal(1./axis_norm, axis)
    return [ axis[0]*math.sin(phi/2),
             axis[1]*math.sin(phi/2),
             axis[2]*math.sin(phi/2),
             math.cos(phi/2) ]

def quatToAxis(q):
    """ Return rotation vector corresponding to unit quaternion q in the form of [axis, angle]
    """
    sine  = math.sin( math.acos(q[3]) );

    if (math.fabs(sine) < sys.float_info.epsilon) :
        axis = [0.0,1.0,0.0]
    else :
        axis = Vec.scal(1/sine, q[0:3])
    phi =  math.acos(q[3]) * 2.0
    return [axis, phi]
