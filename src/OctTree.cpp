#include "OctTree.h"
#include <iostream>
#include <math.h>
using namespace std;

void OctNode::fillSubnodes(){
#ifdef TREE_DEBUG
    if(subnodes!=nullptr){
        cerr<<"Error in QuadTree::fillSubnodes. Subnodes already populated."<<endl;
        return;
    }
#endif
    subnodes=new OctNode*[PARTICLES_PER_QUADNODE];
    float cx=float(0.5)*(x0+x1);
    float cy=float(0.5)*(y0+y1);
    float cz=float(0.5)*(z0+z1);

    //We have to construct 8 subnodes, which is easy.
    //Unfortunately we also have to populate them with their upper and lower bounds. This can be done in each
    //dimension separately, and by taking advantage of our indexing scheme.
    //
    //The variable "index" is defined as follows.
    //The index is going to be 0<=index<8. In binary, the rightmost bit is 0 if we're in the lower cell or
    //1 if we're in the upper cell. The second rightmost bit corresponds to y. The third rightmost bit to z.
    for(int zdim=0;zdim<2;zdim++){
        float z0new=(zdim==0)?z0:cz;
        float z1new=(zdim==0)?cz:z1;
        for(int ydim=0;ydim<2;ydim++){
            float y0new=(ydim==0)?y0:cy;
            float y1new=(ydim==0)?cy:y1;
            for(int xdim=0;xdim<2;xdim++){
                float x0new=(xdim==0)?x0:cx;
                float x1new=(xdim==0)?cx:x1;
                int index=(zdim<<2)|(ydim<<1)|xdim;
                subnodes[index]=new OctNode(x0new,y0new,z0new,x1new,y1new,z1new);
            }
        }
    }
}

void OctNode::deleteSubnodes(){
    if(subnodes!=nullptr){
        for(int i=0;i<8;i++){
            if(subnodes[i]!=nullptr)
                delete subnodes[i];
        }
        delete []subnodes;
    }
}

OctNode::OctNode(float x0,float y0, float z0, float x1, float y1, float z1) : totalm(0),totalmx(0),totalmy(0),totalmz(0),pointmasses(),subnodes(nullptr),x0(x0),y0(y0),z0(z0),x1(x1),y1(y1),z1(z1),status(Empty),diffw2(0) { 
    diffw2=(x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0);
}
OctNode::~OctNode() { deleteSubnodes(); }

void OctNode::setLowerCorner(float x0,float y0,float z0){ this->x0=x0; this->y0=y0; this->z0=z0; }
void OctNode::setUpperCorner(float x1,float y1,float z1){ this->x1=x1; this->y1=y1; this->z1=z1; }
void OctNode::addParticle(PointMass *p){
#ifdef TREE_DEBUG
    if(p->x<x0 || p->y<y0 || p->z<z0 || p->x>x1 || p->y>y1 || p->z>z1){
        cerr<<"Error in OctNode::addParticle. Particle outside quadnode bounds!" << p->x<<" "<<x0<<" "<<x1<<endl;
    }
#endif

    switch(status){
        case Filling:
        case Empty:
            //If the quadtree node is empty or is still filling, we just have to add more particles and update our state variables keeping track of the center of mass.
            pointmasses.push_back(p);
            totalmx+=p->x*p->m;
            totalmy+=p->y*p->m;
            totalmz+=p->z*p->m;
            totalm+=p->m;
            if(pointmasses.size()==PARTICLES_PER_QUADNODE){
                status=Filled;
            } else if(pointmasses.size()>0){
                status=Filling;
            }
            break;
        case Branch:
            {
                //If we're in a branch, we just have to divvy up the work to the appropriate subnode.
                float cx=float(0.5)*(x0+x1);
                float cy=float(0.5)*(y0+y1);
                float cz=float(0.5)*(z0+z1);
                int index=((int)(p->z>cz)<<2)|((int)(p->y>cy)<<1)|((int)(p->x>cx));
#ifdef TREE_DEBUG
                if(subnodes==nullptr || subnodes[index]==nullptr){
                    cerr<<"Error in QuadTree::addPointMass. subnodes not populated even though we're adding to a subnode!"<<endl;
                    return;
                }
#endif
                subnodes[index]->addParticle(p);
                totalmx+=p->x*p->m;
                totalmy+=p->y*p->m;
                totalmz+=p->z*p->m;
                totalm+=p->m;
            }
            break;
        case Filled:
            status=Branch;
            fillSubnodes();
            for(size_t i=0;i<pointmasses.size();i++){
                this->addParticle(pointmasses[i]);
            }
            this->addParticle(p);
            pointmasses.clear();
            break;
    }
}

void OctNode::calculateAcceleration(PointMass *p) const{
    switch(status) {
        case Empty:
            break;
        case Filling:
        case Filled:
            //If we've reached a bottom node, we just have to do the Newtonian force calculation for every particle.
            for(size_t i=0;i<pointmasses.size();i++){
                if(p!=pointmasses[i]){
                    //Do the distance gravitational calculation!
                    float diffx=pointmasses[i]->x-p->x;
                    float diffy=pointmasses[i]->y-p->y;
                    float diffz=pointmasses[i]->z-p->z;
                    float d=sqrt(diffx*diffx+diffy*diffy+diffz*diffz+EPS2);
                    d=pointmasses[i]->m/(d*d*d);
                    p->ax+=diffx*d;
                    p->ay+=diffy*d;
                    p->az+=diffz*d;
                }
            }
            break;
        case Branch:
            {
                //If we've reached a branch node, we can either branch or continue (this is the Barnes-Hut branching criterion.)
                float diffx=totalmx/totalm-p->x;
                float diffy=totalmy/totalm-p->y;
                float diffz=totalmz/totalm-p->z;
                float d=diffx*diffx+diffy*diffy+diffz*diffz;
                if(diffw2>=d*0.8){
                    for(int index=0;index<8;index++){
#ifdef TREE_DEBUG
    if(subnodes==nullptr || subnodes[index]==nullptr){
        cerr<<"Error in OctNode::calculateAcceleration. subnodes not defined/populated!"<<endl;
    }
#endif
                        subnodes[index]->calculateAcceleration(p);
                    }
                } else {
                    //Do the distance gravitational calculation!
                    d+=EPS2;
                    d=sqrt(d);
                    d=totalm/(d*d*d);
                    p->ax+=diffx*d;
                    p->ay+=diffy*d;
                    p->az+=diffz*d;
                }
            }
            break;
    }
}
