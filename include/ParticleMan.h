#ifndef PARTICLEMAN_H
#define PARTICLEMAN_H
#include <string>
#include <vector>
#include "phystructs.h"

/* ParticleManager is used to easily save and load particle data.
 * It also has functionality to place new blobs of particles at
 * the "cursor" position. This would make it easy to eg place
 * two spinning disks.
 *
 * Cursor position and velocity defines the average position and 
 * velocity with which to place the new object. Cursor direction 
 * is only used by the disk functions, and defined the unit normal 
 * to the disk.
 *
 * Possible future extensions would be adding more advanced functions
 * to place particles, or compressed saving/loading (which we'll
 * probably want if we get to 10^8 particles or anything like that).
 *
 * Easy TODOs might be placing a Plummer sphere, or a simple self-consistent spinning disk.
 *
 * */
void getBounds(std::vector<PointMass> &masses,float &x0,float &y0, float &z0, float &x1, float &y1, float &z1);

class ParticleManager {
    float x,y,z,vx,vy,vz,pM;
    
    //(e1,e2,n) should be three mutually orthogonal unit vectors forming a righthanded coordinate system
    //This is just used for constructing the disks.
    float e1x,e1y,e1z,e2x,e2y,e2z,nx,ny,nz;
public:
    std::vector<PointMass> masses;

    ParticleManager();

    void save(std::string filename,const std::vector<PointMass> *data=nullptr);
    void load(std::string filename,std::vector<PointMass> *data=nullptr);

    
    void setCursorPosition(float x, float y, float z);
    void setCursorVelocity(float vx, float vy, float vz);
    //normal to whatever disks we want to place.
    //The result will be normalized, so you can pass in any numbers.
    void setCursorDirection(float nx, float ny, float nz);
    void setCursorParticleMass(float pM);

    //Saves a bitmap image with a white dot at every particle position, projected down to the xy plane.
    //Useful for debugging purposes.
    void saveParticlesAsImage(int sizex, int sizey, std::string filename, float cx=0, float cy=0, float userscale=0);

    //Places a single particle
    void placeParticle();
    //Place N particles randomly in a sphere
    void placeBall(int N,float r, float angvel=0);
    //Same but in a Gaussian
    void placeGaussianBall(int N, float sdev, float angvel=0);
    //Same with a uniform distribution in a 2D disk
    void placeDisk(int N,float r, float angvel=0);
    //Same with a Gaussian distribution on a 2D disk
    void placeGaussianDisk(int N, float sdev, float angvel=0);

    void placeBox(int N,float xmin,float ymin, float zmin, float xmax, float ymax, float zmax,float vdispersion=0.01);
};
#endif //PARTICLEMAN_H
