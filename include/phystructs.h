#ifndef PHYSTRUCTS_H
#define PHYSTRUCTS_H
struct PointMass
{
    float m,x,y,z,vx,vy,vz,ax,ay,az;
    void stepPosition(float dt){
        x+=vx*dt;
        y+=vy*dt;
        z+=vz*dt;
    }
    void zeroAcceleration(){
        ax=0;
        ay=0;
        az=0;
    }
    void stepVelocity(float dt){
        vx+=ax*dt;
        vy+=ay*dt;
        vz+=az*dt;
    }
};

#endif
