/*
 Get Parameters from .rootrc
*/
// STD libs
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <vector>
#include <string>
#include <algorithm>
#include <iterator>
#include <map>

class Readrootrc{
    public:
        Readrootrc();
        std::vector<float> Get_p1();
        std::vector<float> Get_df();
    private:
        std::vector<float> p1;
        std::vector<float> df;
        std::vector<std::string> paramvec;
        std::map<std::string, float> parammap;
        std::vector<std::string> seperated_paramvec;
};

Readrootrc::Readrootrc(){

    std::string line;
    std::ifstream parameters(".rootrc", std::ios::in);
    // Read File
    if (parameters.good() && parameters.is_open()){
        while(!parameters.eof()){
            std::getline(parameters, line);
            paramvec.push_back(line);
        }
        // Seperate Strings
        // Change to iterators
        for (int i = 0; i < int(paramvec.size()); ++i){
            std::istringstream iss(paramvec[i]);
            copy(std::istream_iterator<std::string>(iss),
                 std::istream_iterator<std::string>(),
                 std::back_inserter(seperated_paramvec));
        }
        // Convert to right type
        //change to iterators
        float param_value;
        for (int i = 0; i < int(seperated_paramvec.size()) - 1; ++i){
            std::stringstream Str;
            Str << seperated_paramvec[i + 1];
            Str >> param_value;
            parammap.insert( std::pair<std::string, float>(seperated_paramvec[i], param_value));
        }
        // Search for values of interest
        df.push_back(parammap.find("calib.cpg.1.df:") -> second);
        df.push_back(parammap.find("calib.cpg.2.df:") -> second);
        df.push_back(parammap.find("calib.cpg.3.df:") -> second);
        df.push_back(parammap.find("calib.cpg.4.df:") -> second);
        p1.push_back(parammap.find("calib.cpg.1.p.1:") -> second);
        p1.push_back(parammap.find("calib.cpg.2.p.1:") -> second);
        p1.push_back(parammap.find("calib.cpg.3.p.1:") -> second);
        p1.push_back(parammap.find("calib.cpg.4.p.1:") -> second);

        std::cout << "Read from .rootrc successfull" << std::endl;
    }
    else{
        for (int i=0; i<4; ++i){
            df.push_back(1.0);
            p1.push_back(1.0);
        }
        std::cout << ".rootrc could not be read, all parameters set to default value (1.0)" << std::endl;
    }
}

std::vector<float> Readrootrc::Get_p1(){
    return p1;
}

std::vector<float> Readrootrc::Get_df(){
    return df;
}