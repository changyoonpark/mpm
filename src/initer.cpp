#include "include/initer.h"
#include "include/constants.h"


InitData::InitData(std::string s){
    std::string line;
    std::ifstream inputFile;
    inputFile.open(s);
    n = 0;
    if (inputFile.is_open())
    {
        std::cout << "Reading input file..." << std::endl;
        while ( std::getline (inputFile,line) )
        {
            // std::cout << line << '\n';
            // std::cout << "------------" << "PARTICLE : " << n << "------------" << std::endl;
            auto splittedLine = split(line,' ');

            std::map<std::string,Vector3D> mapToAppend;

            for (std::string entry : splittedLine){

                auto splittedEntry = split(entry,':');
                auto splittedEntry2 = split(splittedEntry[1],',');

                if (splittedEntry2.size() > 1){
                    mapToAppend[splittedEntry[0]] = Vector3D(std::stod(splittedEntry2[0]),
                                                             std::stod(splittedEntry2[1]),
                                                             std::stod(splittedEntry2[2]));
                } else{
                    mapToAppend[splittedEntry[0]] = Vector3D(std::stod(splittedEntry2[0]),
                                                             0.,
                                                             0.);
                }
                // std::cout << splittedEntry[0] << " <- " << mapToAppend[splittedEntry[0]] << std::endl;                
            }

            data.push_back(mapToAppend);
            n++;

        }
        inputFile.close();
    }    

    std::cout << "Reading from " << s << std::endl;

}