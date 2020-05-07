#include <iostream>
#include <sstream>
#include <iomanip>

#include "OctTree.h"
#include "CLITools.h"
#include "ParticleMan.h"

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

int main(int argc, char *argv[]){

    if(argc<=1){
        std::cerr<<"No command line options given! Valid options are:"<<std::endl;
        std::cerr<<"    -dt float (delta t for each substep)"<<std::endl;
        std::cerr<<"    -nout int (number of output files to produce)"<<std::endl;
        std::cerr<<"    -ksteps int (number of timesteps to take between each output file)"<<std::endl;
        std::cerr<<"    -txtdir string (what directory to output text files. If no directory is given, no text file output will be produced.)"<<std::endl;
        std::cerr<<"    -imgdir string (what directory to output image files. If no directory is given, no image file output will be produced.)"<<std::endl;
        std::cerr<<"    -in string (what file to take in. The file is expected to be of 1 particle per line, m x y z vx vy vz.)"<<std::endl;
        std::cerr<<"    -box int (Option to create a unit mass 3D unit box of n particles)"<<std::endl;
        std::cerr<<"    -square int (Option to create a unit mass 2D unit square of n particles)"<<std::endl;
        std::cerr<<"    -disk int (Option to create a unit mass 2D unit disk of n particles)"<<std::endl;
        std::cerr<<"    -sphere int (Option to create a unit mass 3D unit sphere of n particles)"<<std::endl;
        exit(3);
    }
    
    CLITools::CLIArgs runparams=CLITools::parseCLIArgs(argc,argv);

    //Add things to the particle manager:
    ParticleManager pman;
    if(!runparams.infile.empty())
        pman.load(runparams.infile);

    pman.setCursorPosition(0,0,0);
    pman.setCursorVelocity(0,0,0);
    pman.setCursorDirection(0,0,1);
    if(runparams.box>0){
        pman.setCursorParticleMass(1.0/runparams.box);
        pman.placeBox(runparams.square,-1,-1,-1,1,1,1);
    }

    if(runparams.square>0){
        pman.setCursorParticleMass(1.0/runparams.square);
        pman.placeBox(runparams.square,-1,-1,0,1,1,0);
    }

    if(runparams.disk>0){
        pman.setCursorParticleMass(1.0/runparams.disk);
        pman.placeDisk(runparams.disk,1.0);
    }

    if(runparams.sphere>0){
        pman.setCursorParticleMass(1.0/runparams.sphere);
        pman.placeBall(runparams.sphere,1.0);
    }

    for(int k=0;k<runparams.nout;k++){
        for(int i=0;i<runparams.ksteps;i++){
            integrate(pman,runparams.dt);
        }
        if(!runparams.imgdir.empty())
            pman.saveParticlesAsImage(400,400,getFilename(runparams.imgdir+"/out",k,3,".bmp"),0,0,400.0/(3.0));

        if(!runparams.txtdir.empty())
            pman.save(getFilename(runparams.txtdir+"/out",k,3,".txt"));
    }
    return 0;
}

    /*
    int nparticles=400000;
    //Set the total particle mass to be 1
    pman.setCursorParticleMass(1.0/nparticles);
    //Set the disk to be slightly rotated
    pman.setCursorDirection(0,0.1,1);

    //Set the cursor at the origin
    pman.setCursorPosition(0,0,0);
    //Create a slowly rotating disk
    pman.placeBall(nparticles,1.0,0.6); */
