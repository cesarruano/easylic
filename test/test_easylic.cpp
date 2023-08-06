#include <string>
#include <vector>
#include <iostream>

#include "easylic.hpp"

using namespace::EasyLic;

bool test_csv()
{
    // Set up expiration, UUID, and license ID
    long int expiration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 10000; // Add 10000 seconds for expiration
    std::string uuid = get_uuid();
    std::string license_id = "TEST_LICENSE_ID";

    // Generate CSV data
    std::string csv_data = generate_validation_csv(&expiration, &uuid, &license_id);

    // Write CSV data to file
    std::ofstream file("validation.csv");
    if (!file) {
        std::cerr << "File could not be opened\n";
        return 1;
    }
    file << csv_data;
    file.close();

    // Validate the license
    License_activation_requirements lar;
    lar.needs_machine_uuid = true;
    lar.needs_license_id = true;
    lar.expires = true;
    lar.license_id = &license_id;

    // Read CSV data from file
    std::ifstream file_in("validation.csv");
    if (!file_in) {
        std::cerr << "File could not be opened\n";
        return 1;
    }
    std::stringstream buffer;
    buffer << file_in.rdbuf();
    std::string read_csv_data = buffer.str();

    bool valid = validate_csv_license(read_csv_data, lar);

    if (valid) {
        std::cout << "License is valid." << std::endl;
    } else {
        std::cout << "License is not valid." << std::endl;
    }

    return valid;
}

bool test_crypto_csv(){
    long int expiration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + 10000; // Add 10000 seconds for expiration
    std::string uuid = get_uuid();
    std::string license_id = "TEST_LICENSE_ID";
    
    std::string path_to_activation = "./activation.aes";
    
    generate_activation_file(&expiration, &uuid, &license_id, path_to_activation);
    
    
    License_activation_requirements lar;
    lar.needs_machine_uuid = true;
    lar.needs_license_id = true;
    lar.expires = true;
    lar.license_id = &license_id;
    
    bool verification_result =  validate_activation_file("./license_id.txt", lar, path_to_activation);
        
    if (verification_result) {
        std::cout << "License is valid." << std::endl;
    } else {
        std::cout << "License is not valid." << std::endl;
    }
    
    return verification_result;
}

int main(){
    int failed_tests = 0;
    
    if(!test_csv())failed_tests++;
    if(!test_crypto_csv())failed_tests++;
    
    std::cout << "Failed tests: " << failed_tests << std::endl;
}