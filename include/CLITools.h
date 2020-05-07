#ifndef CLITOOLS_H
#define CLITOOLS_H
#include <iostream>
#include <string>
#include <algorithm>
#include <stdlib.h>

namespace CLITools { 
    struct CLIArgs {
        float dt;
        int nout;
        int ksteps;
        std::string imgdir;
        std::string txtdir;
        std::string infile;
        int box,square,disk,sphere;
        inline void printArgs(){
            std::cout<<"{dt="<<dt<<", nout="<<nout<<", ksteps="<<ksteps<<", imgdir="<<imgdir<<", txtdir="<<txtdir<<", infile="<<infile<<", box="<<box<<", square"<<square<<", disk="<<disk<<", sphere="<<sphere<<"}"<<std::endl;
        }
    };


    //This approach was suggested at https://stackoverflow.com/a/868894
    inline char* getOption(char ** begin, char ** end, const std::string & option) {
        char ** itr = std::find(begin, end, option);
        if (itr != end && ++itr != end) {
            return *itr;
        }
        return nullptr;
    }

    inline bool optionExists(char** begin, char** end, const std::string& option) {
        return std::find(begin, end, option) != end;
    }

    /* Helper function for parseCLIArgs.
     * arg and argv are just the inputs from main. argc must be greater than one.
     * option is the option to find and parse. eg to parse "-ksteps 12" you'd call parseIntWithError(argc,argv,"-ksteps")
     * Some values (nstep, ksteps) should be strictly positive. Some values (square,ball,etc) can be zero, so that's what
     * the last argument is for. */
    inline void parseIntWithError(int argc, char *argv[],const std::string& option, int& dest, bool failOnZero=true){
        char *mystr=getOption(argv+1,argv+argc,option);
        if(mystr) {
            dest=strtol(mystr,nullptr,10);
            if(dest<0 || (failOnZero && dest==0)) {
                std::cerr<<"Error in command line argument "<<option<<". invalid format given: \""<<mystr<<"\"."<<std::endl;
                exit(3);
            }
        }
    }

    //Same thing but for dest.
    inline void parseFloatWithError(int argc, char *argv[],const std::string& option, float& dest){
        char *mystr=getOption(argv+1,argv+argc,option);
        if(mystr){
            dest=strtof(mystr,nullptr);
            if(dest<=0) {
                std::cerr<<"Error in command line argument "<<option<<". invalid format given: \""<<mystr<<"\"."<<std::endl;
                exit(3);
            }
        }
    }


    inline CLIArgs parseCLIArgs(int argc, char *argv[]){
        CLIArgs o = {0.01,1,1,"","","",0,0,0,0};
        if(argc>1) {
            parseFloatWithError(argc,argv,"-dt",o.dt);

            parseIntWithError(argc,argv,"-nout",o.nout,true);

            parseIntWithError(argc,argv,"-ksteps",o.ksteps,true);

            //parse the three optional filename arguments.
            //TODO: Add checks on file directory validity
            char *imgdir=getOption(argv+1,argv+argc,"-imgdir");
            if(imgdir){
                o.imgdir=std::string(imgdir);
            }

            //parse filename
            char *txtdir=getOption(argv+1,argv+argc,"-txtdir");
            if(txtdir){
                o.txtdir=std::string(imgdir);
            }

            //parse filename
            char *infile=getOption(argv+1,argv+argc,"-in");
            if(infile){
                o.infile=std::string(infile);
            }

            parseIntWithError(argc,argv,"-box",o.box,false);
            parseIntWithError(argc,argv,"-square",o.square,false);
            parseIntWithError(argc,argv,"-disk",o.disk,false);
            parseIntWithError(argc,argv,"-sphere",o.sphere,false);
        }
        return o;
    }
}

#endif
