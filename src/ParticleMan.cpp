#include "ParticleMan.h"
#include "ImageUtil.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <random>
#include <math.h>
#include <algorithm>

//std::vector<PointMass> masses;

ParticleManager::ParticleManager() : x(0),y(0),z(0),vx(0),vy(0),vz(0),pM(1) { 

    //initialize n,e1,e2.
    setCursorDirection(0,0,1);
}

void ParticleManager::save(std::string filename,const std::vector<PointMass> *data){
    if(data==nullptr)
        data=&masses;
    std::ofstream pfile;
    pfile.open (filename.c_str());
    if(pfile.is_open()){
        for(size_t i=0;i<data->size();i++){
            const PointMass &p=data->at(i);
            pfile<<p.m<<" "<<p.x<<" "<<p.y<<" "<<p.z<<" "<<p.vx<<" "<<p.vy<<" "<<p.vz<<"\n";
        }
        pfile.close();
    } else {std::cerr<<"Could not open file "<<filename<<" for particle data writing";}
}

void  ParticleManager::load(std::string filename,std::vector<PointMass> *data){
    if(data==nullptr){
        data=&masses;
    }
    std::ifstream pfile;
    pfile.open (filename.c_str());
    if(pfile.is_open()){
        *data=std::vector<PointMass>();

        for(std::string line; std::getline(pfile, line); )   //read stream line by line
        {
            std::istringstream in(line);      //make a stream for the line itself
            PointMass p;
            in>>p.m>>p.x>>p.y>>p.z>>p.vx>>p.vy>>p.vz;

            (*data).push_back(p);
        }

        pfile.close();
    } else {std::cerr<<"Could not open file "<<filename<<" for particle data writing";}
}

void getBounds(std::vector<PointMass> &masses, float &x0,float &y0, float &z0, float &x1, float &y1, float &z1){
    x0=y0=z0=1e10;
    x1=y1=z1=-1e10;
    for(size_t i=0;i<masses.size();i++){
        x0=std::min(x0,masses[i].x);
        y0=std::min(y0,masses[i].y);
        z0=std::min(z0,masses[i].z);
        x1=std::max(x1,masses[i].x);
        y1=std::max(y1,masses[i].y);
        z1=std::max(z1,masses[i].z);
    }
}

void ParticleManager::saveParticlesAsImage(int sizex, int sizey, std::string filename, float cx, float cy, float userscale){
    DoubleImage img1(sizex,sizey);
    float xsmall=1e10,ysmall=1e10,xlarge=-1e10,ylarge=-1e10;
    for(size_t i=0;i<masses.size();i++){
        xsmall=std::min(xsmall,masses[i].x);
        ysmall=std::min(ysmall,masses[i].y);
        xlarge=std::max(xlarge,masses[i].x);
        ylarge=std::max(ylarge,masses[i].y);
    }


    float scale;
    if(userscale<=0 ){
        cx=float(0.5)*(xsmall+xlarge);
        cy=float(0.5)*(ysmall+ylarge);
        //Choose a 1:1 aspect ratio and make sure everything fits on screen by choosing the larger of the two possible scale values.
        float scale1=float(sizex)/(xlarge-xsmall);
        float scale2=float(sizey)/(ylarge-ysmall);
        scale=std::min(scale1,scale2);
    }
    scale=userscale;


    for(size_t i=0;i<masses.size();i++){
        int xpixel=(int)floor((masses[i].x-cx)*scale+sizex/2.0);
        int ypixel=(int)floor((masses[i].y-cy)*scale+sizey/2.0);
        //It's safe to call increase even if xpixel<0. The image just won't change.
        img1.increase(xpixel,ypixel,1.0);
    }

    Image img2(sizex,sizey);
    for(int i=0;i<sizex;i++){
        for(int j=0;j<sizey;j++){
            //Result of NSolve[{b Log[a 1+1]==0.5,b Log[a 8+1]==1},{a,b}]
            //1 particle will be half brightness. 8 particles on a spot will be completely white.
            double s=0.256949*log(6*img1.get(i,j)+1);
            //double s=0.36*log(0.5*img1.get(i,j)+1);
            img2.put(i,j,floatToRGB(s*s,s*s,s));
        }
    }
    img2.save(filename);
}

    
void ParticleManager::setCursorPosition(float x, float y, float z) { this->x=x; this->y=y; this->z=z;}
void ParticleManager::setCursorVelocity(float vx, float vy,float vz) { this->vx=vx; this->vy=vy;this->vz=vz;}
//Set direction. Normalize first.
void ParticleManager::setCursorDirection(float nx, float ny, float nz) { 
    float d=sqrt(nx*nx+ny*ny+nz*nz);

    nx/=d;
    ny/=d;
    nz/=d;

    this->nx=nx; 
    this->ny=ny; 
    this->nz=nz;

    d=sqrt(nx*nx+ny*ny);
    if(d>0){
        //A unit vector orthogonal to (nx,ny,nz)
        e1x=-ny/d;
        e1y=nx/d;
        e1z=0;
        //Set e2=Cross(n,e1). This ensures a righthanded coordinate system e1.(e2 cross n)=1
        e2x=-nx*nz/d;
        e2y=-ny*nz/d;
        e2z=d;
    } else {
        //nx and ny are of zero norm, so nz=+-1.
        //Both of these are righthanded coordinate systems
        if(nz>0) {
            e1x=1; e1y=0; e1z=0;
            e2x=0; e2y=1; e2z=0;
        } else {
            e1x=1; e1y=0; e1z=0;
            e2x=0; e2y=-1; e2z=0;
        }
    }

    /*std::cout<<e1x<<" "<<e1y<<" "<<e1z<<std::endl;
    std::cout<<e2x<<" "<<e2y<<" "<<e2z<<std::endl;
    std::cout<<nx<<" "<<ny<<" "<<nz<<std::endl;*/
}
void ParticleManager::setCursorParticleMass(float pM) { this->pM=pM;}

void ParticleManager::placeParticle(){
    masses.push_back({pM,x,y,z,vx,vy,vz});
}

//Place random points inside a sphere https://math.stackexchange.com/a/87238/607269
void ParticleManager::placeBall(int N, float radius, float angvel) {
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<float> dist((float)0.0,(float)1.0);
    std::uniform_real_distribution<float> unif((float)0.0,(float)1.0);

    for(int i=0;i<N;i++){
        //First generate a random point on a sphere
        float X1=dist(mt),X2=dist(mt),X3=dist(mt);
        double distsqrt=sqrt(X1*X1+X2*X2+X3*X3);
        if(distsqrt>0){
            X1=X1/distsqrt;X2=X2/distsqrt;X3=X3/distsqrt;
        }
        //Next generate the radius
        float u=unif(mt);
        float r;
        //Ensure u cannot be zero.
        while(u<=0){
            u=unif(mt);
        }
        r=radius*pow(u,(float)(1.0/3.0));

        //Next generate the position.
        //I don't know if I'm having a brain fart or not, but I think the full matrix multiplication has to be in here.
        //It simplifies the velocity calculation.
        float px=x+(X1*e1x+X2*e2x+X3*nx)*r; 
        float py=y+(X1*e1y+X2*e2y+X3*ny)*r; 
        float pz=z+(X1*e1z+X2*e2z+X3*nz)*r; 
        float r2=sqrt(X1*X1+X2*X2)*r;

        float theta=atan2(X2,X1)+1.57079633;
        float pvx=vx+r2*angvel*(cos(theta)*e1x+sin(theta)*e2x);
        float pvy=vy+r2*angvel*(cos(theta)*e1y+sin(theta)*e2y);
        float pvz=vz+r2*angvel*(cos(theta)*e1z+sin(theta)*e2z);
        masses.push_back({pM,px,py,pz,pvx,pvy,pvz});
    }
}

void ParticleManager::placeGaussianBall(int N, float sdev, float angvel){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<float> dist((float)0.0,sdev);

    for(int i=0;i<N;i++) {
        float X1=dist(mt),X2=dist(mt),X3=dist(mt);
        float r=sqrt(X1*X1+X2*X2+X3*X3);

        float px=x+(X1*e1x+X2*e2x+X3*nx); 
        float py=y+(X1*e1y+X2*e2y+X3*ny); 
        float pz=z+(X1*e1z+X2*e2z+X3*nz); 

        float theta=atan2(X2,X1)+1.57079633;
        float pvx=vx+r*angvel*(cos(theta)*e1x+sin(theta)*e2x);
        float pvy=vy+r*angvel*(cos(theta)*e1y+sin(theta)*e2y);
        float pvz=vz+r*angvel*(cos(theta)*e1z+sin(theta)*e2z);
        masses.push_back({pM,px,py,pz,pvx,pvy,pvz});
    }
}

void ParticleManager::placeDisk(int N,float radius, float angvel){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<float> dist((float)0.0,(float)1.0);
    std::uniform_real_distribution<float> unif((float)0.0,(float)1.0);

    for(int i=0;i<N;i++){
        //First generate a random point on a circle
        float X1=dist(mt),X2=dist(mt);
        double distsqrt=sqrt(X1*X1+X2*X2);
        if(distsqrt>0){
            X1=X1/distsqrt;X2=X2/distsqrt;
        }

        float u=unif(mt);
        float r;
        //Ensure u cannot be zero.
        while(u<=0){
            u=unif(mt);
        }
        r=radius*pow(u,(float)(1.0/2.0));

        //Next generate the position.
        float px=x+(X1*e1x+X2*e2x)*r; 
        float py=y+(X1*e1y+X2*e2y)*r; 
        float pz=z+(X1*e1z+X2*e2z)*r; 
        //std::cout<<X1<<" "<<X2<<" "<<u<<" "<<r<<std::endl;

        float theta=atan2(X2,X1)+1.57079633;
        float pvx=vx+r*angvel*(cos(theta)*e1x+sin(theta)*e2x);
        float pvy=vy+r*angvel*(cos(theta)*e1y+sin(theta)*e2y);
        float pvz=vz+r*angvel*(cos(theta)*e1z+sin(theta)*e2z);
        masses.push_back({pM,px,py,pz,pvx,pvy,pvz});
    }
}
void ParticleManager::placeGaussianDisk(int N, float sdev, float angvel){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<float> dist((float)0.0,sdev);

    for(int i=0;i<N;i++){
        //First generate a random point on a circle
        float X1=dist(mt),X2=dist(mt);
        //Next generate the radius. This time the "2" for "2 dimensions"
        float r=sqrt(X1*X1+X2*X2);

        float px=x+(X1*e1x+X2*e2x); 
        float py=y+(X1*e1y+X2*e2y); 
        float pz=z+(X1*e1z+X2*e2z); 

        float theta=atan2(X2,X1)+1.57079633;
        float pvx=vx+r*angvel*(cos(theta)*e1x+sin(theta)*e2x);
        float pvy=vy+r*angvel*(cos(theta)*e1y+sin(theta)*e2y);
        float pvz=vz+r*angvel*(cos(theta)*e1z+sin(theta)*e2z);
        masses.push_back({pM,px,py,pz,pvx,pvy,pvz});
    }
}

void ParticleManager::placeBox(int N,float xmin, float ymin, float zmin, float xmax, float ymax, float zmax,float vdispersion){
    std::random_device rd;
    std::mt19937 mt(rd());
    std::normal_distribution<float> dist((float)0.0,(float)1.0);
    std::uniform_real_distribution<float> unif((float)0.0,(float)1.0);

    for(int i=0;i<N;i++){
        //First generate a random point on a circle
        float X1=unif(mt),X2=unif(mt),X3=unif(mt);
        float px=x+(xmax-xmin)*X1+xmin; 
        float py=y+(ymax-ymin)*X2+ymin; 
        float pz=z+(zmax-zmin)*X3+zmin; 

        float V1=dist(mt),V2=dist(mt),V3=dist(mt);
        float pvx=vx+V1*vdispersion;
        float pvy=vy+V2*vdispersion;
        float pvz=vz+V3*vdispersion;

        masses.push_back({pM,px,py,pz,pvx,pvy,pvz});
    }
}
