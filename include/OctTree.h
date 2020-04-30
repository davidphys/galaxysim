#ifndef QUADTREE_H
#define QUADTREE_H

#include "ParticleMan.h" //for PointMass
#include <vector>
#define PARTICLES_PER_QUADNODE 8
#define EPS2 1e-4

/* A OctNode is anything but the top node of a QuadTree.
 * */
class OctNode {

    float totalm,totalmx,totalmy,totalmz;
    std::vector<PointMass*> pointmasses;
    OctNode** subnodes;

    float x0,y0,z0;
    float x1,y1,z1;

    enum StatusEnum {
        Filling,//0<nparticles<N_PARTICLES_PER_QUADNODE.
        Empty, //no particles. subnodes has no pointers.
        Branch, //has subnodes, but no actual element.
        Filled, //Exactly PARTICLES_PER_QUADNODE particles. Node is a leaf and subnodes has no pointers.
    } status;

    float diffw2;

    void fillSubnodes();
    void fillSubnode(int i);
    void deleteSubnodes();

public:
    OctNode(float x0,float y0, float z0, float x1, float y1, float z1);
    ~OctNode();

    void setLowerCorner(float x0,float y0,float z0);
    void setUpperCorner(float x1,float y1,float z1);
    void addParticle(PointMass *p);
    void calculateAcceleration(PointMass *p) const;
    

};
#endif
