// Include header files
#include "TypeTraits.hpp"
#include "MethodFactory.hpp"
#include "GetPot"
#include "MyException.hpp"

// Include libraries
#include <iostream>
#include <utility>
#include <chrono>
#include <iomanip>

// Type alias
using T = TypeTraits;

int main(int argc, char *argv[]){

    // Check the number of input files
    try{
        if(argc != 3){
            throw MyException("Not enough files provided.");  
        }
    }
    catch(const MyException& e){
        std::cout << e.what() << std::endl;
    };
    
    // Initialize the filename in input
    T::FileNameType filename1 = argv[1];
    T::FileNameType filename2 = argv[2];

    // Time-Varying Shared Frailty Cox Model
    GetPot datafile(filename1.c_str());
    T::IdType id = datafile("Model/id_model", 2);

    // Create the object factory map
    static T::FactoryType methods(RegisteredMethods());
    PrintMethods(methods);

    // Call the model
    try{
    std::unique_ptr<TVModel::ModelBase> ptrMethod = MakeLikelihoodMethod(id, filename1.c_str(), filename2.c_str());

    // Measure time elapsed
    const auto start = std::chrono::steady_clock::now();
    ptrMethod -> evaluate_loglikelihood();
    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<T::VariableType> elapsed_seconds = end - start;
    std::cout << "Elapsed time: " << resetiosflags(std::ios::scientific) << elapsed_seconds.count() << "s" << std::endl; 
    }
    catch(const MyException& e) {
        std::cout << e.what() << std::endl;
    };
    
    return 0;
}
