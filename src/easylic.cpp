/*
 * Copyright (c) 2023 Cesar Ruano
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <windows.h>

#include "handy_crypto.hpp"
#include "easylic.hpp"

using namespace::Handy_crypto;

namespace EasyLic{
    
    #include "private_key.h"
    #include "public_key.h"
    #include "aes_key.h"
    
    std::string execute_cmd(const std::string& cmd) {
        std::string result;
        char buffer[128];

        // Open the command as a pipe for reading its output
        FILE* pipe = _popen(cmd.c_str(), "r");
        if (pipe == nullptr) {
            std::cerr << "Failed to execute command." << std::endl;
            return result;
        }

        // Read the output of the command
        while (!feof(pipe)) {
            if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                result += buffer;
            }
        }

        // Close the pipe
        _pclose(pipe);

        return result;
    }

    std::string get_uuid(void) {
        std::string command = "wmic csproduct get UUID";

        std::string output = execute_cmd(command);

        // Find the position of the first newline character
        size_t pos = output.find_first_of("\r\n");
        if (pos != std::string::npos) {
            // Remove the header and leading/trailing whitespaces
            output = output.substr(pos + 1);
            output.erase(0, output.find_first_not_of(" \t\r\n"));
            output.erase(output.find_last_not_of(" \t\r\n") + 1);
        }

        return output;
    }

    std::string get_executable_path(void) {
        std::string path;
    #ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileNameA(nullptr, buffer, MAX_PATH);
        path = buffer;
    #else
        // For non-Windows platforms, you can implement the equivalent method to get the executable path.
        // For example, on Linux, you can use the /proc filesystem or read the symbolic link /proc/self/exe.
        // For simplicity, we'll assume it's the current working directory on non-Windows platforms.
        //FIXME
        path = "./";
    #endif

        // Remove the executable filename from the path
        size_t pos = path.find_last_of("\\/");
        if (pos != std::string::npos) {
            path = path.substr(0, pos + 1);
        }

        return path;
    }

    std::string get_current_time_in_seconds() {
        // Get the current time point
        auto now = std::chrono::system_clock::now();

        // Get the duration in seconds from the Unix epoch to the current time point
        auto seconds_since_epoch = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

        // Convert the seconds to a string
        std::stringstream ss;
        ss << seconds_since_epoch;
        return ss.str();
    }

    int parse_string_to_int(const std::string& s) {
        // Convert the string to an integer
        int i = 0;
        std::stringstream ss(s);
        ss >> i;
        return i;
    }

    std::string generate_csv_from_attributes(const std::vector<Attribute>& attributes) {
        std::ostringstream oss;

        // Write the header
        oss << "Name,Value\n";

        // Write the attributes
        for (const Attribute& attribute : attributes) {
            oss << attribute.name << ',' << attribute.value << '\n';
        }

        return oss.str();
    }

    std::vector<Attribute> generate_attributes_from_csv(const std::string& csv_data) {
        std::vector<Attribute> attributes;
        std::istringstream iss(csv_data);
        std::string line;

        // Skip the header line
        std::getline(iss, line);

        // Process each line
        while (std::getline(iss, line)) {
            std::istringstream line_stream(line);
            std::string name;
            std::string value;

            // Parse the name and value
            std::getline(line_stream, name, ',');
            std::getline(line_stream, value);

            // Add the attribute to the vector
            attributes.push_back({name, value});
        }

        return attributes;
    }

    std::string generate_validation_csv(long int * expiration_pt, std::string * uuid_pt, std::string * license_id_pt){
        std::vector<Attribute> attributes;
        
        std::string uuid;
        if (uuid_pt != NULL){
            uuid = *uuid_pt;
            attributes.push_back(Attribute{"uuid", uuid});
        }
        
        std::string license_id;
        if (license_id_pt != NULL){
            license_id = *license_id_pt;
            attributes.push_back(Attribute{"license_id", license_id});
        }
        
        long int expiration;
        if (expiration_pt != NULL){
            expiration = *expiration_pt;
            attributes.push_back(Attribute{"expiration", std::to_string(expiration)});
        }
        
        std::string csv_data = generate_csv_from_attributes(attributes);
        
        return csv_data;
    }

    Attribute* find_attribute(std::vector<Attribute>& attributes, const std::string& nameToFind) {
        auto it = std::find_if(attributes.begin(), attributes.end(), 
                               [&nameToFind](const Attribute& attribute) {
                                   return attribute.name == nameToFind;
                               });

        if (it != attributes.end()) {
            return &(*it);  // Return pointer to the found attribute
        } else {
            return nullptr; // Not found, return null
        }
    }

    bool validate_csv_license(const std::string& csv_data, License_activation_requirements & lar){
        std::vector<Attribute> attributes = generate_attributes_from_csv(csv_data);
        
        bool valid = true;
        
        if(lar.needs_machine_uuid){
           Attribute * attribute_pt = find_attribute(attributes, "uuid");
           std::cout << attribute_pt->value << std::endl;
           if ((attribute_pt == NULL) || (attribute_pt->value != get_uuid())){
               valid = false;
           }
        }
        
        if(lar.needs_license_id){
           Attribute * attribute_pt = find_attribute(attributes, "license_id");
           std::cout << attribute_pt->value << std::endl;
           if ((attribute_pt == NULL) || (attribute_pt->value != *lar.license_id)){
               valid = false;
           }
        }
        
        if(lar.expires){
           Attribute * attribute_pt = find_attribute(attributes, "expiration");
           std::cout << attribute_pt->value << std::endl;
           if ((attribute_pt == NULL) || (parse_string_to_int(attribute_pt->value) <= parse_string_to_int(get_current_time_in_seconds()))){
               valid = false;
           }
        }
        
        return valid;
    }


    void generate_activation_file(long int * expiration_pt, std::string * uuid_pt, std::string * license_id_pt, std::string path_to_create){
        std::string csv_data = generate_validation_csv(expiration_pt, uuid_pt, license_id_pt);
        std::vector<unsigned char> signature = sign(private_key, csv_data);
        write_data_and_signature_to_file(path_to_create, 
                                         csv_data, 
                                         signature,
                                         aes_encryption_key, 
                                         aes_encryption_iv);
    }

    bool validate_activation_file(std::string license_id,  License_activation_requirements & lar, std::string path_to_activation){
        std::string csv_data;
        std::vector<unsigned char> read_signature;
        extract_data_and_signature_from_file(path_to_activation, 
                                             csv_data, 
                                             read_signature,
                                             aes_encryption_key, 
                                             aes_encryption_iv);
        bool valid_signature = verify(public_key, csv_data, read_signature);
        
        if(!valid_signature){
            return false;
        }
        
        bool valid_activation = validate_csv_license(csv_data, lar);
        
        return valid_activation;
    }
}