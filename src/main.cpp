#include "ParticleMan.h"
#include <iostream>
#include "OctTree.h"
#include <sstream>
#include <iomanip>

//Some string manipulation functions for saving files. pad_int(1234,5) returns "01234".
std::string pad_int(int arg, int padcount) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(padcount) << arg;
    return ss.str();
}

//Returns a file name in the form of "prefix00###suffix". For example "image0032.bmp"
std::string getFilename(std::string prefix, int num, int padcount, std::string suffix) {
    return prefix + pad_int(num, padcount) + suffix;
}

void integrate(ParticleManager &pman,float dt){

    //Construct the quadtree
    float x0,y0,z0,x1,y1,z1;



    getBounds(pman.masses,x0,y0,z0,x1,y1,z1);
    OctNode basenode(x0,y0,z0,x1,y1,z1);
    for(size_t i=0;i<pman.masses.size();i++){
        basenode.addParticle(&pman.masses[i]);
    }

    //Step forward in position. We can also take this opportunity to zero out the accelerations.
    for(size_t i=0;i<pman.masses.size();i++){
        pman.masses[i].stepPosition(dt);
        pman.masses[i].zeroAcceleration();
    }

    //Calculate all the accelerations.
#pragma omp parallel for shared(basenode,pman)
    for(size_t i=0;i<pman.masses.size();i++){
        basenode.calculateAcceleration(&pman.masses[i]);
    }

    //Step all the velocities
    for(size_t i=0;i<pman.masses.size();i++){
        pman.masses[i].stepVelocity(dt);
    }
}

int main(){
    ParticleManager pman;
    int nparticles=400000;


    //Set the total particle mass to be 1
    pman.setCursorParticleMass(1.0/nparticles);
    //Set the disk to be slightly rotated
    pman.setCursorDirection(0,0.1,1);

    //Set the cursor at the origin
    pman.setCursorPosition(0,0,0);
    //Create a slowly rotating disk
    pman.placeBall(nparticles,1.0,0.6);


    float dt=0.004;

    for(int k=0;k<1;k++){
        for(int i=0;i<3;i++){
            integrate(pman,dt);
        }
        pman.saveParticlesAsImage(400,400,getFilename("img2/output",k,3,".bmp"),0,0,400.0/(3.0));
    }
    return 0;
}
