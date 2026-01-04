#include <iostream>
#include "configs/SearchConfig.hpp"
#include "folders_inspector/FoldersInspector.hpp"

#include <core/Core.hpp>

#include <fstream>



int main() {



    Core core;
    int prob;

    SearchConfig::get().amount_of_search_threads = 1;
    core.resizeSearchPool();
    
    core.startSeacrhing();

    std::cin >> prob;


    
    
    /*duckx::Document doc("C://src/12.docx");

    doc.open();
    int par = 0;
    for (auto p : doc.paragraphs())
    {
        par++;
        for (auto r : p.runs())
        {
            std::cout << par <<" : "<<format(r.get_text()) << std::endl;
        }
            
    }
        
    std::cout << "END" << std::endl;*/

   /* try {
        UnifiedReader reader("C://src/12.docx");

        while (!reader.empty()) {
            std::cout << reader.readWord() << std::endl;
            reader.moveToNextWord();
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }*/

    /*try {
        UnifiedReader reader("C://src/1.txt");

        std::cout << "File size: " << reader.size() << " bytes\n";

        while (!reader.empty()) {
            std::cout << reader.readWord() << std::endl;
            reader.moveToNextWord();
        }

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
	
	FoldersInspector insepctor;

	insepctor.walk();*/
    

	return 0;
}